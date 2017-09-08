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

class IRSequencer {
    
    int currentIndex {0};
};

class ButtonGrid : public Component,
public Button::Listener,
public ComboBox::Listener,
public KeyListener,
public ValueTree::Listener
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
    void addItemToComboBoxes(String itemName, int itemID);
    void clearComboBoxes();
    //============================================================
    void valueTreePropertyChanged (ValueTree& tree, const Identifier& property) override
    {
        
    }
    //============================================================

    //============================================================
    bool keyPressed (const KeyPress& key,
                     Component* originatingComponent) override
    {
        if (key.getTextCharacter() == juce_wchar{'s'})
        {
            saveState();
        }
        if (key.getTextCharacter() == juce_wchar{'l'})
        {
            std::cout << "YAK" << std::endl;
            state.copyPropertiesFrom(stored, nullptr);
//            loadState();
        }
        return true;
        
    }
    
    
    void saveState()
    {
        stored = state.createCopy();
    }
//
//    void loadState()
//    {
//
//        for (auto irComboBox : irComboBoxes) {
//            irComboBox->clear();
//            for (int i = 0; i < irComboBox->getNumItems(); i++)
//            {
//                auto id = state.getPropertyAsValue(getComboBoxIDPropertyName(irComboBox, i), nullptr).getValue();
//                
//                auto text = state.getProperty(Identifier(getComboBoxItemTextPropertyName(irComboBox, i))).toString();
////                std::cout << "id: " << ((int)id) <<  " text: " << text <<std::endl;
//                irComboBox->addItem(text, id);
//                
//            }
//        }
//        
//        for (auto buttonCell : buttonCells)
//        {
//            bool loadedButtonState = state.getProperty(getButtonStatePropertyName(buttonCell));
////            std::cout << "buttonState: " << loadedButtonState << std::endl;
//            buttonCell->setToggleState(loadedButtonState, dontSendNotification);
//        }
//    };
    
//    void setButtonToggleStateProperty(Button* button)
//    {
//        state.setProperty(getButtonStatePropertyName(button), button->getToggleState(), nullptr);
//    }
    
    Identifier getButtonStatePropertyName(Button* button)
    {
        return Identifier(button->getName() + ("-ToggleState"));
    }
    
    Identifier getComboBoxIDPropertyName(ComboBox* comboBox, int itemIndex)
    {
        return Identifier( comboBox->getName() + "-ItemId-" + String(itemIndex));
    }
    
    Identifier getComboBoxItemTextPropertyName(ComboBox* comboBox, int itemIndex)
    {
        return Identifier( comboBox->getName() + "-ItemText-" + String(itemIndex));
    }
    
    ValueTree state { "IRSequencerState"};
    
    ValueTree stored;
private:
    template<class Function>
    void performFunctionOnColumn(int columnIndex, Function functionToPerform)
    {
        for (int row = 0; row < numRows; row++)
        {
            functionToPerform(buttonCells[columnIndex * numRows + row]);
        }
    }
    template<class Function>
    void performFunctionOnCells(Function functionToPerform)
    {
        for (int column = 0; column < numColumns; column++) {
            for (int row = 0; row < numRows; row++) {
                
                int cellIndex = column * numRows + row;
                functionToPerform(buttonCells[cellIndex], column, row);
            }
        }
    }
    bool columnHasActiveCell(int columnIndexToCheck)
    {
        bool hasActiveCell = false;
        for (int rowIndex = 0; rowIndex < numRows; rowIndex++)
        {
            int index = columnIndexToCheck * numRows + rowIndex;
            if (buttonCells[index]->getToggleState()) {
                    hasActiveCell = true;
                    break;
            }
        }
        return hasActiveCell;
    }
    void deselectAllCellsInColumn(int columnIndex)
    {
        performFunctionOnColumn(columnIndex, [](ToggleButton* button){
            button->setToggleState(false, dontSendNotification);
        });
    }
    
    int numColumns {16};
    int numRows {4};
    int numElements {numColumns * numRows};

    Array<IRState>& storedIrStates;
    Array<ValueTree> sequenceStates;
    
    Array<IRState> patternStates;
    OwnedArray<ToggleButton> buttonCells;
    OwnedArray<ComboBox> irComboBoxes;
    
    OwnedArray<Label> columnLabels;
    CmatrixLookAndFeel lookAndFeel;
    
    Rectangle<int> gridBounds;
    Rectangle<int> comboBoxColumn;
    
    TextButton storeSequenceButton;
    TextButton removeSequenceButton;
    TextButton loadSequenceButton;
    TextButton clearSequencesButton;
    
    ComboBox sequencesComboBox;
};

#endif /* ButtonGrid_hpp */
