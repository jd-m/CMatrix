//
//  IRWaveformEditor.hpp
//  jd_CMatrix
//
//  Created by Jaiden Muschett on 08/09/2017.
//
//

#ifndef IRWaveformEditor_hpp
#define IRWaveformEditor_hpp

#include <stdio.h>
#include <math.h>
#include "../../../JuceLibraryCode/JuceHeader.h"
#include "../../jd-lib/jdHeader.h"
#include "../JDEnvelopeGUI.hpp"

//===================================================================
/* IR State */
//===================================================================
struct IRState {
    IRState(int sourceSamplesPerThumbnailSample,
            AudioFormatManager* formatManagerToUse,
            AudioThumbnailCache* cacheToUse);
    
    IRState (const IRState&other) {
        copyStateFrom(other);
    }
    
    IRState& operator =(const IRState& other) {
        copyStateFrom(other);
        return *this;
    }
    //===================================================================
    double totalDuration() { return thumbnail->getTotalLength(); }
    
    size_t lengthSamples() { return duration * reader->sampleRate; }
    
    size_t startSample() { return std::floor(startTime * reader->sampleRate); }
    //===================================================================
    void drawFullThumbnail      (Graphics &g,
                                 const Rectangle<int> &area,
                                 double zoomFactor);
    
    void drawThumbnailSection   (Graphics &g,
                                 const Rectangle<int> &area,
                                 double zoomFactor,
                                 bool isSelected = false,
                                 Colour selectionColour = Colours::darkorange);
    //===================================================================
    void copyStateFrom( const IRState& target);
    
    String name;
    int uid;
    ScopedPointer<AudioFormatReader> reader;
    int samplesPerThumbnailSample;
    AudioFormatManager* formatManager;//std::shared
    AudioThumbnailCache* thumbnailCache;//std::shared
    ScopedPointer<AudioThumbnail> thumbnail;
    double startTime { 0. };
    double duration { 0. };
    File loadedFile;
    jd::Envelope<float> env;
    bool shouldReverse {false};
};

//===================================================================
/* IR Waveform Editor */
//===================================================================
class IRWaveformEditor :
public Component,
public Button::Listener,
public Slider::Listener,
public ChangeListener
{
public:
    IRWaveformEditor (IRState &sourceIRState);
    //===================================================================
    void paint (Graphics &g) override;
    void resized () override;
    //===================================================================
    void buttonClicked(juce::Button *button) override;
    //===================================================================
    void sliderValueChanged(juce::Slider *slider) override;
    //===================================================================
    void changeListenerCallback(juce::ChangeBroadcaster *source) override;
    //===================================================================
    void updateSelection();
    
    void updateSliderInfo();
    //===================================================================
    void setSliderPos();
    //===================================================================
    IRState &currentIRState;
    
    TextButton m_loadButton;
    Slider selectionSlider;
    
    Rectangle<int>  m_optionBounds,
    m_waveformTopBounds,
    m_waveformInfoBounds,
    m_overlayBounds;
    
    Label selectionStartLabel, selectionEndLabel;
    Label startTimeLabel, endTimeLabel;
    
    TextEditor log;
    String logText;
};
//===================================================================
/* IR Waveform section */
//===================================================================
class IRWaveformSection :
public Component,
public ChangeListener,
public Button::Listener
{
public:
    IRWaveformSection(IRState &sourceIRState);
    
    //Component
    void paint (Graphics &g) override;
    void resized () override;
    //ChangeListener
    void changeListenerCallback(juce::ChangeBroadcaster *source) override {
        repaint();
    }
    
    void setEnvEditable(bool canEditEnvelope)
    {
        if (canEditEnvelope)
        {
            envGui.setAlpha(1.0);
            envGui.setInterceptsMouseClicks(true, true);
            showEnvButton.setButtonText("mk unable.");
        } else {
            envGui.setAlpha(0.1);
            envGui.setInterceptsMouseClicks(false, false);
            showEnvButton.setButtonText("mk able.");
        }
        repaint();
    }
    
    void buttonClicked (Button* button) override;
    
    void makePatternElementInfoEnvelope();
    void setEnvelopeFromCurrentInfo();
    bool envIsEditable = false;
    TextButton showEnvButton;
    
    IRState &currentIRState;
    double m_zoomFactor {1.};
    JDEnvelopeGUI envGui;
    
};

#endif /* IRWaveformEditor_hpp */
