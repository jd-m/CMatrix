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
    startTimerHz(20);
    
    addAndMakeVisible(tabbedWindow);
    tabbedWindow.addTab("Analysis", Colours::darkgrey, &analysisEditor, false);
//    tabbedWindow.addTab("IR", Colours::lightgrey, &irEditor, false);
    
//    File f = File("~/Music/sc_sounds/beat/piano/piano_01.wav");
//    processor.convolver.loadIRFromFile(f, 0);
//    processor.convolverR.loadIRFromFile(f, 0);

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
    g.drawText(String(processor.dbg_meter), 10, 300,200,100, Justification::centred);

}
void Jd_cmatrixAudioProcessorEditor::resized()
{
    auto r = getLocalBounds();
    tabbedWindow.setBounds(r);
}
void Jd_cmatrixAudioProcessorEditor::sliderValueChanged(juce::Slider *slider)
{

    
}
void Jd_cmatrixAudioProcessorEditor::timerCallback()
{
//    repaint();
}
