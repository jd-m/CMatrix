#ifndef IRPatternEditor_hpp
#define IRPatternEditor_hpp

#include <stdio.h>
#include <math.h>
#include "../../../JuceLibraryCode/JuceHeader.h"
#include "../../jd-lib/jdHeader.h"
#include "../JDEnvelopeGUI.hpp"
#include "IrSequencer.hpp"
//===================================================================
/*    IR EDITOR      */
//===================================================================
class IREditor :
public Component,
public Button::Listener,
public ComboBox::Listener,
public ChangeBroadcaster
{
public:
    
    IREditor();
    //===================================================================
    void paint(Graphics& g) override;
    void resized() override;
    //===================================================================
    void buttonClicked (Button* button) override;
    //===================================================================
//    void sliderValueChanged (Slider* slider) override;
    //===================================================================
    void comboBoxChanged(ComboBox* comboBox) override;
    //===================================================================
    void storeIrInfo();
    void setCurrentIR();
    void removeIR();
    
    AudioFormatManager formatManager;
    AudioThumbnailCache thumbnailCache { 100 };
    
    IRState currentIrInfo {
        32,
        &formatManager,
        &thumbnailCache
    };
    
    IRWaveformEditor    waveformEditor  { currentIrInfo };
    IRWaveformSection   waveformSection { currentIrInfo };
    
    Rectangle<int>      irEditorBounds,
                        irEditorOptionBounds,
                        irWaveformSegmentBounds,
                        irWaveformSegmentOptionBounds,
                        irSequenceBounds,
                        irSequenceMenuBounds;
    

    Array<IRState> irInfos;
    
    //IR
    Label irNameLabel;
    TextButton storeIrButton;
    TextButton setIrButton;
    TextButton removeIrButton;
    TextButton removeAllIrsButton;
    ComboBox irInfosComboBox;

    //Sequencer
    ButtonGrid buttonGrid { irInfos };
    
    
    
    //============================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(IREditor)
};


#endif /* IREditor_hpp */
