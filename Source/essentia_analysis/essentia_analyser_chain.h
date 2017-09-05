#ifndef essentia_analyser_chain_h
#define essentia_analyser_chain_h

#include "essentia_analysers.h"
#include "essentia_analysis_gates.h"

#include "jdHeader.h"

enum Detectors {
    LEVEL,
    PITCH,
    PITCH_CONFIDENCE,
    PITCH_SALIENCE,
    INHARMONICITY,
    NUM_DETECTORS
};

struct AnalyserChain {
    
    AnalyserChain()
    {
        detectors.resize(NUM_DETECTORS);
        detectorLimits.resize(NUM_DETECTORS);
        envFollowers.resize(NUM_DETECTORS);
        smoothedAnalyserOutputs.reserve(NUM_DETECTORS);
        outputs.resize(NUM_DETECTORS);
        scalingFuncs.resize(NUM_DETECTORS);
        for (auto f : scalingFuncs)
            f = [](float x){ return x;};
        
        scalingFuncs[LEVEL] = jd::ampdb<float>;
        scalingFuncs[PITCH] = jd::hzmidi<float>;
        
        for (auto d: controlRateDetectors)
            smoothedAnalyserOutputs.emplace_back();
    }
    
    //ControlRate
    void init (float sampleRate, float controlRate, int samplesPerBlock) {
        
        createAlgorithms(sampleRate, samplesPerBlock);
        detectorSignal.resize(samplesPerBlock);
        
        for (auto d_i: audioRateDetectors)
        {
            envFollowers[d_i].init(sampleRate, samplesPerBlock);
            envFollowers[d_i].setBufferSizeMS(20);
            detectors[d_i].init(sampleRate, samplesPerBlock);
            smoothedAnalyserOutputs[d_i].setSampleRate(sampleRate);
            smoothedAnalyserOutputs[d_i].setDurationS(0.01, 1);
        }
        
        for (auto d_i: controlRateDetectors)
        {
            
            envFollowers[d_i].init(controlRate, samplesPerBlock);
            envFollowers[d_i].setBufferSizeMS(20);
            detectors[d_i].init(controlRate, samplesPerBlock);
            smoothedAnalyserOutputs[d_i].setSampleRate(sampleRate);
            smoothedAnalyserOutputs[d_i].setDurationS(0.01, 1);
        }
        detectorLimits[PITCH] = {20.f, 10000.f};
        detectorLimits[LEVEL] = {-60.f,6.f};
        
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
             previousDetectorsWithinRange = (previousDetectorsWithinRange
                                             and d.isWithinRange());
         
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
    };
    
    void processBlock(const float *input,  int numSamples) {
        
//        memcpy(&inputSignal[0], input, numSamples * sizeof(float));
//        computeBlock();
//        
//        //ControlLoop
//        smoothedAnalyserOutputs[PITCH].setValue(pitchYinFFT.output<0>());
//        smoothedAnalyserOutputs[PITCH_CONFIDENCE].setValue(pitchYinFFT.output<1>());
//        smoothedAnalyserOutputs[PITCH_SALIENCE].setValue(pitchSalience.output<0>());
//        smoothedAnalyserOutputs[INHARMONICITY].setValue(inharmonicity.output<0>());
//        
//        for (auto d_i : controlRateDetectors)
//            smoothedAnalyserOutputs[d_i].updateTarget();
//        
//        for (auto d_i : controlRateDetectors) {
//            const auto smoothedOutput = smoothedAnalyserOutputs[d_i].nextValue();
//            auto sig = envFollowers[d_i].processedSample(smoothedOutput);
//            detectors[d_i].checkThreshold(sig);
//            outputs[d_i] = smoothedOutput;
//        }
//        
//        //AudioLoop
//        for (int i = 0; i < numSamples; i++)
//        {
//            //??
//            for (auto d_i : audioRateDetectors) {
//                auto sig = envFollowers[LEVEL].processedSample(inputSignal[i]);
//                detectors[LEVEL].checkThreshold(sig);
//                outputs[d_i] = inputSignal[i];
//            }
//        }
        
    }
    
    float normalisedOutput(int detector)
    {
        return detectorLimits[detector].normalise(outputs[detector]);
    }
    
    void setRange (Detectors detectorID, float lower, float upper)
    {
        detectorLimits[detectorID].lower = lower;
        detectorLimits[detectorID].upper = upper;
    }
    
//    template<typename F>
//    struct RangeLimits
//    {
//        RangeLimits& operator () (const F input) {
//            value = input;
//            return *this;
//        }
//
//        void setLimits(F newLower, F newUpper)
//        {
//            lower = newLower;
//            upper = newUpper;
//        }
//        F normalised (F input) {
//            return jd::linlin(input, lower, upper, (F)0., (F)1.);
//        }
//        F normalised () {
//            return normalised(value);
//        }
//        
//        F lower = 0.;
//        F upper = 1.;
//        F value = {0.};
//        //moving lower
//        //moving upper
//    };
    
    std::vector<float> inputSignal;
     
    DCRemoval dcRemoval;
    Windowing windowing;
    Spectrum spectrum;
    SpectralPeaks spectralPeaks;
    HarmonicPeaks harmonicPeaks;
    PitchYinFFT pitchYinFFT;
    Inharmonicity inharmonicity;
    PitchSalience pitchSalience;

    std::vector<float> detectorSignal;
    
    std::vector<Detectors> audioRateDetectors { LEVEL };
    std::vector<Detectors> controlRateDetectors {  PITCH, PITCH_CONFIDENCE, PITCH_SALIENCE, INHARMONICITY };
    
    std::vector<RangeDetector> detectors;
    std::vector<jd::RMSEnvelopeFollower<float>> envFollowers;
    std::vector<jd::RangeLimits<float>> detectorLimits;
    std::vector<jd::SmoothedValue<float>> smoothedAnalyserOutputs;
    std::vector<jd::FloatConversionFunc<float>> scalingFuncs;
    std::vector<float> outputs;
   
};

#endif /* essentia_analyser_chain_h */
