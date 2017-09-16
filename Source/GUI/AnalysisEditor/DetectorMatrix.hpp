#ifndef DetectorMatrix_hpp
#define DetectorMatrix_hpp

#include <stdio.h>
#include "../../../JuceLibraryCode/JuceHeader.h"
#include "../../jd-lib/jdHeader.h"
#include "LookAndFeel.hpp"
#include "../../PluginProcessor.h"


class DetectorMatrix : public Component,
public Button::Listener
{
public:
    DetectorMatrix(Jd_cmatrixAudioProcessor&p,
                   int sourceDetectorIndex);
    
    void paintOverChildren(Graphics& g) override;
    void resized() override;
    //====================================================
    void buttonClicked(Button* changedButton) override;
  //====================================================
    template<class Container, class Type>
    int getIndexAtItem(Container& container, Type& item) {
        
        for (int i = 0; i < container.size(); i++)
            if (container[i] == item)
                return i;
        return -1;
    }
    //====================================================
    bool shouldBypassColumnAt(int index)
    {
        return setRequireWithinRangeButtons[index]->getToggleState() &&
        setRequireOutsideRangeButtons[index]->getToggleState();
    }
    //====================================================
//private:
    
    int detectorIndex;
    using RequiredDetectorState = Jd_cmatrixAudioProcessor::RequiredDetectorState;
    
    Jd_cmatrixAudioProcessor& processor;

    int numColumns { 5 };
    
    OwnedArray<ToggleButton> setRequireWithinRangeButtons;
    OwnedArray<ToggleButton> setRequireOutsideRangeButtons;
    
    Array<Colour> columnColours;
    CmatrixLookAndFeel lookAndFeel;
};

#endif /* DetectorMatrix_hpp */
