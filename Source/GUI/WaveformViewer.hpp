#ifndef WaveformViewer_hpp
#define WaveformViewer_hpp

#include <stdio.h>
#include "../JuceLibraryCode/JuceHeader.h"
#include "essentia_analyser_chain.h"
#include "Settings.h"

class SignalDrawer  : public Component,
public Timer
{
public:
    SignalDrawer()
    {
        bufferPos = 0;
        bufferSize = 2048;
        circularBuffer = (float*)malloc(sizeof (float) * bufferSize);
//        memset(circularBuffer, 0.f, sizeof(float) * bufferSize);
        clear();
        currentInputLevel = 0.0f;
        numSamplesIn = 0;
//        setOpaque (true);
        startTimerHz(30); 
    }
    ~SignalDrawer()
    {
        free(circularBuffer);
    }
    void paint (Graphics& g)
    {
        g.fillAll (Colours::transparentBlack);
        if (isActive)
            g.setColour (lineColour);
        else
            g.setColour(lineColour.withAlpha((float)0.1));
        const float heightf = static_cast<float>(getHeight());
        int bp = bufferPos;
        
        Path p;
    
        p.startNewSubPath(getWidth(), 0);
        
        for (int x = getWidth() - 1; x > 0; x--) {
            const int samplesAgo = getWidth() - x;
            
            const float level = circularBuffer [(bp + bufferSize - samplesAgo) % bufferSize];
            float xf = static_cast<float>(x);
            
            p.lineTo(xf, heightf - heightf * std::min(level, 0.99999f));
        }
    
        g.strokePath(p, PathStrokeType(1));
            
    }
    void timerCallback()
    {
        repaint();
    }
    void addSample (const float sample)
    {
        currentInputLevel += ::fabs(sample);
//        std::cout << currentInputLevel << std::endl;
        if (++numSamplesIn > samplesToAverage)
        {
            circularBuffer [bufferPos++ % bufferSize] = currentInputLevel / samplesToAverage;
            numSamplesIn = 0;
            currentInputLevel = 0.0f;
        }
    }
    void clear ()
    {
        zeromem (circularBuffer, sizeof (float) * bufferSize);
    }
    void setSamplesToAverage (size_t newSamplesToAverage)
    {
        samplesToAverage = newSamplesToAverage;
    }
    void setLineColour (Colour newLineColour)
    {
        lineColour = newLineColour;
    }
    void setIsActive(bool newIsActive)
    {
        isActive = newIsActive;
        repaint();
    }

private:
    bool isActive {true};
    float* circularBuffer;
    float currentInputLevel;
    Colour lineColour;
    int volatile bufferPos, bufferSize, numSamplesIn, samplesToAverage = 128;
};
#endif /* WaveformViewer_hpp */
