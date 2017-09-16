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
#include "DetectorMatrix.hpp"

class AnalysisEditor : public Component,
public Slider::Listener,
public Button::Listener,
public ComboBox::Listener,
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
    void positionMasterPanel(const Rectangle<int>& sectionBounds);
    void positionEditPanel(const Rectangle<int>& sectionBounds);
    //=====================================================================
    void sliderValueChanged(Slider* slider) override;
    //=====================================================================
    void comboBoxChanged(ComboBox *comboBox) override;
    //=====================================================================
    void buttonClicked(Button* button) override;
private:
    void setMeterTriggerMode(int meterIndex);
    //=====================================================================
    void soloDetectorSignal();
    //=====================================================================
    template<class Container, class ItemType>
    int getIndexOfItemInArray (Container& container, ItemType& item)
    {
        for (int index = 0; index < NUM_DETECTORS; index++) {
            if (item == container[index])
                return index;
        }
        return -1;
        
    }
    void deselectAllEnvelopes();
    void deselectAllDetectionSettings();
    
    
    using GateCode = Jd_cmatrixAudioProcessor::GateCode;
    
    Jd_cmatrixAudioProcessor& processor;
    OwnedArray<SignalDrawer>& waveformViewers { processor.waveformViewers };
    WaveformDisplay waveformDisplay;

    int activeWaveform = LEVEL;
    
    int indexOfDetectorBeingEdited  {-1};
    
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
    OwnedArray<Slider> attackTimeKnobs;
    OwnedArray<Slider> releaseTimeKnobs;
    OwnedArray<Slider> rmsKnobs;
    OwnedArray<Slider> smoothingSpeedKnobs;
    OwnedArray<Slider> samplesPerPixelKnobs;
    
    //TRIGGER
    using TriggerCondition = jd::GateDouble<float>::GateCrossingCode;
    
    using EnvelopeMode = Jd_cmatrixAudioProcessor::EnvelopeMode;
    
    int currentEnvelopeIndex {-1};

    ComboBox setEnvelopeComboBox;
    OwnedArray<Slider> envelopeAttackTimeKnobs;
    OwnedArray<Slider> envelopeDecayTimeKnobs;
    OwnedArray<Slider> envelopeSustainTimeKnobs;
    OwnedArray<Slider> envelopeReleaseTimeKnobs;
    OwnedArray<Slider> envelopeLevelKnobs;

    ApplicationProperties properties;
    
    OwnedArray<ToggleButton> shouldApplyEnvelopeToIRButtons;
    
    //METERS
    OwnedArray<AnalysisMeter> meters;
    
    OwnedArray<ComboBox> triggerConditionComboBoxes;
    OwnedArray<ComboBox> releaseConditionComboBoxes;
    OwnedArray<ComboBox> setEnvelopeModeBoxes;
    OwnedArray<TextButton> loadIRButtons;
    OwnedArray<Slider> inputGainSliders;
    
    CmatrixLookAndFeel lookAndFeel;
    
    //DetectorMatrix
    ComboBox matrixDetectorBox;
    OwnedArray<DetectorMatrix> detectorMatrices;
    
    //SideChainButton
    ToggleButton useSideChainButton;
    
    Slider wetGainDBSlider;
    Slider dryGainDBSlider;
    Slider inputGainDBSlider;
    
    Rectangle<int> detectorDrawingBounds;
    Rectangle<int> analysisSettingBounds;
    Rectangle<int> meterBounds;
    Rectangle<int> masterControlBounds;
    Rectangle<int> envelopeParameterPanel;
    Rectangle<int> conditionPanel;
    
    
};

#endif /* AnalysisEditor_hpp */
