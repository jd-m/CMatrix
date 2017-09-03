#ifndef essentia_analyser_chain_h
#define essentia_analyser_chain_h

#include "essentia_analysers.h"
 struct AnalyserChain {
     
     void createAlgorithms(float sampleRate, int samplesPerBlock)
     {
         
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
         
         
         factory.shutdown();
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
     
     std::vector<float> inputSignal;
     
     DCRemoval dcRemoval;
     Windowing windowing;
     Spectrum spectrum;
     SpectralPeaks spectralPeaks;
     HarmonicPeaks harmonicPeaks;
     PitchYinFFT pitchYinFFT;
     Inharmonicity inharmonicity;
     PitchSalience pitchSalience;
//     Dissonance dissonance;
};
/* 
 enum AlgorithmID {
 DC_REMOVAL,
 WINDOWING,
 SPECTRUM,
 PITCH_YIN_FFT,
 PITCH_SALIENCE,
 SPECTRAL_PEAKS,
 HARMONIC_PEAKS,
 INHARMONICITY,
 DISSONANCE,
 NUM_ALGORITHMS
 };
*/

#endif /* essentia_analyser_chain_h */
