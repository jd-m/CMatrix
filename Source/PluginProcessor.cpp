/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

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
    
    controlBlockSize = (samplesPerBlock / loopsPerBlock);
    double controlRate = (sampleRate / samplesPerBlock) * loopsPerBlock;
    
    std::cout << "\nsampleRate: " << sampleRate <<
    "\ncontrolBlockSize: " << controlBlockSize <<
    "\nloopsPerBlock: " << loopsPerBlock <<
    "\ncontrolRate: "  << controlRate << std::endl;
    //Processors
    convolver.prepareToPlay(sampleRate, controlBlockSize);
    
    for (auto i : analysisChain.detectors)
        waveformViewers.add(new AudioInputWaveformDisplay());
    
    for (auto w : waveformViewers)
        w->setSamplesToAverage(64);
    
    //Analysis
    analysisChain.init(sampleRate, sampleRate, controlBlockSize);

    {

    int i = 0;
    for (auto& d: analysisChain.detectors) {
        d.init(sampleRate, samplesPerBlock);
        d.setThresholds(0.01,0.96);
        
        std::cout <<  d.thresholds.size() << std::endl;
        std::cout << i++ << " lower: " << d.thresholds[0] <<
        " upper: " << d.thresholds[1] << std::endl;
    }
    }
    
    mixedBuf.resize(samplesPerBlock);
    //Testing
    imp.init(sampleRate);
    imp.setFrequency(1.);
    
    sin.init(sampleRate);
    sin.setFrequency(0.125f);
    sin.setAmplitude(2.f);
    
    pitchSalience.setSampleRate(sampleRate);
    pitchSalience.setDurationS(0.01, 1.);
}

void Jd_cmatrixAudioProcessor::releaseResources()
{

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
    const int numInputChannels  = getTotalNumInputChannels();
    const int numOutputChannels = getTotalNumOutputChannels();
    const int numSamples = buffer.getNumSamples();
    
    auto inputs = buffer.getArrayOfWritePointers();
    auto outputs = buffer.getArrayOfWritePointers();

    for (int i = numInputChannels; i < numOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    for (int i = 0; i < numSamples; i++) {
        float sample = 0.f;
        for (int chan = 0; chan < numInputChannels; chan++)
            sample += inputs[chan][i];
    
        mixedBuf[i] = sample / (float)numInputChannels;
    }
//    sin.processBlock(&mixedBuf[0], numSamples);
    
    //control loop
//    for (int i = 0; i < loopsPerBlock; i++)
//    {
//        memcpy(&analysisChain.inputSignal[0], &mixedBuf[i * controlBlockSize], controlBlockSize * sizeof(float));
//        analysisChain.computeBlock();
//        pitchSalience.setValue(analysisChain.pitchSalience.output<0>());
//        
//        for (int j = 0; j < controlBlockSize;j++)
//        {
//            dbg_meter = pitchSalience.getNextValue();
//            
//            waveformViewer.addSample(dbg_meter);
//        }
//    }

    sin.processBlock(&mixedBuf[0], numSamples);
    
    int remaining = numSamples;
    
    while (remaining > 0) {
        const int numToCopy = std::min(remaining, (int)controlBlockSize);
        const int pos = numSamples - remaining;
        auto& ac = analysisChain;
        memcpy(&ac.inputSignal[0], &mixedBuf[pos], controlBlockSize * sizeof(float));
        ac.computeBlock();
        //controlLoop
//        ac.smoothedAnalyserOutputs[PITCH].setValue(ac.pitchYinFFT.output<0>());
//        ac.smoothedAnalyserOutputs[PITCH].setValue(ac.pitchYinFFT.output<0>());
//        ac.smoothedAnalyserOutputs[PITCH].updateTarget();
        
//        ac.smoothedAnalyserOutputs[PITCH_SALIENCE].setValue(ac.pitchSalience.output<0>());
//        ac.smoothedAnalyserOutputs[INHARMONICITY].setValue(ac.inharmonicity.output<0>());
        
//        for (auto d_i : ac.controlRateDetectors)
//            ac.smoothedAnalyserOutputs[d_i].updateTarget();
        
        //audioLoop
//        ac.detector(SOLO).checkThreshold(ac.pitchYinFFT.output<1>());
        for (int i = 0; i < controlBlockSize; i++)
        {
//            for (auto d_i : ac.controlRateDetectors) {
//                
//                const auto smoothedOutput = ac.smoothedAnalyserOutputs[d_i].nextValue();
//                auto sig = ac.envFollowers[d_i].processedSample(smoothedOutput);
//                ac.detectors[d_i].checkThreshold(sig);
//                //            ac.outputs[d_i] = smoothedOutput;
//                
//                waveformViewers[d_i]->addSample(smoothedOutput);
//            }
            
//            waveformViewers[PITCH]->addSample(ac.detectorLimits[PITCH](ac.pitchYinFFT.output<0>()).normalised());
            
            for (auto d_i : ac.audioRateDetectors) {
                auto sig = ac.envFollowers[LEVEL].processedSample(ac.inputSignal[i]);
                ac.detectors[LEVEL].checkThreshold(sig);
                auto scaled = ac.scalingFuncs[LEVEL](sig);
                auto normed = ac.detectorLimits[LEVEL].normalise(scaled);
                ac.outputs[LEVEL] =  ac.scalingFuncs[LEVEL](mixedBuf[pos  +i]);
//                jd::linlin(,-60.f,6.f,0.f,1.f);
                
                waveformViewers[LEVEL]->addSample(ac.normalisedOutput(LEVEL));
//                ac.outputs[d_i] = sig;
                outputs[0][pos + i] = sig ;
                outputs[1][pos + i] = sig;
            }
        }

        remaining -= numToCopy;
    }
    
//    imp.processBlock(mixedBuf.data(), numSamples);
//    sin.processBlock(&mixedBuf[0], numSamples);
//    convolver.processBlock(mixedBuf.data(), numSamples);
//    gate.processBlock(&mixedBuf[0], &mixedBuf[0], numSamples);
    
    for (int chan = 0; chan < numOutputChannels; chan++) {
        for (int i = 0; i < numSamples; i++) {
            
//            dbg_meter = mixedBuf[i];
//            dbg_meter = convolver.bufferData()[i];//dbg
//            outputs[chan][i] = convolver.bufferData()[i];
            outputs[chan][i] = 0.;
        }
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
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
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
