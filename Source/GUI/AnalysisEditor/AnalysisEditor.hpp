#ifndef AnalysisEditor_hpp
#define AnalysisEditor_hpp


#include "../../../JuceLibraryCode/JuceHeader.h"
#include "../../PluginProcessor.h"
#include "../../jd-lib/jdHeader.h"
#include "../JDEnvelopeGUI.hpp"
#include <stdio.h>
#include "SignalDisplayUI.hpp"
#include "AnalysisMeter.hpp"
#include "IRSelector.hpp"
#include "LookAndFeel.hpp"

class AnalysisEditor : public Component,
public Slider::Listener,
public Button::Listener,
public ComboBoxListener,
public ChangeBroadcaster
{
public:
    AnalysisEditor(Jd_cmatrixAudioProcessor& p);
//=====================================================================
    void paint(Graphics& g) override;
    void resized() override;
    //=================================================
    void positionDetectorDrawing(const Rectangle<int>& sectionBounds);
    void positionDetectorSettings(const Rectangle<int>& sectionBounds);
    void positionMeters(const Rectangle<int>& sectionBounds);
    void positionMeterButtons(const Rectangle<int>& sectionBounds);
    void positionMasterPanel(const Rectangle<int>& sectionBounds());
    //=====================================================================
    void sliderValueChanged(Slider* slider) override;
    //=====================================================================
    void comboBoxChanged(ComboBox *comboBox) override;
    //=====================================================================
    void buttonClicked(Button* button) override;
    //=====================================================================
    void setMeterTriggerMode(int meterIndex);
    //=====================================================================
    void soloDetectorSignal();
    //=====================================================================

    void mouseDown(const MouseEvent& e) override
    {
        if (e.getNumberOfClicks() == 2)
        {
//            processor.convolutionEnvelopes[0].trigger();
        }
    }
    
    
    Jd_cmatrixAudioProcessor& processor;
    OwnedArray<SignalDrawer>& waveformViewers { processor.waveformViewers };
    WaveformDisplay waveformDisplay;

    int activeWaveform = LEVEL;
    
    Array<Colour> detectorColours {
        Colours::darkorange,
        Colours::darkred,
        Colours::white,
        Colours::skyblue,
        Colours::purple
    };
    
    ComboBox setDisplayAnnotation;
    ComboBox setActiveDetector;
    
    //Envelope detection
    Slider attackTimeKnob;
    Slider releaseTimeKnob;
    Slider rmsKnob;
    Slider smoothingSpeedKnob;
    Slider samplesPerPixelKnob;
    
    //TRIGGER
    using TriggerCondition = jd::GateDouble<float>::GateCrossingCode;
    
    enum EnvelopeMode {
        oneShot,
        sustain
    };
    
    OwnedArray<ToggleButton> metersToUseForSelectionButtons;
    ComboBox triggerConditionBox;
    TriggerCondition irSequenceTriggerCondition;
    
    //METERS
    OwnedArray<AnalysisMeter> meters;
    
    Array<EnvelopeMode> meterEnvelopeModes;
    OwnedArray<ComboBox> meterIRTriggerModeBoxes;
    OwnedArray<ComboBox> meterIRSelectionBoxes;
    OwnedArray<ComboBox> setEnvelopeModeBoxes;
    OwnedArray<TextButton> loadIRButtons;
    
    CmatrixLookAndFeel lookAndFeel;
    TextButton pad;
    Slider masterFader;
    
};

#endif /* AnalysisEditor_hpp */
