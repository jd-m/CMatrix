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
    
    
    essentia::init();
    
//    processor.convolver.loadIRToConvolver();
//    processor.convolver.loadIRFromFile(File("~/Music/sc_sounds/beat/jam/jam_chord.wav"), 0);
    
    startTimerHz(20);
//    addAndMakeVisible(dbg);
    
    
    File f = File("~/Music/sc_sounds/beat/piano/piano_01.wav");
    processor.convolver.loadIRFromFile(f, 0);
//    processor.convolver.loadIRToConvolver();
    
    dbg.setMultiLine(true);

//    sliders.push_back(std::move(ScopedPointer<Slider>(new Slider)));
    sliders.clear();
    for (int i = 0; i < 8; i++)
    {
        auto s = new Slider( Slider::LinearVertical,
                            Slider::TextBoxBelow );
        sliders.add(s);
        addAndMakeVisible(s);
        s->setRange(0.,1.);
    }
//
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
  
}

void Jd_cmatrixAudioProcessorEditor::resized()
{
    auto r = getLocalBounds();
    dbg.setBounds(r.removeFromLeft(400));
    
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
    String s = String(
    );
    
    auto& chain = processor.analysisChain;
    s += String(chain.inharmonicity.output<0>());
    int count  = 1;
//    for (auto mag : processor.spectralPeakAnalyser.magnitudes())
//        s += String(mag) + (((++count % 10) == 0) ? "\n" : ", ");
//    s+= String(processor.pitchSalienceAnalyser.pitchSalience());
//    s+= "\ninharm" +String(processor.inharmonicityAnalyser.inharmonicity());
    s+="\n";
    
    if (count > 0)
        dbg.setText(s);
}
