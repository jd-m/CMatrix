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
        
        const float margin = 0.1f;
        const float halfMargin = margin * 0.5f;
        const float halfHeight = (float)getHeight() * (1.f - halfMargin);
        
        int bp = bufferPos;
        
        Path p;
        p.startNewSubPath(getWidth(), halfHeight - halfHeight * circularBuffer [(bp + bufferSize) % bufferSize]);
        
        for (int x = getWidth() - 1; x-- >= 0;) {
            const int samplesAgo = getWidth() - x;
            const float level = circularBuffer [(bp + bufferSize - samplesAgo) % bufferSize];
                p.lineTo((float)x, halfHeight - halfHeight * level * (1.f - halfMargin));
        }
        g.strokePath(p, PathStrokeType(1));
        
        auto r = getLocalBounds().toFloat();
        r.removeFromTop(halfMargin * (float)getHeight()).removeFromBottom(halfMargin * (float)getHeight());
        g.drawLine({r.getTopLeft(), r.getTopRight()}, 1);
        g.drawLine({r.getBottomLeft(), r.getBottomRight()}, 1);
    
    }
    void timerCallback()
    {
        repaint();
    }
    void addSample (const float sample)
    {
        currentInputLevel += fabsf (sample);
        
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
