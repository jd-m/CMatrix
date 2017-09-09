//
//  SimpleConvolver.hpp
//  jdConvolver
//
//  Created by Jaiden Muschett on 03/08/2017.
//
//

#ifndef SimpleConvolver_hpp
#define SimpleConvolver_hpp


#include <stdio.h>
#include "TwoStageFFTConvolver.h"
#include "jdHeader.h"
#include "../JuceLibraryCode/JuceHeader.h"

template<int NumChannels>
class SimpleConvolver {
public:
   
    void prepareToPlay (double sampleRate, int samplesPerBlock) {
        
        {
        
            convolverHeadBlockSize = 1;
            while (convolverHeadBlockSize < static_cast<size_t>(samplesPerBlock))
            {
                convolverHeadBlockSize *= 2;
            }
            convolverTailBlockSize = std::max(size_t(8192), 2 * convolverHeadBlockSize);
        }
        
        for (auto& processingBuffer : m_processingBuffers)
            processingBuffer.resize(samplesPerBlock);
    }
    
    void loadIRFromFile (File &file, size_t fileChannel)
    {
        AudioFormatManager formatManager;
        formatManager.registerBasicFormats();
        ScopedPointer<AudioFormatReader> reader(formatManager.createReaderFor(file));
        
        const int fileChannels = reader->numChannels;
        const size_t fileLen = static_cast<size_t>(reader->lengthInSamples);
        
        juce::AudioFormatReaderSource audioFormatReaderSource(reader, false);
        std::vector<float> buffer (fileLen);
        
        size_t pos = 0;
        AudioSampleBuffer importBuffer(fileChannels, 8192);
        
        while (pos < fileLen)
        {
            const int loading = std::min(importBuffer.getNumSamples(), static_cast<int>(fileLen - pos));
            
            juce::AudioSourceChannelInfo info;
            info.buffer = &importBuffer;
            info.startSample = 0;
            info.numSamples = loading;
            audioFormatReaderSource.getNextAudioBlock(info);
        
            ::memcpy(buffer.data() + pos,
                     importBuffer.getReadPointer(fileChannel),
                     static_cast<size_t>(loading) * sizeof(float)
                     );
            pos += static_cast<size_t>(loading);
        }
        
        {
        
        m_convolvers[fileChannel].init(convolverHeadBlockSize,
                       convolverTailBlockSize,
                       buffer.data(),
                       fileLen);
        }
    }
    
    void loadMultiChannelIRfromFile(File& file)
    {
        for(int i = 0; i < NumChannels; i++)
            loadIRFromFile(file, i);
    }

    void processBlock (const float **inputBlock, int numSamples) {
        
        for (int channelNum = 0; channelNum < NumChannels; channelNum++) {
            m_convolvers[channelNum].process(inputBlock[channelNum], m_processingBuffers[channelNum].data(), numSamples);
        }
    }
    
    void processChannel(int channelNum, const float *inputBlock, int numSamples)
    {
        m_convolvers[channelNum].process(inputBlock, m_processingBuffers[channelNum].data(), numSamples);
    }
    
    void crossConvolve(const float **inputBlock, int numSamples){
        //    
        //  [L 0 1]
        //  [R 1 0]
    }

    const float* bufferDataAt(int channelNum) { return m_processingBuffers[channelNum].data(); }
    
    
    size_t convolverHeadBlockSize;
    size_t convolverTailBlockSize;
    String name = "";//Cannot be copied
    std::array<fftconvolver::TwoStageFFTConvolver, NumChannels> m_convolvers;
    std::array<std::vector<float>, NumChannels> m_processingBuffers;
};

//====================================================================

#endif /* SimpleConvolver_hpp */
