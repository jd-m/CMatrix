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
//dbScale
//MeterBar
//Meter
//ParameterPanel
//WaveformViewer
//AnalysisChainMenu


class AnalysisEditor : public Component,
public Slider::Listener,
public Button::Listener,
public ComboBoxListener,
public ChangeBroadcaster
{
public:
    AnalysisEditor(Jd_cmatrixAudioProcessor& p,
                   ButtonGrid& stepSequencerSource);
//=====================================================================
    void paint(Graphics& g) override;
    void resized() override;
    //=====================================================================
    void sliderValueChanged(Slider* slider) override;
    //=====================================================================
    void comboBoxChanged(ComboBox *comboBox) override;
    //=====================================================================
    void buttonClicked(Button* button) override;
    //=====================================================================
    void mouseDown(const MouseEvent& e) override {
        
        if (e.getNumberOfClicks() == 2)
        {
            sequencer.stepToNextEnabledValue();
        }
    }
private:
    Jd_cmatrixAudioProcessor& processor;
    OwnedArray<SignalDrawer>& waveformViewers { processor.waveformViewers };
    WaveformDisplay waveformDisplay;
    
    ButtonGrid& stepSequencer;
    
    
    int activeWaveform = LEVEL;
    Array<Colour> detectorColours {
        Colours::darkorange,
        Colours::darkred,
        Colours::black,
        Colours::skyblue,
        Colours::purple
    };

    ValueTree currentState {"Analysis"};
    ValueTree temporaryState;
    HashMap<String, ValueTree> storedStates;
    

    IRSequencer sequencer {
        processor,
        stepSequencer.storedIrStates,
        stepSequencer
    };
    
    ComboBox setModeBox;
    ComboBox setActiveDetector;
    
    Slider attackTimeKnob;
    Slider releaseTimeKnob;
    Slider rmsKnob;
    Slider smoothingSpeedKnob;
    Slider samplesPerPixelKnob;
    
    ComboBox irSequenceSelectionBox;
    ComboBox irTriggerModeBox;
    
    int gateTriggerCode;
    
    enum EnvelopeMode {
        ONE_SHOT,
        SUSTAIN
    };
    
    OwnedArray<AnalysisMeter> meters;
    
    TextButton makeAllVisibleButton;
    
    
    TextButton pad;
    Slider masterFader;
    
};

#endif /* AnalysisEditor_hpp */
