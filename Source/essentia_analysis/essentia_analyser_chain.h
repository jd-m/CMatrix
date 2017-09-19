#ifndef essentia_analyser_chain_h
#define essentia_analyser_chain_h

#include "essentia_analysers.h"
#include "essentia_analysis_gates.h"

#include "jdHeader.h"


class AnalyserChain {
public:

    void init (float sampleRate, int samplesPerBlock);
    
    void computeBlock (float* signalToAnalyse);
    
    float getPitch();
    float getPitchConfidence();
    float getPitchSalience();
    float getInharmonicity();
private:
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
    
};

#endif /* essentia_analyser_chain_h */
