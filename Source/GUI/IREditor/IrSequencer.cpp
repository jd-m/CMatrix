//
//  ButtonGrid.cpp
//  jd_CMatrix
//
//  Created by Jaiden Muschett on 07/09/2017.
//
//

#include "IrSequencer.hpp"

//============================================================
ButtonGrid::ButtonGrid(Array<IRState>& sourceIRStates):
storedIrStates(sourceIRStates)
{
    for (int i = 0; i < numElements; ++i)
    {
        auto newToggleButton = new ToggleButton ();
        newToggleButton->setLookAndFeel(&lookAndFeel);
        newToggleButton->addListener(this);
        auto name = "ButtonCell-" + String(i);
        newToggleButton->setName(name);
        addAndMakeVisible(newToggleButton);
        buttonCells.add(newToggleButton);
    }
    
    patternStates.ensureStorageAllocated(numRows);
    for (int row = 0; row < numRows; row++) {
        auto newIRComboBox = new ComboBox();
        newIRComboBox->addListener(this);
        newIRComboBox->setName("IRComboBox_" + String(row));
        addAndMakeVisible(newIRComboBox);
        newIRComboBox->setTextWhenNoChoicesAvailable("ir -" + String(row+1));
        irComboBoxes.add(newIRComboBox);
    }
    
    for (auto buttonCell: buttonCells)
    {
        buttonCell->getToggleStateValue().referTo(
                                                  state.getPropertyAsValue(getButtonStatePropertyName(buttonCell),nullptr));
    }
    for (auto irComboBox : irComboBoxes)
    {
        for (int itemIndex = 0; itemIndex < irComboBox->getNumItems(); itemIndex++)
        {
            state.setProperty( getComboBoxIDPropertyName(irComboBox,
                                                         itemIndex),
                              irComboBox->getSelectedId(),
                              nullptr);
            
            state.setProperty(
                              getComboBoxItemTextPropertyName(irComboBox,
                                                              itemIndex),
                              irComboBox->getItemText(itemIndex),
                              nullptr);
        }
    }
    
    
}
//============================================================
void ButtonGrid::paint(juce::Graphics &g)
{
    for (const auto& buttonCell : buttonCells)
        g.drawRect(buttonCell->getBounds());
}
 //============================================================}
void ButtonGrid::paintOverChildren (juce::Graphics &g)
{
    int columnWidth = (float)gridBounds.getWidth()/(float)numColumns;
    
    auto columnBounds = gridBounds;
    for (int column = 0; column < numColumns; column++)
    {
        auto currentColumnBounds = columnBounds.removeFromLeft(columnWidth);
        g.setColour(Colours::darkgrey.withAlpha((float)0.5f));
        if (!columnHasActiveCell(column))
            g.fillRect(currentColumnBounds);
    }
}
//============================================================
void ButtonGrid::resized()
{
    auto r = getLocalBounds();

    auto boxRowBounds = r.removeFromLeft(80);
    
    gridBounds = r;
    
    const int columnWidth = (float)gridBounds.getWidth()/(float)numColumns;
    const int rowHeight = (float)gridBounds.getHeight()/(float)numRows;
    
    for (int column = 0; column < numColumns; column++) {
        
        auto columnBounds = r.removeFromLeft(columnWidth);
        
        for (int row = 0; row < numRows; row++) {
            
            int n = column * numRows + row;
            std::cout << "n: " << n << "row: " << row << " column: " << column << std::endl;
            buttonCells[n]->setBounds(columnBounds.removeFromTop(rowHeight));
        }
    }
    
    for (int row = 0; row < numRows; row++) {
        irComboBoxes[row]->setBounds(boxRowBounds.removeFromTop(columnWidth).reduced(2, 10));
    }
    
}
//============================================================
void ButtonGrid::buttonClicked(Button* clickedButton)
{
    for (int column = 0; column < numColumns; column++) {
        for (int row = 0; row < numRows; row++) {
            
            int n = column * numRows + row;

            if (clickedButton == buttonCells[n]) {
                std::cout << "n: " << n << "row: " << row << " column: " << column << std::endl;
                bool tempButtonState = clickedButton->getToggleState();
                deselectAllCellsInColumn(column);
                buttonCells[n]->setToggleState(tempButtonState, dontSendNotification);
            }
        }
    }
}
//============================================================
void ButtonGrid::comboBoxChanged(juce::ComboBox *changedComboBox)
{
    for (auto irComboBox : irComboBoxes)
        if (changedComboBox == irComboBox) {
            int selectedUID = irComboBox->getSelectedId();
            
            int index = 0;
            for (auto storedState : storedIrStates) {
                if (storedState.uid == selectedUID) {
                    
                    break;
                }
                index++;
            }
            
        }
}
//============================================================
const bool ButtonGrid::getToggleStateAt(int n)
{
    return buttonCells[n]->getToggleState();
}
//============================================================
const bool ButtonGrid::getToggleStateAt(int row, int column)
{
    return getToggleStateAt(row * numRows + column);
}
//============================================================
void ButtonGrid::addItemToComboBoxes(juce::String itemName, int itemID)
{
    for (auto irComboBox : irComboBoxes)
        irComboBox->addItem(itemName, itemID);
}
//============================================================
void ButtonGrid::clearComboBoxes()
{
    for (auto irCombobox : irComboBoxes)
        irCombobox->clear();
}
