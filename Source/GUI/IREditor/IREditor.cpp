#include "IREditor.hpp"
//===================================================================
/*    IR EDITOR      */
//===================================================================
IREditor::IREditor(Jd_cmatrixAudioProcessor& p):
processor(p)
//analysisEditor(sourceAnalysisEditor)
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
    
    if (!irClipDir.exists())
        irClipDir.createDirectory();
    
//    irInfosComboBox.addListener(&analysisEditor);
    
}
IREditor::~IREditor()
{
    irClipDir.deleteRecursively();
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
    buttonGrid.setBounds(r.removeFromTop(200)
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
    waveformSection.makePatternElementInfoEnvelope();
    
    auto newIRName = irNameLabel.getText();
    
    int newUID = 1;
    for (const auto& info: irInfos)
        if (info.uid == newUID) newUID++;
    
    if (!irInfos.contains(newIRName))
    {
        IRState newIrInfo (currentIrInfo);
        newIrInfo.copyStateFrom(currentIrInfo);
        newIrInfo.uid = newUID;
        newIrInfo.name = newIRName;
        newIrInfo.kernelFile = writeIRClipToFile(newIRName);
        irInfos.set(newIRName, std::forward<IRState>(newIrInfo));
        irInfosComboBox.addItem(newIRName, newUID);
        buttonGrid.addItemToIRComboBoxes(newIRName, newUID);
//        analysisEditor.addIRsToComboBoxes(&irInfosComboBox);
        sendChangeMessage();
        
        std::cout << newIRName << std::endl;
        std::cout << irInfos[newIRName].reader->numChannels << std::endl;
        std::cout << irInfos[newIRName].lengthSamples() << std::endl;
        
        
    }

}
//===================================================================
void IREditor::setCurrentIR()
{
    if (irInfos.contains(irInfosComboBox.getText()))
    {
        auto info = irInfos[irInfosComboBox.getText()];
        currentIrInfo.copyStateFrom(info);
        currentIrInfo.thumbnail->sendChangeMessage();
        sendChangeMessage();
    }
}
//===================================================================
void IREditor::removeIR()
{
    
    if (irInfos.contains(irInfosComboBox.getText())) {
        
            //delete
            irInfos.remove(irInfosComboBox.getText());
            irInfosComboBox.clear();
//            analysisEditor.clearMeterIRcomboBoxes();
            buttonGrid.clearIRComboBoxes();
        
        int i = 1;
        for (const auto& info : irInfos) {
            irInfosComboBox.addItem(info.name,
                                    info.uid);
            buttonGrid.addItemToIRComboBoxes(info.name,
                                             info.uid);

        }
//        analysisEditor.addIRsToComboBoxes(&irInfosComboBox);

    }
}
//===================================================================
File IREditor::writeIRClipToFile(String irInfoName)
{
    if (irInfos.contains(irInfoName))
    {
        auto selectedIrClip = irInfos[irInfoName];
        
        std::cout << " numSamples " << selectedIrClip.lengthSamples()
        << " duration: " << selectedIrClip.totalDuration()
        << " numChannels: "
        << selectedIrClip.reader->numChannels << std::endl;
        AudioSampleBuffer irClipBuf (selectedIrClip.reader->numChannels,
                                     selectedIrClip.lengthSamples());
        
        File outputFile = irClipDir.getNonexistentChildFile(selectedIrClip.name, ".wav", true);
//        File outputFile ("~/Desktop/test.wav");
        
        if (!outputFile.exists())
            outputFile.create();

        FileOutputStream *ostream = outputFile.createOutputStream();
        ScopedPointer<WavAudioFormat> wavFormat = new WavAudioFormat();
        AudioFormatReader* reader = selectedIrClip.reader;
        
        ScopedPointer<AudioFormatWriter> writer =
            wavFormat->createWriterFor(ostream,
                                       reader->sampleRate,
                                       reader->numChannels,
                                       reader->bitsPerSample,
                                       StringPairArray(),0);
        
        const size_t loadBlockSize = 8192;
        
        const size_t totalSamplesToCopy = selectedIrClip.lengthSamples();
        size_t remainingToCopy = totalSamplesToCopy;
        writer->flush();
        
        AudioSampleBuffer tempBuf (reader->numChannels,
                                   loadBlockSize);
        
        
        
        selectedIrClip.env.setIncrementRate(selectedIrClip.reader->sampleRate);
        selectedIrClip.env.trigger();
        

        for (int i = 0; i < selectedIrClip.env.times.size(); i++)
        {
            auto t = selectedIrClip.env.times[i];
            auto l = selectedIrClip.env.levels[i];
            auto c = selectedIrClip.env.curves[i];
            std::cout << "level: " << l << " time:  " << t
            << " curve:  " <<  c << std::endl;
        }
        
        while (remainingToCopy > 0) {
            size_t numToCopy = std::min(remainingToCopy, loadBlockSize);
            size_t readerIndex = totalSamplesToCopy - remainingToCopy + selectedIrClip.startSample();
            
            selectedIrClip.reader->read(&tempBuf, 0, numToCopy, readerIndex, true, true);
            
            
            for (int i = 0; i < numToCopy; i++) {
                selectedIrClip.env.updateAction();
                
                const float logScaledEnvGain = selectedIrClip.env.value();

                for (int chan = 0; chan <  reader->numChannels; chan++)
                    tempBuf.getWritePointer(chan)[i] *= logScaledEnvGain;
            }
    
            writer->writeFromAudioSampleBuffer(tempBuf, 0, numToCopy);
            remainingToCopy -= numToCopy;
        }

        return outputFile;
    };
    return File::nonexistent;
}
