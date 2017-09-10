#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Jd_cmatrixAudioProcessor::Jd_cmatrixAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    for (int i = 0; i < NUM_DETECTORS; i++) {
        auto w = new SignalDrawer();
        waveformViewers.add(w);
        
        auto newStereoConvolver = new StereoConvolver();
        convolvers.add (newStereoConvolver);
    }
    
    for (auto& env : convolutionEnvelopes) {
        env.adsr(0.1f,0.1f, jd::dbamp(-6.f), 2.f);
    }
    
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
    
    controlBlockSize = (samplesPerBlock / loopsPerBlock);
    double controlRate = (sampleRate / samplesPerBlock) * loopsPerBlock;
    
    std::cout << "\nsampleRate: " << sampleRate <<
    "\ncontrolBlockSize: " << controlBlockSize <<
    "\nloopsPerBlock: " << loopsPerBlock <<
    "\ncontrolRate: "  << controlRate << std::endl;
    
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
    detectors[PITCH].shouldConvertInput = false;
    
    
    detectors[LEVEL].setOutputScalingFunc(jd::dbamp<float>);
    detectors[LEVEL].setLimits(logAmpScale.front(), logAmpScale.back());
    detectors[LEVEL].setInputScalingFunc([] (float x){
        return jd::ampdb(jd::clip(x,
                                  jd::dbamp(-60.f),
                                  jd::dbamp(6.f)));
    });
    detectors[LEVEL].shouldConvertInput = true;
    
    for (auto w : waveformViewers)
        w->setSamplesToAverage(128);

    //Analysis
    analysisChain.init(sampleRate, sampleRate, controlBlockSize);

    mixedBuf.resize(samplesPerBlock);
    wetBuffer.setSize(2, samplesPerBlock);
    
    //Triggering
    detectorIsEnabled.fill(false);
    convolutionTriggered.fill(false);
    
    //CONVOLUTION
    for (auto& convEnvBuf : convolutionEnvelopeBuffers)
        convEnvBuf.setSize(1, samplesPerBlock);

    for (auto& env : convolutionEnvelopes) {
        env.init(sampleRate, samplesPerBlock);
    }

    //Testing
    imp.init(sampleRate);
    imp.setFrequency(0.5f);
    
    sin.init(sampleRate);
    sin.setFrequency(0.125f);
    sin.setAmplitude(2.f);
 
    
    for (int i = 0; i < NUM_DETECTORS; i++) {
        convolvers[i]->prepareToPlay (sampleRate, samplesPerBlock);
    }
    
}

void Jd_cmatrixAudioProcessor::releaseResources()
{
    for (auto w :waveformViewers)
        w->clear();
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

    //Input Scale
    
    
    //Mono Analysis Signal
    mixedBuf.clear();
    for (int i = 0; i < numSamples; i++) {
        float sample = 0.f;
        for (int chan = 0; chan < numInputChannels; chan++)
            sample += inputs[chan][i];
    
        mixedBuf[i] = sample / (float)numInputChannels;
    }
//    sin.processBlock(&mixedBuf[0], numSamples);
    
    
    int remaining = numSamples;
    //Analysis
    while (remaining > 0) {
        const int numToCopy = std::min(remaining, (int)controlBlockSize);
        const int bufOffset = numSamples - remaining;

        memcpy(&analysisChain.inputSignal[0],
               &mixedBuf[bufOffset],
               controlBlockSize * sizeof(float));
        //ControlLoop
        analysisChain.computeBlock();
        
        
        //AudioLoop
        for (int i = 0; i < controlBlockSize; i++)
        {
            //ThresholdChecking
            detectors[PITCH].setInput
            (analysisChain.pitchYinFFT.output<0>());
            
            detectors[PITCH_CONFIDENCE].setInput
            (analysisChain.pitchYinFFT.output<1>());
            
            detectors[PITCH_SALIENCE].setInput
            (analysisChain.pitchSalience.output<0>());
            
            detectors[INHARMONICITY].setInput
            (analysisChain.inharmonicity.output<0>());
            
            detectors[LEVEL].setInput(mixedBuf[bufOffset + i]);
        
            if (detectors[LEVEL].getGateCode() == triggerConditions[LEVEL])
            {
                convolutionEnvelopes[0].trigger();
            }
            if (detectors[LEVEL].getGateCode() == releaseConditions[LEVEL])
            {
                convolutionEnvelopes[0].trigger();
            }

            for (int d_i = 0; d_i < NUM_DETECTORS; d_i++) {
                detectors[d_i].applySmoothing();
                waveformViewers[d_i]->addSample(jd::clip(detectors[d_i].normalisedScaledOutput(),0.f,1.f));
            }
        }

        remaining -= numToCopy;
    }
    
    //Convolution

//    if (convolutionEnabled[0]) {
//        
//        if (convolutionTriggered[0]) {
//            convolutionEnvelopes[0].trigger();
//            convolutionTriggered[0] = false;
//        }
    
        convolvers[0]->processBlock(inputs, numSamples);
        
            convolutionEnvelopes[0].updateAction();
        convolutionEnvelopes[0].writeToBlock(convolutionEnvelopeBuffers[0].getWritePointer(0), numSamples);

        FloatVectorOperations::multiply(wetBuffer.getWritePointer(0),
                                        convolutionEnvelopeBuffers[0]
                                        .getWritePointer(0),
                                        buffer.getReadPointer(0), numSamples);
        
        FloatVectorOperations::multiply(wetBuffer.getWritePointer(1),
                                        convolutionEnvelopeBuffers[0]
                                        .getWritePointer(0),
                                        buffer.getReadPointer(1)
                                        , numSamples);
    

//    }?
    
    
    //Cookbook EQ??
    
    //mix input and output
    
    
    //masterLevel
    for (int i = 0; i  < numSamples; i++)
    {
//        outputs[0][i] = convolvers[0]->leftChannel.getBufferData()[i];
//        outputs[1][i] = convolvers[0]->rightChannel.getBufferData()[i];
        
        outputs[0][i] = wetBuffer.getSample(0, i);
        outputs[1][i] = wetBuffer.getSample(1, i);
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
