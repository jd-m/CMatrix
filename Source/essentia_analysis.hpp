//
//  jdAnalysis.hpp
//  jdConvolver2
//
//  Created by Jaiden Muschett on 05/08/2017.
//
//

#ifndef jdAnalysis_hpp
#define jdAnalysis_hpp

#include <stdio.h>
#include <essentia/essentia.h>
#include <essentia/algorithmfactory.h>
#include <essentia/essentiamath.h> // for the isSilent function
#include <essentia/pool.h>
#include <string>
#include <mutex>

class Analyser {
public:
    using jdAlgorithmFactory = essentia::standard::AlgorithmFactory;
    using jdAlgorithm = essentia::standard::Algorithm;
    Analyser(){
        if (!essentia::isInitialized()) essentia::init();
    }
    virtual ~Analyser(){}
    void init (float sampleRate, int blockSize)
    {
        jdAlgorithmFactory& factory = jdAlgorithmFactory::instance();
        createAlgorithm(factory, sampleRate, blockSize);
    }
    virtual void createAlgorithm (jdAlgorithmFactory& ,
                                  float sampleRate,
                                  int blockSize) = 0;
    virtual void analyseBlock (const float* input, int numSamples) = 0;
    
    void processBlock (const float* input, int numSamples) {
        analyseBlock(input, numSamples);
    }
};
//=====================================================================
class SpectrumAnalyser : public Analyser {
public:
    using jdAlgorithmFactory = essentia::standard::AlgorithmFactory;
    using jdAlgorithm = essentia::standard::Algorithm;
    using jdAlgorithmPtr = std::unique_ptr<jdAlgorithm>;
  
    void createAlgorithm (jdAlgorithmFactory& factory,
                          float sampleRate,
                          int blockSize) override
    {
        m_windowedFrame.resize(blockSize);
        m_spectrumFrame.resize(blockSize);
        
        m_windowAlgo = jdAlgorithmPtr (factory.create("Windowing",
                                      "type", "hann",
                                      "zeroPadding", 0));
        
        m_spectrumAlgo = jdAlgorithmPtr (factory.create("Spectrum"
                                        ,"size", blockSize));
        
        m_windowAlgo->input("frame").set(m_windowedFrame);
        m_windowAlgo->output("frame").set(m_windowedFrame);
        
        m_spectrumAlgo->input("frame").set(m_windowedFrame);
        m_spectrumAlgo->output("spectrum").set(m_spectrumFrame);
    }
    
    void analyseBlock (const float* input, int numSamples) override
    {
        memcpy(m_windowedFrame.data(), input, numSamples * sizeof(float));
        m_windowAlgo->compute();
        m_spectrumAlgo->compute();
    }
    
    std::vector<float>& getSpectrum() { return m_spectrumFrame; }
    
    jdAlgorithmPtr m_spectrumAlgo;
    jdAlgorithmPtr m_windowAlgo;
    
    std::vector<float> m_windowedFrame;
    std::vector<float> m_spectrumFrame;
};
//==========================================================================
class FFTPitchAnalyser : public Analyser {
public:
    using jdAlgorithmFactory = essentia::standard::AlgorithmFactory;
    using jdAlgorithm = essentia::standard::Algorithm;
    using jdAlgorithmPtr = std::unique_ptr<jdAlgorithm>;
    
    void createAlgorithm (jdAlgorithmFactory& factory,
                          float sampleRate,
                          int blockSize) override
    {
        spectrumAnalyser.createAlgorithm(factory, sampleRate, blockSize);
        
        pitchDetectAlgo = jdAlgorithmPtr( factory.create("PitchYinFFT",
                                         "frameSize", blockSize,
                                         "sampleRate", sampleRate));

        pitchDetectAlgo->input("spectrum").set(spectrumAnalyser.getSpectrum());
        pitchDetectAlgo->output("pitch").set(m_pitch);
        pitchDetectAlgo->output("pitchConfidence").set(m_pitchConfidence);
    }
    
    void analyseBlock (const float* input, int numSamples) override
    {
        spectrumAnalyser.analyseBlock(input, numSamples);
        pitchDetectAlgo->compute();
    }
    
    float getPitch () { return m_pitch; }
    float getPitchConfidence () { return m_pitchConfidence; }
    
    SpectrumAnalyser spectrumAnalyser;
    
    jdAlgorithmPtr pitchDetectAlgo {nullptr};
    float m_pitch = 0., m_pitchConfidence = 0.;
    
};
//==========================================================================
class PitchSalienceAnalyser : public Analyser {
public:
    using jdAlgorithmFactory = essentia::standard::AlgorithmFactory;
    using jdAlgorithm = essentia::standard::Algorithm;
    using jdAlgorithmPtr = std::unique_ptr<jdAlgorithm>;
    
    void createAlgorithm (jdAlgorithmFactory& factory,
                          float sampleRate,
                          int blockSize) override
    {
        spectrumAnalyser.createAlgorithm(factory, sampleRate, blockSize);
        
        pitchSalienceAlgo = jdAlgorithmPtr (factory.create("PitchSalience"));
        pitchSalienceAlgo->input("spectrum").set(spectrumAnalyser.getSpectrum());
        pitchSalienceAlgo->output("pitchSalience").set(m_pitchSalience);
    }
    
    void analyseBlock (const float* input, int numSamples) override
    {
        spectrumAnalyser.analyseBlock(input, numSamples);
        pitchSalienceAlgo->compute();
    }

    const float getPitchSalience () { return m_pitchSalience; }
    
    SpectrumAnalyser spectrumAnalyser;
    jdAlgorithmPtr pitchSalienceAlgo;
    float m_pitchSalience = 0.f;
};
//==========================================================================
class SpectralPeakAnalysis : public Analyser {
public:
    
    using jdAlgorithmFactory = essentia::standard::AlgorithmFactory;
    using jdAlgorithm = essentia::standard::Algorithm;
    using jdAlgorithmPtr = std::unique_ptr<jdAlgorithm>;

    void createAlgorithm (jdAlgorithmFactory& factory,
                          float sampleRate,
                          int blockSize) override
    {
        spectrumAnalyser.createAlgorithm(factory, sampleRate, blockSize);
        
        spectralPeakAlgo = jdAlgorithmPtr (factory.create("SpectralPeaks",
                                          "magnitudeThreshold", 0.,
                                          "maxFrequency", 5000,
                                          "maxPeaks", 100,
                                          "minFrequency", 0.,
                                          "orderBy", "frequency",
                                          "sampleRate", sampleRate ));

        spectralPeakAlgo->input("spectrum").set(spectrumAnalyser.getSpectrum());
        spectralPeakAlgo->output("frequencies").set(m_frequencies);
        spectralPeakAlgo->output("magnitudes").set(m_magnitudes);
    }
    
    void analyseBlock (const float* input, int numSamples) override
    {
        spectrumAnalyser.analyseBlock(input, numSamples);
        spectralPeakAlgo->compute();
    }

    std::vector<float>& getFrequencies() { return m_frequencies; }
    std::vector<float>& getMagnitudes() { return m_magnitudes; }
    
    SpectrumAnalyser spectrumAnalyser;
    jdAlgorithmPtr spectralPeakAlgo;
    
    std::vector<float> m_frequencies;
    std::vector<float> m_magnitudes;
};
//================================================================
class HarmonicPeakAnalyser : public Analyser {
public:
    using jdAlgorithmFactory = essentia::standard::AlgorithmFactory;
    using jdAlgorithm = essentia::standard::Algorithm;
    using jdAlgorithmPtr = std::unique_ptr<jdAlgorithm>;
    
    void createAlgorithm (jdAlgorithmFactory& factory,
                          float sampleRate,
                          int blockSize) override
    {
        
        m_harmonicFrequencies.resize(20);
        m_harmonicMagnitudes.resize(20);
        
        spectralPeakAnalyser.createAlgorithm(factory, sampleRate, blockSize);
        pitchAnalyser.createAlgorithm(factory, sampleRate, blockSize);
        
//        harmonicPeakAnalyser = jdAlgorithmPtr( factory.create("HarmonicPeaks",
//                                                              "maxHarmonic", 20.,
//                                                              "tolerance", 0.1
//                                                              ));
//        harmonicPeakAnalyser->input("frequencies").set(spectralPeakAnalyser.getFrequencies());
//        harmonicPeakAnalyser->input("magnitudes").set(spectralPeakAnalyser.getMagnitudes());
//        harmonicPeakAnalyser->input("pitch").set(pitchAnalyser.getPitch());
//        
//        harmonicPeakAnalyser->output("harmonicFrequencies").set(m_harmonicFrequencies);
//        harmonicPeakAnalyser->output("harmonicMagnitudes").set(m_harmonicMagnitudes);
        
    }
    void analyseBlock (const float* input, int numSamples) override
    {
        spectralPeakAnalyser.analyseBlock(input, numSamples);
        pitchAnalyser.analyseBlock(input, numSamples);
        
//        harmonicPeakAnalyser->compute();
    }
    
    const std::vector<float>& getHarmonicFrequencies() { return m_harmonicFrequencies; };
    const std::vector<float>& getHarmonicMagnitudes() { return m_harmonicMagnitudes; };
    
    FFTPitchAnalyser pitchAnalyser;
    SpectralPeakAnalysis spectralPeakAnalyser;
    jdAlgorithmPtr harmonicPeakAnalyser;
    
    std::vector<float> m_harmonicFrequencies;
    std::vector<float> m_harmonicMagnitudes;
};
//================================================================
class InharmonicityAnalyser : public Analyser {
public:
    using jdAlgorithmFactory = essentia::standard::AlgorithmFactory;
    using jdAlgorithm = essentia::standard::Algorithm;
    using jdAlgorithmPtr = std::unique_ptr<jdAlgorithm>;
    InharmonicityAnalyser(){}
    ~InharmonicityAnalyser(){}
    
    void createAlgorithm (jdAlgorithmFactory& factory,
                          float sampleRate,
                          int blockSize) override
    {
        harmonicPeakAnalyser.createAlgorithm(factory, sampleRate, blockSize);
        inharmonicityAlgo  = jdAlgorithmPtr (factory.create("Inharmonicity"));
        
        inharmonicityAlgo->input("frequencies").set(harmonicPeakAnalyser.getHarmonicFrequencies());
        inharmonicityAlgo->input("magnitudes").set(harmonicPeakAnalyser.getHarmonicMagnitudes());
        inharmonicityAlgo->output("inharmonicity").set(m_inharmonicity);
    }
    
    void analyseBlock (const float* input, int numSamples) override
    {
        harmonicPeakAnalyser.processBlock(input, numSamples);
        inharmonicityAlgo->compute();
    }
    
    const float getInharmonicity () { return m_inharmonicity; }
    
    HarmonicPeakAnalyser harmonicPeakAnalyser;
    jdAlgorithmPtr inharmonicityAlgo;
    
    float m_inharmonicity = 0.f;
};

#endif /* jdAnalysis_hpp */
