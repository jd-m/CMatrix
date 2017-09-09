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
    String irClipName;
    bool isEnabled;
};
using IRSequence = Array<IRSequenceElement>;

class ButtonGrid : public Component,
public Button::Listener,
public ComboBox::Listener,
public Value::Listener
{
public:
    ButtonGrid(HashMap<String, IRState>& sourceIRStates);
    //====================================================
    void paint(Graphics& g) override;
    void paintOverChildren (Graphics& g) override;
    void resized() override;
    //====================================================
    void buttonClicked (Button* clickedButton) override;
    //====================================================
    void comboBoxChanged(ComboBox* changedComboBox) override;
    //====================================================
    void valueChanged(Value& changedValue) override;
    //====================================================
    const bool getToggleStateAt(int n);
    const bool getToggleStateAt(int row, int column);
    //============================================================
    void addItemToIRComboBoxes(String itemName, int itemID);
    void clearIRComboBoxes();
    //===========================================================
    void setParametersToReferToValueTreeProperties();
    String getCurrentStateName();

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

    friend class AnalysisEditor;
    friend class IRSequencer;
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
    ValueTree waitingSequenceState { "WaitingIRSequencerState"};
    HashMap<String, ValueTree> sequenceStates;
    
    HashMap<String, IRState>& storedIrStates;
    
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

#endif /* ButtonGrid_hpp */
