#ifndef AnalysisMeter_hpp
#define AnalysisMeter_hpp

#include <stdio.h>
#include "../../../JuceLibraryCode/JuceHeader.h"
#include "../../jd-lib/jdHeader.h"
#include "LookAndFeel.hpp"
#include "../../PluginProcessor.h"

class AnalysisMeter : public Component,
public Slider::Listener,
public Button::Listener,
public Timer
{
public:
    AnalysisMeter(DetectorUnit& sourceDetector);
    //=====================================================
    void paint (Graphics& g) override;
    void resized () override;
    //=====================================================
    void sliderValueChanged(Slider* slider) override;
    //=====================================================
    void buttonClicked(Button* button) override;
    //=====================================================
    void timerCallback() override;
    //=====================================================
    void setName(String meterName);
    void setRangeIsInverted(bool shouldInvertRange);
//private:
    //=====================================================
    class AnalysisMeterBar : public Component
    {
    public:
        void paint (Graphics& g) override;
        void resized () override {}
        void setLevel(const float level);
        void setRange(const float min, const float max);
        Range<float> getRange();
        bool isWithinRange();
    private:
        Colour outsideRangeCol { Colours::darkred };
        Colour withinRangeCol { Colours::darkorange };
        float m_level { 1.f };
        Range<float> m_range {0.25f, 0.75f};
        bool rangeIsInverted { false };
        friend AnalysisMeter;
    };
    //=====================================================
    DetectorUnit&       detector;
    Label               nameLabel;
    AnalysisMeterBar    meterBar;
    Slider              thresholdSlider;
    ToggleButton        enableButton;
    ToggleButton        invertRangeButton;
    CmatrixLookAndFeel  lookAndFeel;
    
    String m_name;
};

#endif /* AnalysisMeter_hpp */
