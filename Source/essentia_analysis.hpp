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
#include <iostream>
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
    using jdAlgorithmPtr = std::unique_ptr<essentia::standard::Algorithm>;
    
    
    Analyser(){
        if (!essentia::isInitialized()) essentia::init();
    }
    virtual ~Analyser(){}
    void init (double sampleRate, int blockSize)
    {
        jdAlgorithmFactory& factory = jdAlgorithmFactory::instance();
        createAlgorithm(factory, sampleRate, blockSize);
    }
    virtual void createAlgorithm (jdAlgorithmFactory& ,
                                  double sampleRate,
                                  int blockSize) = 0;
    virtual void analyseBlock () {
        try {
            algorithm->compute();
        } catch (essentia::EssentiaException& e) {
            std::cout << e.what() << std::endl;
        }
    }
    
    template <typename Key, typename Value>
    void set(Key key, Value value)
    {
        algorithm->configure(key, value);
    }
    
    template<typename Key, typename Value, class ... Args>
    void set(Key key, Value value, Args ...args)
    {
        set(key, value);
        set(args...);
    }
    
    jdAlgorithmPtr algorithm;
};

class DCRemover : public Analyser {
    using jdAlgorithmFactory = essentia::standard::AlgorithmFactory;
    using jdAlgorithm = essentia::standard::Algorithm;
    using jdAlgorithmPtr = std::unique_ptr<jdAlgorithm>;
    
public:
    DCRemover (std::vector<float>& sourceInputSignal):
    inputSignal(sourceInputSignal) {}
    
    void createAlgorithm (jdAlgorithmFactory& factory,
                          double sampleRate,
                          int blockSize ) override
    {
        try {
        algorithm = jdAlgorithmPtr (factory.create("DCRemoval",
                                                   "sampleRate", sampleRate) );
        } catch (const essentia::EssentiaException& e) {}
        
        algorithm->input("signal").set(inputSignal);
        algorithm->output("signal").set(m_outputSignal);
    }

    std::vector<float> outputSignal() { return m_outputSignal; }
    
    std::vector<float>& inputSignal;
    std::vector<float> m_outputSignal;
    
};
//=====================================================================
class FrameCutter : public Analyser
{
    using jdAlgorithmFactory = essentia::standard::AlgorithmFactory;
    using jdAlgorithm = essentia::standard::Algorithm;
    using jdAlgorithmPtr = std::unique_ptr<jdAlgorithm>;
    
public:
    std::vector<float>& m_windowedFrame;
    
    FrameCutter(std::vector<float>& sourceFrame):
    m_windowedFrame(sourceFrame)
    {
    }
    
    void createAlgorithm (jdAlgorithmFactory& factory,
                          double sampleRate,
                          int blockSize) override
    {
        algorithm = jdAlgorithmPtr (factory.create("Windowing",
                                                      "type", "hann",
                                                      "zeroPadding", 0));
        
        algorithm->input("frame").set(m_windowedFrame);
        algorithm->output("frame").set(m_windowedFrame);
    }
    
    std::vector<float>& windowedFrame() { return m_windowedFrame; }
};
//=====================================================================
class SpectrumAnalyser : public Analyser {
public:
    using jdAlgorithmFactory = essentia::standard::AlgorithmFactory;
    using jdAlgorithm = essentia::standard::Algorithm;
    using jdAlgorithmPtr = std::unique_ptr<jdAlgorithm>;
    
    SpectrumAnalyser(FrameCutter& sourceFrameCutter,
                     std::vector<float>& sourceSpectrumFrame):
    frameCutter(sourceFrameCutter),
    m_spectrumFrame(sourceSpectrumFrame)
    {}
    
    void createAlgorithm (jdAlgorithmFactory& factory,
                          double sampleRate,
                          int blockSize) override
    {
        algorithm = jdAlgorithmPtr (factory.create("Spectrum",
                                        "size", blockSize));
        
        algorithm->input("frame").set(frameCutter.windowedFrame());
        algorithm->output("spectrum").set(m_spectrumFrame);
    }
    
    void analyseBlock () override {
        try {
            algorithm->compute();
        } catch (essentia::EssentiaException& e) {
            std::cout << e.what() << std::endl;
        }
    }
    
    std::vector<float>& spectrum() { return m_spectrumFrame; }
    
    FrameCutter &frameCutter;
    std::vector<float> &m_spectrumFrame;
};
//==========================================================================
class FFTPitchAnalyser : public Analyser {
public:
    using jdAlgorithmFactory = essentia::standard::AlgorithmFactory;
    using jdAlgorithm = essentia::standard::Algorithm;
    using jdAlgorithmPtr = std::unique_ptr<jdAlgorithm>;
    
    
    FFTPitchAnalyser(SpectrumAnalyser& sourceSpectrumAnalyser):
    spectrumAnalyser(sourceSpectrumAnalyser)
    {}
    
    void createAlgorithm (jdAlgorithmFactory& factory,
                          double sampleRate,
                          int blockSize) override
    {
        algorithm = jdAlgorithmPtr( factory.create("PitchYinFFT",
                                         "frameSize", blockSize,
                                         "sampleRate", sampleRate));

        algorithm->input("spectrum").set(spectrumAnalyser.spectrum());
        algorithm->output("pitch").set(m_pitch);
        algorithm->output("pitchConfidence").set(m_pitchConfidence);
    }

    float pitch () { return m_pitch; }
    float pitchConfidence () { return m_pitchConfidence; }
    
    SpectrumAnalyser &spectrumAnalyser;
    float m_pitch = 0., m_pitchConfidence = 0.;
};
//==========================================================================
class PitchSalienceAnalyser : public Analyser {
public:
    using jdAlgorithmFactory = essentia::standard::AlgorithmFactory;
    using jdAlgorithm = essentia::standard::Algorithm;
    using jdAlgorithmPtr = std::unique_ptr<jdAlgorithm>;
    
    
    PitchSalienceAnalyser(SpectrumAnalyser& sourceSpectrumAnalyser):
    spectrumAnalyser(sourceSpectrumAnalyser)
    {}
    
    void createAlgorithm (jdAlgorithmFactory& factory,
                          double sampleRate,
                          int blockSize) override
    {
//        spectrumAnalyser.createAlgorithm(factory, sampleRate, blockSize);
        
        algorithm = jdAlgorithmPtr (factory.create("PitchSalience"));
        algorithm->input("spectrum").set(spectrumAnalyser.spectrum());
        algorithm->output("pitchSalience").set(m_pitchSalience);
    }

    float pitchSalience () { return m_pitchSalience; }
    
    SpectrumAnalyser &spectrumAnalyser;
    float m_pitchSalience = 0.f;
};
//==========================================================================
class SpectralPeakAnalysis : public Analyser {
public:
    
    using jdAlgorithmFactory = essentia::standard::AlgorithmFactory;
    using jdAlgorithm = essentia::standard::Algorithm;
    using jdAlgorithmPtr = std::unique_ptr<jdAlgorithm>;
    

    SpectralPeakAnalysis(SpectrumAnalyser& sourceSpectrumAnalyser):
    spectrumAnalyser(sourceSpectrumAnalyser)
    {}
    
    void createAlgorithm (jdAlgorithmFactory& factory,
                          double sampleRate,
                          int blockSize) override
    {
        algorithm = jdAlgorithmPtr (factory.create("SpectralPeaks",
                                          "magnitudeThreshold", 0.,
                                          "maxFrequency", 5000,
                                          "maxPeaks", 100,
                                          "minFrequency", 1.,
                                          "orderBy", "frequency",
                                          "sampleRate", sampleRate ));

        algorithm->input("spectrum").set(spectrumAnalyser.spectrum());
        algorithm->output("frequencies").set(m_frequencies);
        algorithm->output("magnitudes").set(m_magnitudes);
    }

    std::vector<float>& frequencies() { return m_frequencies; }
    std::vector<float>& magnitudes() { return m_magnitudes; }
    
    SpectrumAnalyser &spectrumAnalyser;
    
    std::vector<float> m_frequencies;
    std::vector<float> m_magnitudes;
};
//================================================================
class HarmonicPeakAnalyser : public Analyser {
public:
    HarmonicPeakAnalyser(FFTPitchAnalyser& sourcePitchAnalyser,
                         SpectralPeakAnalysis& sourceSpectralPeakAnalysis):
    pitchAnalyser(sourcePitchAnalyser),
    spectralPeakAnalyser (sourceSpectralPeakAnalysis)
    {
        
    }
    using jdAlgorithmFactory = essentia::standard::AlgorithmFactory;
    using jdAlgorithm = essentia::standard::Algorithm;
    using jdAlgorithmPtr = std::unique_ptr<jdAlgorithm>;
    
    
    void createAlgorithm (jdAlgorithmFactory& factory,
                          double sampleRate,
                          int blockSize) override
    {
        algorithm = jdAlgorithmPtr( factory.create("HarmonicPeaks",
                                           "maxHarmonics", 50,
                                           "tolerance", 0.15 ));
        
        algorithm->input("frequencies").set(spectralPeakAnalyser.frequencies());
        algorithm->input("magnitudes").set(spectralPeakAnalyser.magnitudes());
        algorithm->input("pitch").set(pitchAnalyser.pitch());
        
        algorithm->output("harmonicFrequencies").set(m_harmonicFrequencies);
        algorithm->output("harmonicMagnitudes").set(m_harmonicMagnitudes);
        
    }

    std::vector<float>& harmonicFrequencies() { return m_harmonicFrequencies; };
    std::vector<float>& harmonicMagnitudes() { return m_harmonicMagnitudes; };
    
    FFTPitchAnalyser &pitchAnalyser;
    SpectralPeakAnalysis &spectralPeakAnalyser;
    
    std::vector<float> m_harmonicFrequencies;
    std::vector<float> m_harmonicMagnitudes;

};
//================================================================
class InharmonicityAnalyser : public Analyser {
public:
    using jdAlgorithmFactory = essentia::standard::AlgorithmFactory;
    using jdAlgorithm = essentia::standard::Algorithm;
    using jdAlgorithmPtr = std::unique_ptr<jdAlgorithm>;
    
    
    InharmonicityAnalyser(HarmonicPeakAnalyser& sourceHarmonicPeakAnalyser):
    harmonicPeakAnalyser(sourceHarmonicPeakAnalyser)
    {}
    
    void createAlgorithm (jdAlgorithmFactory& factory,
                          double sampleRate,
                          int blockSize ) override
    {
        algorithm  = jdAlgorithmPtr (factory.create("Inharmonicity"));
        
        algorithm->input("frequencies").set(harmonicPeakAnalyser.harmonicFrequencies());
        algorithm->input("magnitudes").set(harmonicPeakAnalyser.harmonicMagnitudes());
        algorithm->output("inharmonicity").set(m_inharmonicity);
    }

    float inharmonicity () { return m_inharmonicity; }
    
    HarmonicPeakAnalyser &harmonicPeakAnalyser;
    
    float m_inharmonicity = 0.f;
};

#endif /* jdAnalysis_hpp */
