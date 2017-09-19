/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
Jd_cmatrixAudioProcessorEditor::Jd_cmatrixAudioProcessorEditor (Jd_cmatrixAudioProcessor& p)
: AudioProcessorEditor (&p), processor (p)
{
    
    addAndMakeVisible(tabbedWindow);
    tabbedWindow.addTab("Analysis", Colours::darkgrey, &analysisEditor, false);

    setSize (800, 750);
    setOpaque(true);
}

Jd_cmatrixAudioProcessorEditor::~Jd_cmatrixAudioProcessorEditor()
{
}
//==============================================================================
void Jd_cmatrixAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll(Colours::darkgrey);
}
void Jd_cmatrixAudioProcessorEditor::resized()
{
    auto r = getLocalBounds();
    tabbedWindow.setBounds(r);
}
