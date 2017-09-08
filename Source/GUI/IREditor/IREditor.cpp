#include "IREditor.hpp"
//===================================================================
/*    IR EDITOR      */
//===================================================================
IREditor::IREditor()
{
    formatManager.registerBasicFormats();
    
    addAndMakeVisible(waveformEditor);
    addChangeListener(&waveformEditor);

    addAndMakeVisible(waveformSection);
    addChangeListener(&waveformSection);
    
    addAndMakeVisible(buttonGrid);

    //ADD IR
    addAndMakeVisible(storeIrButton);
    storeIrButton.setButtonText("store");
    storeIrButton.addListener(this);
    
    addAndMakeVisible(removeIrButton);
    removeIrButton.setButtonText("remove");
    removeIrButton.addListener(this);
    
    addAndMakeVisible(setIrButton);
    setIrButton.setButtonText("set");
    setIrButton.addListener(this);
    
    addAndMakeVisible(removeAllIrsButton);
    removeAllIrsButton.setButtonText("remove all");
    removeAllIrsButton.addListener(this);
    
    addAndMakeVisible(irNameLabel);
    irNameLabel.setEditable(true);
    irNameLabel.setText("ir-name", dontSendNotification);
    
    addAndMakeVisible(irInfosComboBox);
    irInfosComboBox.addListener(this);
    
}
//===================================================================
void IREditor::paint(Graphics& g)
{
    g.fillAll(Colours::darkgrey);
    
    g.setColour(Colours::lightgrey);
    g.fillRoundedRectangle(irNameLabel.getBounds().toFloat(),5);
    
}
//===================================================================
void IREditor::resized()
{
    auto r = getLocalBounds();
    //top
    irEditorBounds = r.removeFromTop(250);
    //mid
    auto midSection = r.removeFromTop(200);
    irWaveformSegmentBounds = midSection.removeFromLeft(400);
    irWaveformSegmentOptionBounds = midSection;
    
    //bottom
    irSequenceBounds = r;
    irSequenceMenuBounds = midSection.removeFromRight(200);
    
    waveformEditor.setBounds(irEditorBounds);
    waveformSection.setBounds(irWaveformSegmentBounds);
    
    auto irOptionColumn = irWaveformSegmentOptionBounds.removeFromLeft(200).reduced(10);
    int buttonHeight = 30;
    irNameLabel.setBounds(irOptionColumn.removeFromTop(buttonHeight).reduced(0, 2));
    storeIrButton.setBounds(irOptionColumn.removeFromTop(buttonHeight).reduced(0, 2));
    setIrButton.setBounds(irOptionColumn.removeFromTop(buttonHeight).reduced(0, 2));
    removeIrButton.setBounds(irOptionColumn.removeFromTop(buttonHeight).reduced(0, 2));
    irInfosComboBox.setBounds(irOptionColumn.removeFromTop(buttonHeight).reduced(0, 2));
    
    auto bottom = r;
    buttonGrid.setBounds(r.removeFromTop(200).
                         removeFromRight(900)
                         .reduced(10,10)    );
}
//===================================================================
void IREditor::buttonClicked(juce::Button *button)
{
    if (button == &storeIrButton)   storeIrInfo();
    if (button == &setIrButton)     setCurrentIR();
    if (button == &removeIrButton)  removeIR();
}
//===================================================================
void IREditor::comboBoxChanged(juce::ComboBox *comboBox)
{
    if (comboBox == &irInfosComboBox);
}
//===================================================================
void IREditor::storeIrInfo()
{
    auto name = irNameLabel.getText();
    
    bool nameAlreadyUsed = false;
    int newUID = 1;
    for (const auto& info: irInfos) {
        nameAlreadyUsed = (name == info.name);
        if (info.uid == newUID) newUID++;
    }
    
    if (!nameAlreadyUsed) {
        IRState newIrInfo (currentIrInfo);
        newIrInfo.uid = newUID;
        newIrInfo.name = name;
        irInfos.add(newIrInfo);
        irInfosComboBox.addItem(name, newUID);
        buttonGrid.addItemToIRComboBoxes(name, newUID);
    }

}
//===================================================================
void IREditor::setCurrentIR()
{
    int selectedItemUID = irInfosComboBox.getSelectedId();
    for (const auto& info : irInfos)
        if (info.uid == selectedItemUID) {
            currentIrInfo.copyStateFrom(info);
            currentIrInfo.thumbnail->sendChangeMessage();
            sendChangeMessage();
        }
}
//===================================================================
void IREditor::removeIR()
{
    
    int selectedIrInfoUID = irInfosComboBox.getSelectedId();

    int index = 0;
    bool removedIr = false;
    for (const auto& info : irInfos)
    {
        if(info.uid == selectedIrInfoUID)
        {
            irInfos.remove(index);
            irInfosComboBox.clear();
            buttonGrid.clearIRComboBoxes();
            removedIr = true;
            break;
        }
        index++;
    }
    if (removedIr)
        for (const auto& info : irInfos) {
            irInfosComboBox.addItem(info.name,
                                    info.uid);
            buttonGrid.addItemToIRComboBoxes(info.name, info.uid);
        }
    
}
