/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#ifndef PLUGINEDITOR_H_INCLUDED
#define PLUGINEDITOR_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "AnalysisEditor.hpp"

//==============================================================================
/**
*/
class Jd_cmatrixAudioProcessorEditor  : public AudioProcessorEditor
{
public:
    Jd_cmatrixAudioProcessorEditor (Jd_cmatrixAudioProcessor&);
    ~Jd_cmatrixAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    
private:

    Jd_cmatrixAudioProcessor& processor;
    
    TabbedComponent tabbedWindow {TabbedButtonBar::Orientation::TabsAtTop};
    AnalysisEditor analysisEditor { processor };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Jd_cmatrixAudioProcessorEditor)
};


#endif  // PLUGINEDITOR_H_INCLUDED
