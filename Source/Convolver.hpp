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

class SimpleConvolver {
public:
    SimpleConvolver () {}
    ~SimpleConvolver () {}
    
    void prepareToPlay (double sampleRate, int samplesPerBlock) {
        
        {
            juce::ScopedLock convolverLock(convolverMutex);
            convolverHeadBlockSize = 1;
            while (convolverHeadBlockSize < static_cast<size_t>(samplesPerBlock))
            {
                convolverHeadBlockSize *= 2;
            }
            convolverTailBlockSize = std::max(size_t(8192), 2 * convolverHeadBlockSize);
        }
        m_processingBuffer.resize(samplesPerBlock);
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
        ScopedLock lock { convolverMutex };
        m_convolver.init(convolverHeadBlockSize,
                       convolverTailBlockSize,
                       buffer.data(),
                       fileLen);
        }
    }

    void processBlock (const float *inputBlock, size_t numSamples) {
        m_convolver.process(inputBlock, m_processingBuffer.data(), numSamples);
    }
    
    fftconvolver::TwoStageFFTConvolver& convolver() { return m_convolver; }
    const float* bufferData() { return m_processingBuffer.data(); }
    
    mutable CriticalSection convolverMutex;
    size_t convolverHeadBlockSize;
    size_t convolverTailBlockSize;
    
    fftconvolver::TwoStageFFTConvolver m_convolver;
    std::vector<float> m_processingBuffer;
};

#endif /* SimpleConvolver_hpp */
