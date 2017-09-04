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
    addAndMakeVisible(dbg);
    
    
    File f = File("~/Music/sc_sounds/beat/piano/piano_01.wav");
    processor.convolver.loadIRFromFile(f, 0);
//    processor.convolver.loadIRToConvolver();
    
    dbg.setMultiLine(true);
    
    for (int i = 0; i < 8; i++)
    {
        auto s = new Slider( Slider::LinearVertical,
                            Slider::TextBoxBelow );
        sliders.add(s);
        addAndMakeVisible(s);
        s->setRange(0.,1.);
    }

    addAndMakeVisible(waveformViewer);
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
    
    waveformViewer.setBounds(r.removeFromBottom(200).withX(0));
    
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
    
    String s = "is within range: " + String(
        processor.gate.isWithinRange()
    );
    
//    auto& chain = processor.analysisChain;
//    s += "\n" + String(processor.dbg_meter);
//    "RMS: " + String(chain.detectorSignal[0]) + " is Within Range: " +
//    String(chain.detector(chain.LEVEL).isWithinRange()) + "\n"
//    
//    "PITCH: " + String(chain.pitchYinFFT.output<0>()) + " is Within Range: " +
//    String(chain.detector(chain.PITCH).isWithinRange()) + "\n"
//    
//    "PITCH_CONFIDENCE: " + String(chain.pitchYinFFT.output<1>()) + " is Within Range: " +
//    String(chain.detector(chain.PITCH_CONFIDENCE).isWithinRange()) + "\n"
//    
//    "PITCH_SALIENCE: " + String(chain.pitchSalience.output<0>()) + " is Within Range: " +
//    String(chain.detector(chain.PITCH_SALIENCE).isWithinRange()) + "\n"
//    
//    "INHARMONICITY: " + String(chain.inharmonicity.output<0>()) + " is Within Range: " +
//    String(chain.detector(chain.INHARMONICITY).isWithinRange()) + "\n"
//    ;
//    int count  = 1;
//    for (auto mag : processor.mixedBuf)
//        s += String(mag) + (((++count % 5) == 0) ? "\n" : ", ");
//    s+= String(processor.pitchSalienceAnalyser.pitchSalience());
//    s+= "\ninharm" +String(processor.inharmonicityAnalyser.inharmonicity());
//    s+="\n";
    
//    if (count > 0)
        dbg.setText(s + "\n" + String(processor.gate.text));
}
