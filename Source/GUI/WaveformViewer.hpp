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
//        memset(circularBuffer, 0.f, sizeof(float) * bufferSize);
        clear();
        currentInputLevel = 0.0f;
        numSamplesIn = 0;
        setOpaque (false);
        startTimerHz(30); 
    }
    ~AudioInputWaveformDisplay()
    {
        free(circularBuffer);
    }
    
    enum ScalingMode {
        LIN,
        LOG_AMP,
        LOG_FREQ
    };
    
    
    void paint (Graphics& g)
    {
        g.fillAll (Colours::transparentBlack);
        g.setColour (lineColour);
        
//        const float margin = 0.1f;
//        const float halfMargin = margin * 0.5f;
//        const float halfHeight = (float)getHeight() * 0.5f;
        const float heightf = static_cast<float>(getHeight());
        int bp = bufferPos;
        
        Path p;
    
        p.startNewSubPath(getWidth(), 0);
        
        for (int x = getWidth() - 1; x-- >= 0;) {
            const int samplesAgo = getWidth() - x;
            const float level = circularBuffer [(bp + bufferSize - samplesAgo) % bufferSize];
                p.lineTo((float)x, heightf - heightf * level);
        }
        g.strokePath(p, PathStrokeType(1));
        
        auto r = getLocalBounds().toFloat();
        
//        drawLogScaledRange(g, {20.f,100.f,500.f,2000.f,10000.f }, [](float const x){
//            return jd::hzmidi(x);
//        });
        drawLogScaledRange(g, {-60,-20,-6,0,6}, [](float x){ return x; });
        
//        switch (scalingMode) {
//            case LIN:
//                0;
//                break;
//            case LOG_AMP:
//                0;
//                break;
//            case LOG_FREQ:
//                0;
//                break;
//                
//            default:
//                break;
//        }
    
    }

    template<class C = std::vector<float>, class ScalingFunc>
    void drawLogScaledRange(Graphics &g, C unscaledMarkers, ScalingFunc func = [](float x){ return x;})
    {
        g.setColour(Colours::grey.withAlpha((float)0.1f));
        
        auto drawTextBox = [&](String text, int y) {
            
            auto textBox = Rectangle<int> { 3, y - 5, 50, 10 };
            g.setColour(Colours::grey.withAlpha((float)0.7f));
            g.fillRect(textBox);
            g.setColour(Colours::darkgrey.withAlpha((float)0.7f));
            g.drawRect(textBox);
            
            auto textBoxFont = Font("courier", 8, 0);
            g.setColour(Colours::black);
            g.setFont(textBoxFont);
            g.drawText(text,textBox, Justification::centred);
        };
        
        drawTextBox (String(unscaledMarkers.back())+"Hz", 5);
        
        for (int i = unscaledMarkers.size()-2; i > 0; --i)
        {
            auto normalisedMarker = jd::linlin(func(unscaledMarkers[i]),
                                               func(unscaledMarkers.front()),
                                               func(unscaledMarkers.back()),
                                               0.f,
                                               1.f );
            int y = getHeight() * (1.f - normalisedMarker);
            g.drawLine ( 0, y, getWidth(), y, 1);
            drawTextBox(String(unscaledMarkers[i])+"Hz", y);
        }
        drawTextBox (String(unscaledMarkers.front())+"Hz", getHeight()-5);
    }
    void timerCallback()
    {
        repaint();
    }
    void addSample (const float sample)
    {
        currentInputLevel += ::fabs(sample);
        
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
    
    void setMode (ScalingMode newScalingMode)
    {
        scalingMode = newScalingMode;
    }
    
    
private:
    bool isInFront {true};
    float* circularBuffer;
    float currentInputLevel;
    Colour lineColour;
    ScalingMode scalingMode;
    int volatile bufferPos, bufferSize, numSamplesIn, samplesToAverage = 128;
};

#endif /* WaveformViewer_hpp */
