#include "AnalysisMeter.hpp"
//====================================================================
/* Analysis Meter Bar */
//====================================================================
void AnalysisMeter::AnalysisMeterBar::paint(juce::Graphics &g)
{
    auto r = getLocalBounds();
    
    if (((AnalysisMeter*)getParentComponent())->enableButton.getToggleState()) {
        if (isWithinRange())
            g.setColour(withinRangeCol);
        else
            g.setColour(outsideRangeCol);
    } else {
        g.setColour(Colours::darkgrey);
    }
    
    g.fillRect(r.removeFromBottom((m_level) * (float)r.getHeight()));
    
    g.setColour(Colours::grey.withAlpha(0.3f));
    
    auto threshBounds = getLocalBounds()
    .withY((int)(m_range.getStart()  * (float)getHeight()))
    .withHeight((int)(m_range.getLength() * (float)getHeight())).toFloat();
    
    g.fillRect(threshBounds);
    
    g.setColour(Colours::black);
    g.drawRect(getLocalBounds());
    
}
//============================================================================
void AnalysisMeter::AnalysisMeterBar::setLevel(const float level)
{
    m_level = level;
    
    if (((AnalysisMeter*)getParentComponent())->enableButton.getToggleState())
        repaint();
}
//============================================================================
void AnalysisMeter::AnalysisMeterBar::setRange(const float min, const float max)
{
    m_range.setStart(min < max ? min : max);
    m_range.setEnd(min < max ? max : min);
    repaint();
}
//============================================================================
bool AnalysisMeter::AnalysisMeterBar::isWithinRange()
{
    return rangeIsInverted ?  !m_range.contains(1. - m_level) : m_range.contains(1. - m_level);
}
//====================================================================
/* Analysis Meter */
//====================================================================
AnalysisMeter::AnalysisMeter(DetectorUnit& sourceDetector):
detector(sourceDetector)
{
    
    //NameLabel
    addAndMakeVisible(nameLabel);
    nameLabel.setFont(Font("courier", 14,0));
    nameLabel.setJustificationType(Justification::centred);
    //MeterBar
    addAndMakeVisible(meterBar);
    //ThresholdSlider
    addAndMakeVisible(thresholdSlider);
    thresholdSlider.setSliderStyle(Slider::SliderStyle::TwoValueVertical);
    thresholdSlider.setRange(0., 1.);
    thresholdSlider.setMinAndMaxValues(0.0,0.0);
    thresholdSlider.addListener(this);
    
    addAndMakeVisible(enableButton);
    enableButton.addListener(this);
    
    addAndMakeVisible(invertRangeButton);
    invertRangeButton.addListener(this);
    
    startTimerHz(30);
    
    enableButton.setLookAndFeel(&lookAndFeel);
    invertRangeButton.setLookAndFeel(&lookAndFeel);
    
    setOpaque(true);
}
//============================================================================
void AnalysisMeter::paint(juce::Graphics &g)
{
    g.drawRoundedRectangle(getLocalBounds().toFloat(), 5.f, 1.f);
    g.fillAll(Colours::darkgrey);
    g.setColour(Colours::darkorange);
    g.drawText("on",
               enableButton.getBounds().getX(),
               enableButton.getBounds().getBottom(),
               enableButton.getBounds().getWidth(),
               20,
               Justification::centred);
    
    g.drawText("inv",
               invertRangeButton.getBounds().getX(),
               invertRangeButton.getBounds().getBottom(),
               invertRangeButton.getBounds().getWidth(),
               20,
               Justification::centred);
    


    g.setColour(Colours::black);
    g.drawRect(getLocalBounds());
    
}
//============================================================================
void AnalysisMeter::resized()
{
    auto r = getLocalBounds();
    
    nameLabel.setBounds(r.removeFromTop(20));
    
    thresholdSlider.setBounds(r.removeFromLeft(25));
    meterBar.setBounds(r.removeFromLeft(25).reduced(2, 5));
    
    enableButton.setBounds(r.removeFromTop(r.getWidth()).reduced(6));
    
    invertRangeButton.setBounds(enableButton.getBounds().translated(0, 70));
    
    
}
//============================================================================
void AnalysisMeter::sliderValueChanged(juce::Slider *slider)
{
    if (slider == &thresholdSlider) {
        
//        auto normScale = [&](float x){
//            return detector.shouldConvertInput ? detector.normalisedScaled(detector.scaleInput(jd::clip(x, detector.limits))) :
//            detector.normalisedScaled(jd::clip(x, detector.limits.lower,detector.limits.upper));
//        };
//        
        float lower {}, upper {};
//        if(!detector.shouldConvertInput &&
//            detector.shouldConvertOutput)
//        {
//            lower = (jd::linlin((float)slider->getMinValue(),
//                                         0.f,
//                                         1.f,
//                                         (detector.limits.lower),
//                                         (detector.limits.upper)));
//            upper = (jd::linlin((float)slider->getMaxValue(),
//                                         0.f,
//                                         1.f,
//                                         (detector.limits.lower),
//                                         (detector.limits.upper)));
//        } else if (detector.shouldConvertInput &&
//                   detector.shouldConvertOutput) {
        lower = detector.normalisedScaled(slider->getMinValue());
        upper = detector.normalisedScaled(slider->getMaxValue());
//
//        }  else  {
//            lower = (float)slider->getMinValue();
//            upper = (float)slider->getMaxValue();
//        }
        
        meterBar.setRange(1. - lower,
                          1. - upper );
        detector.setRangeFromNormalised(lower, upper);
    }
}
//============================================================================
void AnalysisMeter::buttonClicked(juce::Button *button)
{
    if(button == &enableButton)
        detector.setEnabled(button->getToggleState());
    if (button == &invertRangeButton)
        setRangeIsInverted(button->getToggleState());
}
//============================================================================
void AnalysisMeter::timerCallback()
{
    meterBar.setLevel(detector.normalisedScaledOutput());
    meterBar.repaint();

}
//============================================================================
void AnalysisMeter::setName(const juce::String meterName)
{
    m_name = meterName;
    nameLabel.setText(meterName, dontSendNotification);
}
//============================================================================
void AnalysisMeter::setRangeIsInverted(bool shouldInvertRange)
{
    meterBar.rangeIsInverted = shouldInvertRange;
}
    
