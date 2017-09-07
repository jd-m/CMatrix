#ifndef AnalysisEditor_hpp
#define AnalysisEditor_hpp


#include "../../../JuceLibraryCode/JuceHeader.h"
#include "../../PluginProcessor.h"
#include "../../jd-lib/jdHeader.h"
#include "../JDEnvelopeGUI.hpp"
#include <stdio.h>
#include "SignalDisplayUI.hpp"
#include "AnalysisMeter.hpp"
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
public ValueTree::Listener,
public ChangeBroadcaster,
public KeyListener
{
public:
    AnalysisEditor(Jd_cmatrixAudioProcessor& p);
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
    void valueTreePropertyChanged(juce::ValueTree &tree,
                                  const juce::Identifier &property) override
    {
        
    }
    void valueTreeChildAdded (ValueTree& parentTree,
                              ValueTree& childWhichHasBeenAdded) override {}
    
    void valueTreeChildRemoved (ValueTree& parentTree,
                                ValueTree& childWhichHasBeenRemoved,
                                int indexFromWhichChildWasRemoved) override {}
    
    void valueTreeChildOrderChanged (ValueTree& parentTreeWhoseChildrenHaveMoved,
                                     int oldIndex, int newIndex) override {}
    
    void valueTreeParentChanged (ValueTree& treeWhoseParentHasChanged) override {}
    //=====================================================================
    bool keyPressed(const KeyPress &key, Component *originatingComponent) override
    {
        if (key.getTextCharacter() == juce_wchar{'s'}) {
            std::cout << "YAK" << std::endl;
            store = currentState.createCopy();
        }
        if (key.getTextCharacter() == 'l') {
            
            currentState.copyPropertiesFrom(store, nullptr);
            for (int i = 0; i < currentState.getNumProperties(); i++)
            {
            }
        }
        if (key.getTextCharacter() == 'c') {
            
        }
        if (key.getTextCharacter() == 'b') {
            
        }
        return {};
    }
    //=====================================================================
    
private:
    Jd_cmatrixAudioProcessor& processor;
    OwnedArray<SignalDrawer>& waveformViewers { processor.waveformViewers };
    WaveformDisplay waveformDisplay;
    
    int activeWaveform = LEVEL;
    Array<Colour> detectorColours {
        Colours::darkorange,
        Colours::darkred,
        Colours::black,
        Colours::skyblue,
        Colours::purple
    };

    ValueTree currentState {"tree"};
    ValueTree store;
    std::array<ValueTree,4> storedStates;
    
    ComboBox setModeBox;
    ComboBox setActiveDetector;
    
    Slider attackTimeKnob;
    Slider releaseTimeKnob;
    Slider rmsKnob;
    Slider smoothingSpeedKnob;
    Slider samplesPerPixelKnob;
    
    OwnedArray<AnalysisMeter> meters;
    
    TextButton makeAllVisibleButton;
    
};

#endif /* AnalysisEditor_hpp */
