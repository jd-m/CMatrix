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
#include "essentia_analysis/essentia_analyser_chain.h"
#include <random>
#include <algorithm>
#include <list>
#include "WaveformViewer.hpp"

//==============================================================================
/**
*/
struct TestGate : public RangeDetector {

    
    void onExitFromBelow(int sampleIndex) override {
            text =  "onExitFromBelow" ;
    }
    void onEntryToBelow(int sampleIndex) override{
            text =  "onEntryToBelow" ;
    }
    void onExitFromAbove(int sampleIndex) override{
            text =  "onExitFromAbove" ;
    }
    void onEntryToAbove(int sampleIndex) override{
            text =  "onEntryToAbove" ;
    }
    
    String text;
};

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
    
    
    AnalyserChain analysisChain;
    //FOR-GUI
    
    //DEV
    TestGate gate;
    volatile double dbg_meter = 0.;
    jd::Impulse<float> imp;
    jd::TriOsc<float> sin;
    
    AudioInputWaveformDisplay waveformViewer;
    LinearSmoothedValue<float> pitchSalience;
    
private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Jd_cmatrixAudioProcessor)
};


#endif  // PLUGINPROCESSOR_H_INCLUDED
