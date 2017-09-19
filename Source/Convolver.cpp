//
//  Convolver.cpp
//  jd_CMatrix
//
//  Created by Jaiden Muschett on 30/08/2017.
//
//

#include "Convolver.hpp"

void Convolver::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    {
        convolverHeadBlockSize = 1;
        while (convolverHeadBlockSize < static_cast<size_t> (samplesPerBlock))
        {
            convolverHeadBlockSize *= 2;
        }
        convolverTailBlockSize = std::max (size_t(8192), 2 * convolverHeadBlockSize);
    }
    
    processingBuffer.resize (samplesPerBlock);
}
//========================================================================
void Convolver::loadIRFromFile (juce::File &file, size_t fileChannel)
{
    AudioFormatManager formatManager;
    formatManager.registerBasicFormats();
    ScopedPointer<AudioFormatReader> reader (formatManager.createReaderFor(file));
    
    const int fileChannels = reader->numChannels;
    const size_t fileLen = static_cast<size_t> (reader->lengthInSamples);
    
    juce::AudioFormatReaderSource audioFormatReaderSource (reader, false);
    std::vector<float> buffer (fileLen);
    
    size_t pos = 0;
    AudioSampleBuffer importBuffer (fileChannels, 8192);
    
    while (pos < fileLen)
    {
        const int loading = std::min (importBuffer.getNumSamples(), static_cast<int>(fileLen - pos));
        
        juce::AudioSourceChannelInfo info;
        info.buffer = &importBuffer;
        info.startSample = 0;
        info.numSamples = loading;
        audioFormatReaderSource.getNextAudioBlock (info);
        
        ::memcpy(buffer.data() + pos,
                 importBuffer.getReadPointer (fileChannel % fileChannels),
                 static_cast<size_t> (loading) * sizeof(float) );
        pos += static_cast<size_t> (loading);
    }
    
    {
        convolver.init(convolverHeadBlockSize,
                       convolverTailBlockSize,
                       buffer.data(),
                       fileLen);
    }
}
//========================================================================
void Convolver::processBlock (const float *inputBlock, int numSamples)
{
    convolver.process (inputBlock, processingBuffer.data(), numSamples);
}
//========================================================================
float* Convolver::getBufferData()
{
    return processingBuffer.data();
}
//========================================================================
/* Multi-Channel Convolver */
//========================================================================
void MultiChannelConvolver::prepareToPlay (double sampleRate, int blockSize)
{
    for (auto& convolverChannel : convolverChannels)
        convolverChannel->prepareToPlay (sampleRate, blockSize);
}
//========================================================================
void MultiChannelConvolver::loadIRFromFile(juce::File &file)
{
    int channelIndex = 0;
    for (auto& convolverChannel : convolverChannels)
        convolverChannel->loadIRFromFile (file, channelIndex++);
}
//========================================================================
void MultiChannelConvolver::setNumChannels (size_t newNumChannels)
{
    convolverChannels.resize (newNumChannels);
    
    for (int i = numChannels; i < newNumChannels; i++) 
        convolverChannels[i] = std::move(std::unique_ptr<Convolver> (new Convolver()));
    
    numChannels = newNumChannels;
}
//========================================================================
float* MultiChannelConvolver::getChannelData(size_t channelIndex)
{
    return convolverChannels[channelIndex]->getBufferData();
}
//========================================================================
void MultiChannelConvolver::processBlock (const float **inputs, int numSamples)
{
    int channelIndex = 0;
    for (auto& convolverChannel : convolverChannels)
        convolverChannel->processBlock (inputs[channelIndex++], numSamples);
}
