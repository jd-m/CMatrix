#ifndef IRPatternEditor_hpp
#define IRPatternEditor_hpp

#include <stdio.h>
#include <math.h>
#include "../../../JuceLibraryCode/JuceHeader.h"
#include "../../jd-lib/jdHeader.h"
#include "../JDEnvelopeGUI.hpp"

class IRPatternEditor;
class IRPatternSequence;
//===================================================================
/* Pattern Element Info */
//===================================================================
struct IRPatternElementInfo {
    IRPatternElementInfo(int sourceSamplesPerThumbnailSample,
                         AudioFormatManager* formatManagerToUse,
                         AudioThumbnailCache* cacheToUse);
    
    double totalDuration() { return thumbnail->getTotalLength(); }
    size_t lengthSamples() { return duration * reader->sampleRate; }
    size_t startSample() { return std::floor(startTime * reader->sampleRate); }
    
    void drawFullThumbnail      (Graphics &g, const Rectangle<int> &area, double zoomFactor);
    void drawThumbnailSection   (Graphics &g, const Rectangle<int> &area, double zoomFactor, bool isSelected = false, Colour selectionColour = Colours::darkorange);
    
    void copyStateFrom( const IRPatternElementInfo target);
    
    AudioFormatReader* reader;
    int samplesPerThumbnailSample;
    AudioFormatManager *formatManager;
    AudioThumbnailCache *thumbnailCache;
    AudioThumbnail* thumbnail;
    double startTime { 0. };
    double duration { 0. };
    File loadedFile;
    jd::Envelope<float> env;
    bool shouldReverse;
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
    IRWaveformEditor (IRPatternElementInfo &sourceIrPatternElementInfo);
    //Component
    void paint (Graphics &g) override;
    void resized () override;
    //Button Listener
    void buttonClicked(juce::Button *button) override;
    //Slider
    void sliderValueChanged(juce::Slider *slider) override;
    //Change
    void changeListenerCallback(juce::ChangeBroadcaster *source) override;
    //
    void updateSelection();
    void updateSliderInfo();
    void setSliderPos();
    IRPatternElementInfo &m_patternElementInfo;
    
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
    IRWaveformSection(IRPatternElementInfo &sourceIrPatternElementInfo);
    
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
    
    IRPatternElementInfo &m_patternElementInfo;
    double m_zoomFactor {1.};
    JDEnvelopeGUI envGui;
    
};

//===================================================================
/*          */
//===================================================================

//===================================================================
/*     IRSequence     */
//===================================================================

//===================================================================
/*    IR EDITOR      */
//===================================================================
class IREditor : public Component {

    IREditor();
    //FullWaveform
    
    //WaveformSection
    //WaveformSectionEnvelope
    //AddElement
    //AddToSet
};


#endif /* IREditor_hpp */
