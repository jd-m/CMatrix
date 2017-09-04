#ifndef WaveformViewer_hpp
#define WaveformViewer_hpp

#include <stdio.h>
#include "../JuceLibraryCode/JuceHeader.h"


class AudioInputWaveformDisplay  : public Component,
public Timer
{
public:
    AudioInputWaveformDisplay()
    {
        bufferPos = 0;
        bufferSize = 2048;
        circularBuffer = (float*)malloc(sizeof (float) * bufferSize);
        currentInputLevel = 0.0f;
        numSamplesIn = 0;
        setOpaque (true);
        startTimerHz(30); 
    }
    ~AudioInputWaveformDisplay()
    {
        free(circularBuffer);
    }
    void paint (Graphics& g)
    {
        g.fillAll (Colours::black);
        g.setColour (Colours::darkorange);
        
//        const float margin = 0.1f;
//        const float halfMargin = margin * 0.5f;
        const float halfHeight = (float)getHeight() * 0.5f;
        const float heightf = static_cast<float>(getHeight());
        int bp = bufferPos;
        
        Path p;
        
        p.startNewSubPath(getWidth(), halfHeight - halfHeight * circularBuffer [(bp + bufferSize) % bufferSize]);
        
        for (int x = getWidth() - 1; x-- >= 0;) {
            const int samplesAgo = getWidth() - x;
            const float level = circularBuffer [(bp + bufferSize - samplesAgo) % bufferSize];
                p.lineTo((float)x, heightf - heightf * level);
        }
        g.strokePath(p, PathStrokeType(1));
        
        auto r = getLocalBounds().toFloat();
        g.drawRect(r.removeFromTop(heightf * 0.5f));
        g.drawRect(r);
    
    }
    void timerCallback()
    {
        repaint();
    }
    void addSample (const float sample)
    {
        currentInputLevel += ::fabsf(sample);
        
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
private:
    float* circularBuffer;
    float currentInputLevel;
    int volatile bufferPos, bufferSize, numSamplesIn, samplesToAverage = 128;
};

#endif /* WaveformViewer_hpp */
