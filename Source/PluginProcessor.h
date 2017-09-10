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
    //==============================================================================
    
    using TriggerCondition = jd::GateDouble<float>::GateCrossingCode;
    
    enum EnvelopeMode {
        oneShot,
        sustain
    };
    
    std::vector<float> mixedBuf;// for analysis
    
    AudioSampleBuffer wetBuffer;
    
    int controlBlockSize {0};
    int loopsPerBlock { 8 };

    
    RangeDetector gate;
    
    //DEV
    AnalyserChain analysisChain;
    DetectorChain detectors;
    
    mutable CriticalSection convolverMutex;
    
    OwnedArray<StereoConvolver> convolvers;
    std::array<jd::Envelope<float>, NUM_DETECTORS> convolutionEnvelopes;
    std::array<AudioSampleBuffer, NUM_DETECTORS> convolutionEnvelopeBuffers;
    std::array<bool, NUM_DETECTORS> convolutionTriggered;
    std::array<bool, NUM_DETECTORS> convolutionEnabled;
    
    std::array<TriggerCondition, NUM_DETECTORS> meterTriggerConditions;
    std::array<bool, NUM_DETECTORS> detectorEnabled;
    
    //FOR GUI
    OwnedArray<SignalDrawer> waveformViewers;
    
    //levels
    float inputLeveldB {-6.f};
    float masterLeveldB {-6.f};
    float mad18dB {-18.f};
    
    //TEST SIGNALS
    volatile double dbg_meter = 0.;
    jd::Impulse<float> imp;
    jd::Phasor<float> sin;
    
private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Jd_cmatrixAudioProcessor)
};


#endif  // PLUGINPROCESSOR_H_INCLUDED
