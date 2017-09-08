//
//  ButtonGrid.hpp
//  jd_CMatrix
//
//  Created by Jaiden Muschett on 07/09/2017.
//
//

#ifndef ButtonGrid_hpp
#define ButtonGrid_hpp

#include <stdio.h>
#include "../../jd-lib/jdHeader.h"
#include "../../../JuceLibraryCode/JuceHeader.h"
#include "LookAndFeel.hpp"
#include "IRWaveformEditor.hpp"


struct IRSequenceElement {
    int stepIndex;
    int irClipindex;
    bool isEnabled;
};
using IRSequence = Array<IRSequenceElement>;


class ButtonGrid : public Component,
public Button::Listener,
public ComboBox::Listener,
public KeyListener
{
public:
    ButtonGrid(Array<IRState>& sourceIRStates);
    //====================================================
    void paint(Graphics& g) override;
    void paintOverChildren (Graphics& g) override;
    void resized() override;
    //====================================================
    void buttonClicked (Button* clickedButton) override;
    //====================================================
    void comboBoxChanged(ComboBox* changedComboBox) override;
    //====================================================
    const bool getToggleStateAt(int n);
    const bool getToggleStateAt(int row, int column);
    //============================================================
    void addItemToIRComboBoxes(String itemName, int itemID);
    void clearIRComboBoxes();
    //===========================================================
    void setParametersToReferToValueTreeProperties();
    bool keyPressed (const KeyPress& key,
                     Component* originatingComponent) override
    {
        if (key.getTextCharacter() == juce_wchar{'s'})
        {
        }
        if (key.getTextCharacter() == juce_wchar{'n'})
        {
            std::cout << currentSequenceState.getProperty("name").toString() << std::endl;
        }
        if (key.getTextCharacter() == juce_wchar{'l'})
        {
//            for (const auto& irSequence : irSequences)
//            {
//                for (auto el : irSequence.second)
//                {
//                    std::cout << "sequence name: " <<
//                    irSequence.first << " stepIndex: "
//                    <<  el.stepIndex << " irClipIndex " << el.irClipindex
//                    << " isEnabled " << el.isEnabled << std::endl;
//                }
//            }
//            std::cout << irSequences.size() << std::endl;
        }
        return true;
        
    }

    static Identifier getButtonStatePropertyName(Button* button)
    {
        return Identifier(button->getName() + ("-ToggleState"));
    }
    static Identifier getComboBoxIDPropertyName(ComboBox* comboBox, int itemIndex)
    {
        return Identifier( comboBox->getName() + "-ItemId-" + String(itemIndex));
    }
    static Identifier getComboBoxItemTextPropertyName(ComboBox* comboBox, int itemIndex)
    {
        return Identifier( comboBox->getName() + "-ItemText-" + String(itemIndex));
    }

private:
    //===========================================================
    void storeCurrentSequence();
    void overwriteCurrentSequence();
    void removeCurrentSequence();
    void setCurrentSequence();
    void clearCurrentSequence();
    void clearAllSequences();
    //===========================================================
    IRSequence generateIRSequenceFromCurrentState();
    //===========================================================
    template<class Function>
    void performFunctionOnColumn(int columnIndex, Function functionToPerform);
    //===========================================================
    template<class Function>
    void performFunctionOnCells(Function functionToPerform);
    //===========================================================
    bool columnHasActiveCell(int columnIndexToCheck);
    int getActiveRowInColumn(int columnIndex);
    void deselectAllCellsInColumn(int columnIndex);
    //===========================================================
    
    int numColumns {16};
    int numRows {4};
    int numElements {numColumns * numRows};

    ValueTree currentSequenceState { "IRSequencerState"};
    HashMap<String, ValueTree> sequenceStates;
    
    Array<IRState>& storedIrStates;

    HashMap<String, IRSequence> irSequences;
    //===========================================================
    OwnedArray<ToggleButton> buttonCells;
    OwnedArray<ComboBox> irComboBoxes;
    
    OwnedArray<Label> columnLabels;
    CmatrixLookAndFeel lookAndFeel;
    
    Rectangle<int> gridBounds;
    
    Label sequenceNameLabel;
    
    TextButton storeSequenceButton;
    TextButton removeSequenceButton;
    TextButton overwriteSequenceButton;
    TextButton setSequenceButton;
    TextButton clearCurrentSequenceButton;
    TextButton clearSequencesButton;
    
    ComboBox sequencesComboBox;
};
//=========================================================================
/*              IR SEQUENCER
    Each Convolution Chain has an instance of an IR SEQUENCER which it
    uses to step through a sequence stored by the button grid;
 */
//=========================================================================
class IRSequencer {
    
    IRSequencer(Array<IRState>& sourceIRClipDefs,
                ValueTree& valueTreeToReferFrom);

    Array<IRState>& irClipDefs;
    
    void reset();
    
    
    int currentIndex {0};
};


#endif /* ButtonGrid_hpp */
