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
: AudioProcessorEditor (&p), processor (p), waveformViewers(p.waveformViewers)
{
    
    
    essentia::init();
    
//    processor.convolver.loadIRToConvolver();
//    processor.convolver.loadIRFromFile(File("~/Music/sc_sounds/beat/jam/jam_chord.wav"), 0);
    
    startTimerHz(20);
    addAndMakeVisible(dbg);
    
    
    File f = File("~/Music/sc_sounds/beat/piano/piano_01.wav");
    processor.convolver.loadIRFromFile(f, 0);
//    processor.convolver.loadIRToConvolver();
    
    addAndMakeVisible(dbg);
    dbg.setMultiLine(true);
    
    for (int i = 0; i < 8; i++)
    {
        auto s = new Slider( Slider::LinearVertical,
                            Slider::TextBoxBelow );
        sliders.add(s);
        addAndMakeVisible(s);
        s->setRange(0.,1.);
    }
    { int i = 0;
    for (auto w : waveformViewers) {
        addAndMakeVisible(w);
        w->setLineColour(detectorColours[i++]);
    }
    }
    setSize (800, 400);
}

Jd_cmatrixAudioProcessorEditor::~Jd_cmatrixAudioProcessorEditor()
{
}

//==============================================================================
void Jd_cmatrixAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll (Colours::white);
    auto r = getLocalBounds();
    r.removeFromTop(200).removeFromLeft(400);
    for(auto h : heights) {
        r.removeFromLeft(40);
        g.drawEllipse(r.getX() + 20, h * 180 + 20, 10, 10,1);
    }
}

void Jd_cmatrixAudioProcessorEditor::resized()
{
    auto r = getLocalBounds();
    dbg.setBounds(r.removeFromLeft(400));
    
    auto waveBounds = r.removeFromBottom(200).withX(0);
    for (auto w : waveformViewers)
        w->setBounds(waveBounds);
    
    int sliceSize = 50;
    
    for (auto s : sliders)
        s->setBounds(400 + (sliders.indexOf(s) * sliceSize),0,50,400);

}

void Jd_cmatrixAudioProcessorEditor::sliderValueChanged(juce::Slider *slider)
{
//    if (slider == sliders)
    
}

void Jd_cmatrixAudioProcessorEditor::timerCallback()
{
    auto& a = processor.analysisChain;
    
    auto d_i = LEVEL;
    String s = String("is within range: " + String( a.detector(d_i).isWithinRange()));
    s+= "\nis above[0]: " + String(a.detector(d_i).isAbove[0])+
    "\nis Above [1]: " + String(a.detector(d_i).isAbove[1]) +
    "\nvalue: " + String(a.outputs[d_i]) +
    "\n threshold 0: " + String(a.detector(d_i).thresholds[0])+
    "\n threshold 1: " + String(a.detector(d_i).thresholds[1])
    ;
    
    for (int i = 0; i < a.NUM_DETECTORS; i++)
    {
        sliders[i]->setValue(a.normalisedOutput(i));
    }

    dbg.setText(s + " solo: " + String(SOLO));
}
