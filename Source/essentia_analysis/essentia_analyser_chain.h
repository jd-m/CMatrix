#ifndef essentia_analyser_chain_h
#define essentia_analyser_chain_h

#include "essentia_analysers.h"
#include "essentia_analysis_gates.h"



struct AnalyserChain {
     
    AnalyserChain()
     {
         detectors.resize(NUM_DETECTORS);
         detectorLimits.resize(NUM_DETECTORS);
         for (int i = 0; i < NUM_DETECTORS; i++)
         {
             detectors[i].setRMSWindowSizeMS(1000);
         }
     }
     
     void createAlgorithms(float sampleRate, int samplesPerBlock)
     {
         using AlgorithmFactory = essentia::standard::AlgorithmFactory;
         
         if (!essentia::isInitialized()) essentia::init();
         
         AlgorithmFactory& factory = AlgorithmFactory::instance();
         
         dcRemoval.setAlgorithm(    factory.create("DCRemoval",
                                                   "sampleRate", sampleRate ));
         
         windowing.setAlgorithm(    factory.create("Windowing",
                                                   "type", "hann",
                                                   "zeroPadding", 0));
         
         spectrum.setAlgorithm(     factory.create("Spectrum",
                                                   "size", samplesPerBlock));
         
         spectralPeaks.setAlgorithm(factory.create("SpectralPeaks",
                                                   "maxPeaks", 10000,
                                                   "magnitudeThreshold", (float)0.00001f,
                                                   "minFrequency", 40.f,
                                                   "maxFrequency", 5000.f,
                                                   "orderBy", "frequency"));
         
         inharmonicity.setAlgorithm(factory.create("Inharmonicity"));
         
         pitchYinFFT.setAlgorithm(  factory.create("PitchYinFFT",
                                                   "frameSize", samplesPerBlock,
                                                   "sampleRate", sampleRate));
         
         pitchSalience.setAlgorithm(factory.create("PitchSalience",
                                                   "highBoundary", 4100.f,
                                                   "lowBoundary", 100.f,
                                                   "sampleRate", sampleRate));
         
         harmonicPeaks.setAlgorithm(factory.create("HarmonicPeaks") );
         
         
         inputSignal.resize(samplesPerBlock);
         dcRemoval.setInputs("signal", inputSignal);
         
         windowing.setInputs("frame", dcRemoval.output<DCRemoval::SIGNAL>());
         windowing.output<Windowing::FRAME>().resize(samplesPerBlock);
         
         spectrum.setInputs("frame", windowing.output<Windowing::FRAME>());
         spectrum.output<Spectrum::SPECTRUM>().resize(samplesPerBlock/2+1);
         
         spectralPeaks.setInputs("spectrum", spectrum.output<Spectrum::SPECTRUM>());
         
         pitchYinFFT.setInputs("spectrum", spectrum.output<Spectrum::SPECTRUM>());
         
         harmonicPeaks.setInputs("frequencies", spectralPeaks.output<SpectralPeaks::FREQUENCIES>(),
                                 "magnitudes", spectralPeaks.output<SpectralPeaks::MAGNITUDES>(),
                                 "pitch", pitchYinFFT.output<PitchYinFFT::PITCH>());
        
         
         inharmonicity.setInputs("frequencies",
                                 harmonicPeaks.output<HarmonicPeaks::FREQUENCIES>(),
                                 "magnitudes",
                                 harmonicPeaks.output<HarmonicPeaks::MAGNITUDES>());
         
         pitchSalience.setInputs("spectrum", spectrum.output<Spectrum::SPECTRUM>());
         
         
         //Detector
         detector(LEVEL).init(sampleRate, samplesPerBlock);
         for (auto i : {
             PITCH,
             PITCH_CONFIDENCE,
             PITCH_SALIENCE,
             INHARMONICITY })
             {
                 detector(i).init(sampleRate / static_cast<float>(samplesPerBlock),
                        samplesPerBlock);
             }
         
         detectorSignal.resize(samplesPerBlock);
     }
    
     enum Detectors {
         LEVEL,
         PITCH,
         PITCH_CONFIDENCE,
         PITCH_SALIENCE,
         INHARMONICITY,
         NUM_DETECTORS
     };
    
     bool allDetectorsWithinRange ()
     {
         bool previousDetectorsWithinRange { false };
         
         for (auto d : detectors)
             previousDetectorsWithinRange = (previousDetectorsWithinRange and d.isWithinRange());
         
         return previousDetectorsWithinRange;
     }
     
    RangeDetector& detector(int detectorIndex)
    {
        return detectors[detectorIndex];
    }
  
    void computeBlock () {
        
         dcRemoval.compute();
         windowing.compute();
         spectrum.compute();
         spectralPeaks.compute();
         harmonicPeaks.compute();
         inharmonicity.compute();
         pitchYinFFT.compute();
         pitchSalience.compute();
        
        for (int i = 0; i < inputSignal.size(); i++)
        {
            auto& lims = detectorLimits[LEVEL];
            auto input = inputSignal[i];
            auto normalisedInput = lims.normalised(input);
            auto normalisedOutput = detector(LEVEL).processSample(normalisedInput);
            detectorSignal[i] = normalisedOutput;
        }
//         detector(LEVEL).processBlock(&inputSignal[0], &detectorSignal[0], inputSignal.size());
         detector(PITCH).processedSample(pitchYinFFT.output<PitchYinFFT::PITCH>(),0);
         detector(PITCH_CONFIDENCE).processedSample(pitchYinFFT.output<PitchYinFFT::PITCH_CONFIDENCE>(),0);
         detector(INHARMONICITY).processedSample(inharmonicity.output<Inharmonicity::INHARMONICITY>(),0);
         detector(PITCH_SALIENCE).processedSample(pitchSalience.output<PitchSalience::PITCH_SALIENCE>(),0);
    };
     
    std::vector<float> inputSignal;
     
    DCRemoval dcRemoval;
    Windowing windowing;
    Spectrum spectrum;
    SpectralPeaks spectralPeaks;
    HarmonicPeaks harmonicPeaks;
    PitchYinFFT pitchYinFFT;
    Inharmonicity inharmonicity;
    PitchSalience pitchSalience;
     
    std::vector<RangeDetector> detectors;
    
    void setRange (Detectors detectorID, float lower, float upper)
    {
        detectorLimits[detectorID].lower = lower;
        detectorLimits[detectorID].upper = upper;
    }
    
    template<typename F>
    struct RangeLimits
    {
        F normalised (F input) {
            return jd::linlin(input, lower, upper, (F)0., (F)1.);
        }
        F lower = 0.;
        F upper = 1.;
    };
    std::vector<RangeLimits<float>> detectorLimits;
    std::vector<float> detectorSignal;
   
};

#endif /* essentia_analyser_chain_h */
