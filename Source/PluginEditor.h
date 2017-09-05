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
#include <memory.h>

//==============================================================================
/**
*/
class Jd_cmatrixAudioProcessorEditor  : public AudioProcessorEditor,
public Timer,
public Slider::Listener
{
public:
    Jd_cmatrixAudioProcessorEditor (Jd_cmatrixAudioProcessor&);
    ~Jd_cmatrixAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    void timerCallback() override;
    void sliderValueChanged(Slider* slider) override;

private:

    Jd_cmatrixAudioProcessor& processor;
    
    OwnedArray<AudioInputWaveformDisplay>& waveformViewers;
    Array<Colour> detectorColours {
        Colours::darkorange,
        Colours::darkred,
        Colours::black,
        Colours::skyblue,
        Colours::purple
    };
    Detectors& SOLO { processor.SOLO };
    TextEditor dbg;
    
    OwnedArray<Slider> sliders;
    
    std::vector<float> heights { std::vector<float> (5,0.) };
    
    String detectorInfo (int d_i)
    {
        auto& a = processor.analysisChain;
        
        auto s = String("is within range: " + String( a.detector(d_i).isWithinRange()));
        s+= "\nis above[0]: " + String(a.detector(d_i).isAbove[0])+
        "\nis Above [1]: " + String(a.detector(d_i).isAbove[1]) +
        "\nvalue: " + String(a.outputs[d_i]) +
        "\n threshold 0: " + String(a.detector(d_i).thresholds[0])+
               "\n threshold 1: " + String(a.detector(d_i).thresholds[1])
        ;
        return s;
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Jd_cmatrixAudioProcessorEditor)
};


#endif  // PLUGINEDITOR_H_INCLUDED
