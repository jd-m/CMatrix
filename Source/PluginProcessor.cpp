#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Jd_cmatrixAudioProcessor::Jd_cmatrixAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                       .withInput("Sidechain", AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    
    for (int i = 0; i < util::NUM_DETECTORS; i++) {
        auto w = new SignalDrawer();
        waveformViewers.add(w);
        w->setSamplesToAverage(128);
        
        auto newConvolver = new MultiChannelConvolver();
        newConvolver->setNumChannels(numChannels);
        convolvers.add(newConvolver);
    }
    
    for (auto& env : convolutionEnvelopes) {
        env.adsr(0.1f,0.1f, jd::dbamp(-6.f), 2.f);
    }
    
    //Triggering
    detectorIsEnabled.fill(false);
    convolutionEnabled.fill(false);
    entryToRangeTriggered.fill(false);
    shouldReverseEnabledRange.fill(false);
    envelopeModes.fill(oneShot);
    
    triggerCooldowns.fill(0);
    triggerCooldownTimes.fill(100);
}

Jd_cmatrixAudioProcessor::~Jd_cmatrixAudioProcessor()
{
}

//==============================================================================
const String Jd_cmatrixAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool Jd_cmatrixAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool Jd_cmatrixAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

double Jd_cmatrixAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int Jd_cmatrixAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int Jd_cmatrixAudioProcessor::getCurrentProgram()
{
    return 0;
}

void Jd_cmatrixAudioProcessor::setCurrentProgram (int index)
{
}

const String Jd_cmatrixAudioProcessor::getProgramName (int index)
{
    return String();
}

void Jd_cmatrixAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void Jd_cmatrixAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    using namespace util;
    
    releaseResources();
    
    controlBlockSize = std::min(targetControlBlocksize, samplesPerBlock);

    for (auto i : audioRateDetectors) {
        auto& d = detectors[i];
        d.init(sampleRate, sampleRate, samplesPerBlock);
        d.setRange(0.f, 0.f);
        d.setRMSWindowSize(5);
    }
    
    for (auto i : controlRateDetectors) {
        auto& d = detectors[i];
        d.init(sampleRate, sampleRate, controlBlockSize);
        d.setRange(0.f,0.f);
        d.setRMSWindowSize(10);
    }
    
    detectors[PITCH].setInputScalingFunc(jd::midihz<float>);
    detectors[PITCH].setOutputScalingFunc(jd::hzmidi<float>);
    detectors[PITCH].setLimits(freqScale.front(), freqScale.back());
    detectors[PITCH].shouldConvertOutput = true;
    
    detectors[LEVEL].setOutputScalingFunc(jd::dbamp<float>);
    detectors[LEVEL].setLimits(logAmpScale.front(), logAmpScale.back());
    detectors[LEVEL].setInputScalingFunc([] (float x){
        return jd::ampdb(jd::clip(x,
                                  jd::dbamp(-60.f),
                                  jd::dbamp(6.f)));
    });
    detectors[LEVEL].shouldConvertInput = true;
    
    //Analysis
    analysisChain.init(sampleRate, controlBlockSize);

    mixedBuf.resize(samplesPerBlock);
    wetBuffer.setSize(2, samplesPerBlock);
    multiplicationBuffer.setSize(2, samplesPerBlock);
    
    //CONVOLUTION
    for (auto& convEnvBuf : convolutionEnvelopeBuffers)
        convEnvBuf.setSize(1, samplesPerBlock);

    for (auto& env : convolutionEnvelopes) {
        env.init(sampleRate, samplesPerBlock);
    }

    for (int i = 0; i < NUM_DETECTORS; i++) {
        convolvers[i]->prepareToPlay (sampleRate, samplesPerBlock);
    }
    
    wetGainDB.setSampleRate(sampleRate);
    wetGainDB.setDurationS(0.01, 1.f);
    
    dryGainDB.setSampleRate(sampleRate);
    dryGainDB.setDurationS(0.01, 1.f);
    
    inputGainDB.setSampleRate(sampleRate);
    inputGainDB.setDurationS(0.01, 1.f);
    
}

void Jd_cmatrixAudioProcessor::releaseResources()
{
    wetBuffer.clear();

}

#ifndef JucePlugin_PreferredChannelConfigurations
bool Jd_cmatrixAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void Jd_cmatrixAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    using namespace util;
    
    const int numInputChannels  = getTotalNumInputChannels();
    const int numOutputChannels = getTotalNumOutputChannels();
    const int numSamples = buffer.getNumSamples();
    
    const float ** inputs = buffer.getArrayOfReadPointers();
    float** outputs = buffer.getArrayOfWritePointers();

    for (int i = numInputChannels; i < numOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    //SideChain
    getBusBuffer (buffer, true, 1);
    //Input Scale
    
    inputGainDB.updateTarget();
    //Mono Analysis Signal
    for (int i = 0; i < numSamples; i++) {
        
        float sample = 0.f;
        float inputGain = inputGainDB.nextValue();
        
        for (int chan = 0; chan < numOutputChannels; chan++) {
            sample += buffer.getSample(chan, i) * inputGain;
        }
    
        mixedBuf[i] = sample / (float)numInputChannels;
    }
    
    int remaining = numSamples;
    //Analysis
    while (remaining > 0) {
        
        const int numToCopy = std::min (remaining, (int)controlBlockSize);
        const int bufOffset = numSamples - remaining;
        
        //ControlLoop
        analysisChain.computeBlock(&mixedBuf[bufOffset]);
        
        //AudioLoop
        for (int i = 0; i < controlBlockSize; i++)
        {
            //ThresholdChecking
            detectors[PITCH].setInput (analysisChain.getPitch());
            
            detectors[PITCH_CONFIDENCE].setInput (analysisChain.getPitchConfidence());
            
            detectors[PITCH_SALIENCE].setInput (analysisChain.getPitchSalience());
            
            detectors[INHARMONICITY].setInput (analysisChain.getInharmonicity());
            
            detectors[LEVEL].setInput (mixedBuf[bufOffset + i]);
        
            for (int detectorIndex = 0; detectorIndex < NUM_DETECTORS; detectorIndex++) {
                //Waveforms
                detectors[detectorIndex].applySmoothing();
                waveformViewers[detectorIndex]->addSample (detectors[detectorIndex].normalisedScaledOutput());
              
                //CheckOtherDetector in/out of range
                bool allDetectorsMeetRequirements = true;
                
                for (int requiredStateIndex = 0; requiredStateIndex < NUM_DETECTORS; requiredStateIndex++)
                {
                    if (!(requirementsOfOtherDetectors[detectorIndex][requiredStateIndex] == RequiredDetectorState::none) &&
                        convolutionEnabled[requiredStateIndex]
                        )
                    {
                        RequiredDetectorState detectorRangeState;
                        if (detectors[requiredStateIndex].isWithinRange())
                            detectorRangeState = withinRange;
                        else
                            detectorRangeState = outsideRange;
                        
                        bool requirementMetForDetector =
                        requirementsOfOtherDetectors[detectorIndex][requiredStateIndex] == detectorRangeState;
                        
                        allDetectorsMeetRequirements = requirementMetForDetector && allDetectorsMeetRequirements;
                    }
                }
                
                //cool down
                if (triggerCooldowns[detectorIndex] > 0)
                    triggerCooldowns[detectorIndex]--;
                
                //Convolution Envelope
                if (convolutionEnabled[detectorIndex] &&
                    allDetectorsMeetRequirements) {
                    
                    //IF CROSSED A THRESHOLD
                    if (detectors[detectorIndex].crossedThresholdOnLastCheck())
                    {
                        //CHECK TRIGGER CONDITION
                        if (triggerConditions[detectorIndex].at (detectors[detectorIndex].getGateCode())) {
                            
                            if (triggerCooldowns[detectorIndex] == 0) {
                                convolutionEnvelopes[detectorIndex].trigger();
                                triggerCooldowns[detectorIndex] = triggerCooldownTimes[detectorIndex];
                            }
                        }
                        
                        if (releaseConditions[detectorIndex].at(detectors[detectorIndex].getGateCode()))
                            convolutionEnvelopes[detectorIndex].release();
                    }
                    
                    //WRITE GATE VALUES TO BLOCK
                    convolutionEnvelopes[detectorIndex].updateAction();
                    float envSample =  envSmoother[detectorIndex](convolutionEnvelopes[detectorIndex].value());
                    
                    convolutionEnvelopeBuffers[detectorIndex].setSample (0, bufOffset + i, envSample);
                }
                
            }
        }

        remaining -= numToCopy;
    } //END LOOP
    
    // Convolution
    for (int convolverIndex = 0; convolverIndex < util::NUM_DETECTORS; ++convolverIndex)
    {
        if (convolutionEnabled[convolverIndex])
        {
            //Convolve
            convolvers[convolverIndex]->processBlock (inputs, numSamples);
            
            for (int channelIndex = 0; channelIndex < numChannels; ++channelIndex)
            {
                FloatVectorOperations::multiply (multiplicationBuffer.getWritePointer(channelIndex),
                                                 convolutionEnvelopeBuffers[convolverIndex].getReadPointer(0),
                                                 convolvers[convolverIndex]->getChannelData(channelIndex),
                                                 numSamples);
                
                //SUM INTO WET BUFFER
                FloatVectorOperations::add (wetBuffer.getWritePointer (channelIndex),
                                            multiplicationBuffer.getReadPointer (channelIndex),
                                            numSamples);
            }
        }
    }
    
    //SCALE WET BUFFER
    for (int channelIndex = 0; channelIndex < numChannels; ++channelIndex)
    {
        FloatVectorOperations::multiply (wetBuffer.getWritePointer(channelIndex),
                                         0.5f,
                                         numSamples);
    }

    //Mix
    remaining = numSamples;
    while (remaining > 0) {
        const int numToCopy = std::min (remaining, (int)controlBlockSize);
        const int bufOffset = numSamples - remaining;
        
        dryGainDB.updateTarget();
        wetGainDB.updateTarget();
        
        for (int controlBlockIndex = 0; controlBlockIndex < controlBlockSize; ++controlBlockIndex)
        {
            const int audioSampleIndex = bufOffset + controlBlockIndex;
            
            const float dryGainAmpSample = dryGainDB.nextValue();
            const float wetGainAmpSample = wetGainDB.nextValue();
            
            for (int channelIndex = 0; channelIndex < numChannels; ++channelIndex)
            {
                const float wetSample = wetBuffer.getSample (channelIndex, audioSampleIndex) * wetGainAmpSample;
                const float drySample = inputs[channelIndex][audioSampleIndex] * dryGainAmpSample;
                
                outputs[channelIndex][audioSampleIndex] = wetSample + drySample;
            }
        }
        remaining -= numToCopy;
    }
    
}

//==============================================================================
bool Jd_cmatrixAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* Jd_cmatrixAudioProcessor::createEditor()
{
    return new Jd_cmatrixAudioProcessorEditor (*this);
}

//==============================================================================
void Jd_cmatrixAudioProcessor::getStateInformation (MemoryBlock& destData)
{

}

void Jd_cmatrixAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Jd_cmatrixAudioProcessor();
}
