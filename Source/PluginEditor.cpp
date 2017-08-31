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
    
    setSize (400, 300);

    
//    processor.convolver.loadIRToConvolver();
//    processor.convolver.loadIRFromFile(File("~/Music/sc_sounds/beat/jam/jam_chord.wav"), 0);
    
    startTimerHz(20);
    addAndMakeVisible(dbg);
    
    
    File f = File("~/Music/sc_sounds/beat/piano/piano_01.wav");
    processor.convolver.loadIRFromFile(f, 0);
//    processor.convolver.loadIRToConvolver();
}

Jd_cmatrixAudioProcessorEditor::~Jd_cmatrixAudioProcessorEditor()
{
}

//==============================================================================
void Jd_cmatrixAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll (Colours::white);
}

void Jd_cmatrixAudioProcessorEditor::resized()
{
    auto r = getLocalBounds();
    dbg.setBounds(r);
}

void Jd_cmatrixAudioProcessorEditor::timerCallback()
{
    String s = String(
    
    );
    
    dbg.setText(s);
}
