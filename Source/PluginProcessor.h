/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#ifndef PLUGINPROCESSOR_H_INCLUDED
#define PLUGINPROCESSOR_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "Convolver.hpp"
#include "jdHeader.h"
#include "Settings.h"
#include "essentia_analysis.hpp"
#include "AnalysisChain.hpp"
#include <random>
#include <algorithm>
#include <list>

//==============================================================================
/**
*/

class Jd_cmatrixAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    Jd_cmatrixAudioProcessor();
    ~Jd_cmatrixAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioSampleBuffer&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    std::vector<float> mixedBuf;

    //PROCESSOR
    SimpleConvolver convolver;
//    AnalysisChain chain;
    
    //AnalysisChain
    std::vector<float> windowedFrame;
    std::vector<float> spectrumFrame;
    
    FrameCutter frameCutter { windowedFrame };
    SpectrumAnalyser spectrumAnalyser { frameCutter, spectrumFrame};
    
    DCRemover dcRemover { spectrumFrame };
    FFTPitchAnalyser pitchAnalyser { spectrumAnalyser };
    PitchSalienceAnalyser pitchSalienceAnalyser { spectrumAnalyser };
    SpectralPeakAnalysis spectralPeakAnalyser { spectrumAnalyser };
    HarmonicPeakAnalyser harmonicPeakAnalyser { pitchAnalyser, spectralPeakAnalyser };
    
    InharmonicityAnalyser inharmonicityAnalyser { harmonicPeakAnalyser };
    
    
    
    //FOR-GUI
    
    //DEV
    volatile double dbg_meter = 0.;
    jd::Impulse<float> imp;
    jd::SinOsc<float> sin;
    
private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Jd_cmatrixAudioProcessor)
};


#endif  // PLUGINPROCESSOR_H_INCLUDED
