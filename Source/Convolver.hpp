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
   
    void prepareToPlay (double sampleRate, int samplesPerBlock);
    
    void loadIRFromFile (File &file, size_t fileChannel);

    void processBlock (const float *inputBlock, int numSamples);
    
    float* getBufferData();
    
private:
    size_t convolverHeadBlockSize;
    size_t convolverTailBlockSize;
    fftconvolver::TwoStageFFTConvolver convolver;
    std::vector<float> processingBuffer;
};

//====================================================================

class MultiChannelConvolver {
public:
    void prepareToPlay (double sampleRate, int blockSize);
    
    void loadIRFromFile (File& file);
    
    void setNumChannels (size_t newNumChannels);
    
    float* getChannelData(size_t channelIndex);
    
    void processBlock (const float** inputs, int numSamples);
private:
    std::vector<std::unique_ptr<Convolver>> convolverChannels;
    size_t numChannels {0};
};

#endif /* SimpleConvolver_hpp */
