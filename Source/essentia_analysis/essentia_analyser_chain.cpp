//
//  essentia_analyser_chain.cpp
//  jd_CMatrix
//
//  Created by Jaiden Muschett on 18/09/2017.
//
//

#include <stdio.h>
#include "essentia_analyser_chain.h"

void AnalyserChain::init(float sampleRate, int samplesPerBlock)
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
//=====================================================================
void AnalyserChain::computeBlock(float *signalToAnalyse)
{
    
    memcpy(&inputSignal[0], signalToAnalyse, inputSignal.size() * sizeof(float));
    
    dcRemoval.compute();
    windowing.compute();
    spectrum.compute();
    spectralPeaks.compute();
    harmonicPeaks.compute();
    inharmonicity.compute();
    pitchYinFFT.compute();
    pitchSalience.compute();
};
//=====================================================================
float AnalyserChain::getPitch() {
    return pitchYinFFT.output<PitchYinFFT::PITCH>();
}

float AnalyserChain::getPitchConfidence() {
    return pitchYinFFT.output<PitchYinFFT::PITCH_CONFIDENCE>();
}

float AnalyserChain::getPitchSalience() {
    return pitchSalience.output<PitchSalience::PITCH_SALIENCE>();
}

float AnalyserChain::getInharmonicity() {
    return inharmonicity.output<Inharmonicity::INHARMONICITY>();
}

    


