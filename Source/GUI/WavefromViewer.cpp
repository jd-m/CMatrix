#include "WaveformViewer.hpp"

SignalDrawer::SignalDrawer()
{
    circularBuffer.resize(bufferSize);
    clear();
 
    startTimerHz(30);
}
//=======================================================================
void SignalDrawer::paint(juce::Graphics &g)
{
    g.fillAll (Colours::transparentBlack);
    if (isActive)
        g.setColour (lineColour);
    else
        g.setColour(lineColour.withAlpha((float)0.1));
    const float heightf = (float)getHeight();
    int bp = bufferPos;
    
    Path p;
    
    p.startNewSubPath(getWidth(), 0);
    
    for (int x = getWidth() - 1; x > 0; x--)
    {
        const int samplesAgo = getWidth() - x;
        
        const float level = circularBuffer [(bp + bufferSize - samplesAgo) % bufferSize];
        float xf = (float)x;
        
        p.lineTo(xf, heightf - heightf * level);
    }
    
    g.strokePath(p, PathStrokeType(1));
    
}
//=======================================================================
void SignalDrawer::addSample(const float sample)
{
    currentInputLevel += ::fabs(sample);
    if (++numSamplesIn > samplesToAverage)
    {
        circularBuffer [bufferPos++ % bufferSize] = currentInputLevel / samplesToAverage;
        numSamplesIn = 0;
        currentInputLevel = 0.0f;
    }
}
//=======================================================================
void SignalDrawer::clear()
{
    circularBuffer.clear();
}
//=======================================================================
void SignalDrawer::setSamplesToAverage(size_t newSamplesToAverage)
{
    samplesToAverage = newSamplesToAverage;
}
//=======================================================================
void SignalDrawer::setLineColour(juce::Colour newLineColour)
{
    lineColour = newLineColour;
}
//=======================================================================
void SignalDrawer::setIsActive(bool newIsActive)
{
    isActive = newIsActive;
    repaint();
}
//=======================================================================
void SignalDrawer::timerCallback()
{
    repaint();
}
