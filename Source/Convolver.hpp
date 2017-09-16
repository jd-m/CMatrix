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
#include "FFTConvolver/TwoStageFFTConvolver.h"
#include "jd-lib/jdHeader.h"
#include "../JuceLibraryCode/JuceHeader.h"


class Convolver {
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
                     importBuffer.getReadPointer(fileChannel % fileChannels),
                     static_cast<size_t>(loading) * sizeof(float)
                     );
            pos += static_cast<size_t>(loading);
        }
        
        {
        
        convolver.init(convolverHeadBlockSize,
                       convolverTailBlockSize,
                       buffer.data(),
                       fileLen);
        }
    }

    void processBlock (const float *inputBlock, int numSamples) {
            convolver.process(inputBlock, processingBuffer.data(), numSamples);
    }
    
    float* getBufferData() { return processingBuffer.data(); }
    
    size_t convolverHeadBlockSize;
    size_t convolverTailBlockSize;
    fftconvolver::TwoStageFFTConvolver convolver;
    std::vector<float> processingBuffer;
};

//====================================================================

struct StereoConvolver {

    void prepareToPlay(double sampleRate, int blockSize)
    {
        leftChannel.prepareToPlay(sampleRate, blockSize);
        rightChannel.prepareToPlay(sampleRate, blockSize);
    }
    
    void loadIRFromFile(File& file)
    {
        leftChannel.loadIRFromFile(file, 0);
        rightChannel.loadIRFromFile(file, 1);
    }
    
    void processBlock(const float** input, int numSamples)
    {
        leftChannel.processBlock(input[0], numSamples);
        rightChannel.processBlock(input[1], numSamples);
    }
    
    
    Convolver leftChannel;
    Convolver rightChannel;
};

#endif /* SimpleConvolver_hpp */
