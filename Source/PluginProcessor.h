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
    
    using TriggerCondition = std::array<bool, 4>;
    
    template<class Type>
    using DetectorPropertyArray = std::array<Type, NUM_DETECTORS>;
    
    enum EnvelopeMode {
        oneShot,
        sustain
    };
    
    enum GateCode {
        onExitFromBelow = 0,
        onEntryToBelow,
        onExitToAbove,
        onEntryFromAbove,
        onEntryToRange,
        onExitFromRange
    };
    
    enum RequiredDetectorState
    {
        none,
        withinRange,
        outsideRange,
    };

    std::vector<float> mixedBuf;// for analysis
    
    AudioSampleBuffer wetBuffer;
    AudioSampleBuffer multiplicationBuffer;
    
    bool shouldUseSidechain {false};
    AudioSampleBuffer sideChainBuffer;
    
    int targetControlBlocksize { 128 };
    int controlBlockSize { 0 };
    int loopsPerBlock { 8 };

    RangeDetector gate;
    
    //DEV
    AnalyserChain analysisChain;
    DetectorChain detectors;
    
    mutable CriticalSection convolverMutex;
    
    OwnedArray<StereoConvolver> convolvers;
    DetectorPropertyArray<jd::Envelope<float>> convolutionEnvelopes;
    DetectorPropertyArray<AudioSampleBuffer> convolutionEnvelopeBuffers;
    
    DetectorPropertyArray<bool> entryToRangeTriggered;
    DetectorPropertyArray<bool> convolutionEnabled;
    
    DetectorPropertyArray<TriggerCondition> triggerConditions;
    DetectorPropertyArray<TriggerCondition> releaseConditions;
    DetectorPropertyArray<bool> detectorIsEnabled;
    DetectorPropertyArray<bool> shouldReverseEnabledRange;
    DetectorPropertyArray<EnvelopeMode> envelopeModes;
    DetectorPropertyArray<size_t> triggerCooldowns;
    DetectorPropertyArray<size_t> triggerCooldownTimes;
    
    DetectorPropertyArray<std::array<RequiredDetectorState, NUM_DETECTORS>> requirementsOfOtherDetectors;
    
    //FOR GUI
    OwnedArray<SignalDrawer> waveformViewers;
    
    //levels
    jd::AtomicSmoothedValue<float> dryGainDB {-6.f};
    jd::AtomicSmoothedValue<float> wetGainDB {-6.f};
    jd::AtomicSmoothedValue<float> inputGainDB { -6.f};
    
    std::array<jd::OneZero<float>, 5> envSmoother;
    
    //TEST SIGNALS
    volatile double dbg_meter = 0.;
    jd::Impulse<float> imp;
    jd::Phasor<float> sin;
    
private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Jd_cmatrixAudioProcessor)
};


#endif  // PLUGINPROCESSOR_H_INCLUDED
