#include "SignalDisplayUI.hpp"



//================================================================
void WaveformDisplay::paint(juce::Graphics &g)
{
 
    g.fillAll(Colours::transparentBlack);
    auto r = getLocalBounds().toFloat();
    
    auto unit = [](float val) { return val; };
    switch (scalingMode) {
        case LOG_AMP:
            drawScaledRange(g,
                            " dB",
                            util::logAmpScale,
                            unit,
                            unit);
            break;
        case LIN:
            drawScaledRange(g,
                            "",
                            util::linAmpScale,
                            unit,
                            unit);
            break;
        case LOG_FREQ:
            drawScaledRange(g,
                            " Hz",
                            util::freqScale,
                            jd::hzmidi<float>,
                            jd::hzmidi<float>);
            break;
            
        default:
            break;
    }

}
void WaveformDisplay::resized()
{
}
//================================================================
template<class C, class InputScalingFunc, class OutputScalingFunc>
void WaveformDisplay::drawScaledRange(Graphics &g,
                                      String unit,
                                      C unscaledMarkers,
                                      InputScalingFunc scaleInput,
                                      OutputScalingFunc scaleOutput)
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
    
    drawTextBox (String(unscaledMarkers.back()) + unit, 5);
    
    for (int i = unscaledMarkers.size()-2; i > 0; --i)
    {
        auto normalisedMarker = jd::linlin(scaleInput(unscaledMarkers[i]),
                                           scaleOutput(unscaledMarkers.front()),
                                           scaleOutput(unscaledMarkers.back()),
                                           0.f,
                                           1.f );
        int y = getHeight() * (1.f - normalisedMarker);
        g.drawLine ( 0, y, getWidth(), y, 1);
        drawTextBox(String(unscaledMarkers[i]) + unit, y);
    }
    drawTextBox (String(unscaledMarkers.front()) + unit, getHeight() - 5);
}
//================================================================
void WaveformDisplay::setMode (ScalingMode newScalingMode)
{
    scalingMode = newScalingMode;
    repaint();
}
