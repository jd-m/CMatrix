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
    
//    setInterceptsMouseClicks(true, true);
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
    
    std::stringstream ss;
//    std::cout <<  << std::endl;
    int i = 0;
//    for (auto& d : processor.detectors.detectors)
    {
//        auto& d = processor.detectors[1];
//        ss << "detector: " << i++ << ""
//        " outvalue :" << d.output <<
//        " outvalue normscaled: " << d.normalisedScaledOutput() << "\n";
//        " isWithinRange: " << d.isWithinRange()
//        << " low: " << d.rangeChecker.thresholds[0] << " upper: " << d.rangeChecker.thresholds[1] << " output: " << d.output << "\n";
    }
//
//    std::cout << ss.str() << std::endl;

//    if (processor.detectors[PITCH_SALIENCE].getGateCode() > -1)
//        std::cout << processor.detectors[LEVEL].getGateCode() << std::endl;
    repaint();
}
