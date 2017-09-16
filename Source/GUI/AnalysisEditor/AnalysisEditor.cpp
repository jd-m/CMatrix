#include "AnalysisEditor.hpp"


AnalysisEditor::AnalysisEditor(Jd_cmatrixAudioProcessor& p):
processor(p)
{
    setOpaque(true);
    
    for (int i = 0; i  < NUM_DETECTORS; i++)
    {
        auto w = waveformViewers[i];
        addAndMakeVisible(w);
        w->setLineColour(detectorColours[i]);
    }
    
    addAndMakeVisible(waveformDisplay);
    waveformDisplay.setMode(WaveformDisplay::LOG_AMP);
    
    addAndMakeVisible(setDisplayAnnotation);
    setDisplayAnnotation.addListener(this);
    setDisplayAnnotation.addItemList({"Linear Normalised",
                                        "Log Amp (dB)",
                                        "Log Freq (Hz)" }, 1);
    
    addAndMakeVisible(setActiveDetector);
    setActiveDetector.addItemList({"level", "pitch", "pitch confidence", "pitch_salnce", "inharmoncity" , "all"}, 1);
    setActiveDetector.addListener(this);
    
    auto meterNames = {"amp", "pitch", "pitch confidence", "pitch salience", "inharmonicity"};

    //ANALYSIS METERS
    
    for (int i = 0; i < NUM_DETECTORS; i++)
    {
        auto& d = processor.detectors[i];
        auto m = new AnalysisMeter(d);
        addAndMakeVisible(m);
        m->invertRangeButton.addListener(this);
        m->enableButton.addListener(this);
        m->thresholdSlider.setRange(d.limits.lower, d.limits.upper);
    
        meters.add(m);
        
        //ENV ATTACK
        auto newAttackTimeKnob = new Slider();
        addChildComponent(newAttackTimeKnob);
        newAttackTimeKnob->addListener(this);
        newAttackTimeKnob->setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
        newAttackTimeKnob->setTextBoxStyle(Slider::TextBoxBelow, false, 40, 20);
        newAttackTimeKnob->setRange(0.001, 0.5);
        attackTimeKnobs.add(newAttackTimeKnob);
        
        //RMS WINDOW SIZE
        auto newReleaseTimeKnob = new Slider();
        addChildComponent(newReleaseTimeKnob);
        newReleaseTimeKnob->addListener(this);
        newReleaseTimeKnob->setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
        newReleaseTimeKnob->setTextBoxStyle(Slider::TextBoxBelow, false, 40, 20);
        newReleaseTimeKnob->setRange(0.001, 0.5);
        releaseTimeKnobs.add(newReleaseTimeKnob);
        
        //RMS WINDOW SIZE
        auto newRmsSizeKnob = new Slider();
        addChildComponent(newRmsSizeKnob);
        newRmsSizeKnob->addListener(this);
        newRmsSizeKnob->setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
        newRmsSizeKnob->setTextBoxStyle(Slider::TextBoxBelow, false, 40, 20);
        newRmsSizeKnob->setRange(1, 500);
        rmsKnobs.add(newRmsSizeKnob);
        
        //Smoothing Speed
        auto newSmoothingSpeedKnob = new Slider();
        addChildComponent(newSmoothingSpeedKnob);
        newSmoothingSpeedKnob->addListener(this);
        newSmoothingSpeedKnob->setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
        newSmoothingSpeedKnob->setTextBoxStyle(Slider::TextBoxBelow, false, 40, 20);
        newSmoothingSpeedKnob->setRange(0.001, 0.5);
        smoothingSpeedKnobs.add(newSmoothingSpeedKnob);
        
        //Smoothing Speed
        auto newSamplesPerPixelKnob = new Slider();
        addChildComponent(newSamplesPerPixelKnob);
        newSamplesPerPixelKnob->addListener(this);
        newSamplesPerPixelKnob->setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
        newSamplesPerPixelKnob->setTextBoxStyle(Slider::TextBoxBelow, false, 40, 20);
        newSamplesPerPixelKnob->setRange(32, 512);
        samplesPerPixelKnobs.add(newSamplesPerPixelKnob);
    
        
        auto triggerConditionNames = {"exitFromBelow",
            "entryToBelow",
            "entryToAbove",
            "exitFromAbove",
            "entryToRange",
            "exitFromRange" };
        auto newTriggerConditionComboBox = new ComboBox();
        addAndMakeVisible(newTriggerConditionComboBox);
        newTriggerConditionComboBox->addListener(this);
        newTriggerConditionComboBox->addItemList(triggerConditionNames, 1);
        newTriggerConditionComboBox->setSelectedItemIndex(0);
        triggerConditionComboBoxes.add(newTriggerConditionComboBox);
        
        auto newReleaseConditionComboBox = new ComboBox();
        addAndMakeVisible(newReleaseConditionComboBox);
        newReleaseConditionComboBox->addListener(this);
        newReleaseConditionComboBox->addItemList(triggerConditionNames, 1);
        newReleaseConditionComboBox->setSelectedItemIndex(0);
        
        releaseConditionComboBoxes.add(newReleaseConditionComboBox);
        

        auto setEnvelopeModeBox = new ComboBox();
        addAndMakeVisible(setEnvelopeModeBox);
        setEnvelopeModeBox->addListener(this);
        setEnvelopeModeBox->setSelectedItemIndex(0);
        setEnvelopeModeBox->addItemList({"oneshot", "sustain"}, 1);
        setEnvelopeModeBoxes.add(setEnvelopeModeBox);
        
        auto newLoadIRButton = new TextButton();
        newLoadIRButton->setButtonText("load ir");
        newLoadIRButton->addListener(this);
        addAndMakeVisible(newLoadIRButton);
        loadIRButtons.set( i, newLoadIRButton);
  
        auto newEnvelopeAttackSlider = new Slider(
                                                    Slider::SliderStyle::RotaryHorizontalVerticalDrag,
                                                    Slider::TextBoxBelow);
        newEnvelopeAttackSlider->setRange(0.1, 20.);
        newEnvelopeAttackSlider->setTextBoxStyle(Slider::TextBoxBelow, false, 50, 20);
        addChildComponent(newEnvelopeAttackSlider);
        newEnvelopeAttackSlider->addListener(this);
        envelopeAttackTimeKnobs.add(newEnvelopeAttackSlider);
        
        auto newEnvelopeDecaySlider = new Slider(
                                                  Slider::SliderStyle::RotaryHorizontalVerticalDrag,
                                                  Slider::TextBoxBelow);
        newEnvelopeDecaySlider->setRange(0.1, 20.);
        addChildComponent(newEnvelopeDecaySlider);
        newEnvelopeDecaySlider->setTextBoxStyle(Slider::TextBoxBelow, false, 50, 20);
        newEnvelopeDecaySlider->addListener(this);
        envelopeDecayTimeKnobs.add(newEnvelopeDecaySlider);
        
        auto newEnvelopeReleaseSlider = new Slider(
                                                  Slider::SliderStyle::RotaryHorizontalVerticalDrag,
                                                  Slider::TextBoxBelow);
        newEnvelopeReleaseSlider->setRange(0.1, 20.);
        addChildComponent(newEnvelopeReleaseSlider);
        newEnvelopeReleaseSlider->addListener(this);
        envelopeReleaseTimeKnobs.add(newEnvelopeReleaseSlider);
        
        auto newEnvelopeSustainSlider = new Slider(
                                                  Slider::SliderStyle::RotaryHorizontalVerticalDrag,
                                                  Slider::TextBoxBelow);
        newEnvelopeSustainSlider->setRange(0.1, 20.);
        newEnvelopeSustainSlider->addListener(this);
        addChildComponent(newEnvelopeSustainSlider);
        envelopeSustainTimeKnobs.add(newEnvelopeSustainSlider);
        
        auto newEnvelopeLevelSlider = new Slider(
                                                   Slider::SliderStyle::RotaryHorizontalVerticalDrag,
                                                   Slider::TextBoxBelow);
        newEnvelopeLevelSlider->setRange(-80.,6.);
        addChildComponent(newEnvelopeLevelSlider);
        newEnvelopeLevelSlider->addListener(this);
        envelopeLevelKnobs.add(newEnvelopeLevelSlider);
        
        //MATRIX
        auto newDetectorMatrix = new DetectorMatrix(p, i);
        addChildComponent(newDetectorMatrix);
        detectorMatrices.add(newDetectorMatrix);
    }
    
    {
        int i = 0;
        for (auto meterName : meterNames)
            meters[i++]->setName(meterName);
    }
        
    meters[PITCH]->thresholdSlider.setSkewFactor(0.125);
    meters[LEVEL]->thresholdSlider.setSkewFactor(0.5, true);
    
    
    addAndMakeVisible(matrixDetectorBox);
    matrixDetectorBox.addListener(this);
    matrixDetectorBox.addItemList(meterNames, 1);
    matrixDetectorBox.setSelectedItemIndex(0);
    
    //Env PAram Selection
    addAndMakeVisible(setEnvelopeComboBox);
    setEnvelopeComboBox.addListener(this);
    setEnvelopeComboBox.addItemList(meterNames, 1);
    setEnvelopeComboBox.setSelectedItemIndex(0);

    
    addAndMakeVisible(useSideChainButton);
    useSideChainButton.setToggleState(false, dontSendNotification);
    useSideChainButton.setLookAndFeel(&lookAndFeel);
    
    addAndMakeVisible(dryGainDBSlider);
    dryGainDBSlider.addListener(this);
    dryGainDBSlider.setRange(-120., 12.);
    dryGainDBSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 40, 15);
    dryGainDBSlider.setSliderStyle(Slider::SliderStyle::LinearBarVertical);
    
    addAndMakeVisible(wetGainDBSlider);
    wetGainDBSlider.addListener(this);
    wetGainDBSlider.setRange(-120.f, 12.f);
    wetGainDBSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 40, 15);
    wetGainDBSlider.setSliderStyle(Slider::SliderStyle::LinearBarVertical);
    
    addAndMakeVisible(inputGainDBSlider);
    inputGainDBSlider.addListener(this);
    inputGainDBSlider.setRange(-120.f, 12.f);
    inputGainDBSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 40, 15);
    inputGainDBSlider.setSliderStyle(Slider::SliderStyle::LinearBarVertical);
   
}
//=====================================================================
void AnalysisEditor::paint(Graphics& g)
{
    auto r = getLocalBounds();
    auto top = r.removeFromTop(250);
    
    g.fillAll(Colours::darkgrey);
    
    g.setColour(Colours::black);
    g.drawRect(top);

    g.fillRect(detectorDrawingBounds);
    g.setFont(Font("arial", 12,0));
    
    g.drawRoundedRectangle(envelopeParameterPanel.toFloat(), 10.f, 2);
    g.setColour(Colours::lightgrey);
    
    g.drawText("envelope parameters",
               envelopeParameterPanel.getCentreX() - 70,
               envelopeParameterPanel.getY(),
               140,
               30, Justification::centred);
    
    
    
    auto drawEnvParamLabel = [&](String name, OwnedArray<Slider>& envParams){
        g.setColour(Colours::darkorange.withAlpha(0.8f));
        auto paramBounds = envParams.getFirst()->getBounds();
        g.drawText(name,
                   paramBounds.getX() - 30,
                   paramBounds.getBottom(),
                   paramBounds.getWidth() + 60,
                   20,
                   Justification::centred);
    };
    
    drawEnvParamLabel("ATK", envelopeAttackTimeKnobs);
    drawEnvParamLabel("DEC", envelopeDecayTimeKnobs);
    drawEnvParamLabel("SUS", envelopeSustainTimeKnobs);
    drawEnvParamLabel("REL", envelopeReleaseTimeKnobs);
    drawEnvParamLabel("GAIN", envelopeLevelKnobs);
    
    drawEnvParamLabel("ATK", attackTimeKnobs);
    drawEnvParamLabel("RELEASE", releaseTimeKnobs);
    drawEnvParamLabel("RMS SIZE", rmsKnobs);
    drawEnvParamLabel("SMOOTH", smoothingSpeedKnobs);

    g.drawText("WET", wetGainDBSlider.getBounds().withHeight(20).withWidth(25).translated(0, wetGainDBSlider.getHeight() + 5), Justification::centred);
    
    g.drawText("DRY", dryGainDBSlider.getBounds().withHeight(20).withWidth(25).translated(0, dryGainDBSlider.getHeight() + 5), Justification::centred);
    g.drawText("IN", inputGainDBSlider.getBounds().withHeight(20).withWidth(25).translated(0, inputGainDBSlider.getHeight() + 5), Justification::centred);
    
    g.setColour(Colours::black);
    g.drawRoundedRectangle(masterControlBounds.toFloat(), 5.f, 2);
    g.setColour(Colours::darkorange);
    g.drawText("S/C", useSideChainButton.getBounds().translated(0,20), Justification::centred);
    

    g.fillRect(meterBounds.removeFromLeft(100));
}
//=====================================================================
void AnalysisEditor::resized()
{
    auto r = getLocalBounds();
    auto top = r.removeFromTop(250);
    
    detectorDrawingBounds = top.removeFromLeft(600);
    positionDetectorDrawing(detectorDrawingBounds);
    
    analysisSettingBounds = top;
    positionDetectorSettings(analysisSettingBounds);
    
    auto middle = r.removeFromTop(250);
    meterBounds = middle.removeFromLeft(600);
    positionMeters(meterBounds);
    positionEditPanel(middle);
    
    auto bottom = r.removeFromTop(250);
    positionMeterButtons(bottom.removeFromLeft(600));
    
    masterControlBounds = bottom;
    positionMasterPanel(masterControlBounds);
}
//=====================================================================
void AnalysisEditor::positionDetectorDrawing(const Rectangle<int>& sectionBounds)
{
    auto waveformViewerBounds = sectionBounds;
    for (int i = 0; i  < NUM_DETECTORS; i++)
    {
        waveformViewers[i]->setBounds(waveformViewerBounds);
    }
    waveformDisplay.setBounds(sectionBounds);
}
//=====================================================================
void AnalysisEditor::positionDetectorSettings(const Rectangle<int>& sectionBounds)
{
    
    auto b = sectionBounds.reduced(20);
    
    
    auto top = b.removeFromTop(50);
    
    setDisplayAnnotation.setBounds(top.removeFromTop(25));
    setActiveDetector.setBounds(top.removeFromTop(20).reduced(5, 2));
    
    auto left = b.removeFromLeft(90);
    
    auto attackTimeKnobBounds = left.removeFromTop(75).reduced(5);
    auto rmsKnobBounds = left.removeFromTop(75).reduced(5).translated(0, 10);
    
    auto right = b;
    auto releaseTimeKnob = right.removeFromTop(75).reduced(2);
    auto smoothingSpeedKnob = right.removeFromTop(75).reduced(5).translated(0, 10);
    
    for (int i = 0; i < NUM_DETECTORS; i++)
    {
        attackTimeKnobs[i]->setBounds(attackTimeKnobBounds);
        releaseTimeKnobs[i]->setBounds(releaseTimeKnob);
        rmsKnobs[i]->setBounds(rmsKnobBounds);
        smoothingSpeedKnobs[i]->setBounds(smoothingSpeedKnob);

    }
    
    useSideChainButton.setBounds(sectionBounds.getX(), sectionBounds.getCentreY(), 30,30);
    
}
//=====================================================================
void AnalysisEditor::positionMeters(const Rectangle<int>& sectionBounds)
{
    
    auto meterBounds = sectionBounds;
    for (auto meter : meters)
        meter->setBounds(meterBounds.removeFromLeft(100));
    
    for (auto matrix : detectorMatrices)
        matrix->setBounds(meterBounds);
}
//=====================================================================
void AnalysisEditor::positionMeterButtons(const Rectangle<int>& sectionBounds)
{

    auto meterIrOptionBounds = sectionBounds;

    for (int i = 0; i < NUM_DETECTORS; i++)
    {
        auto meterIrColumn = meterIrOptionBounds.removeFromLeft(100).reduced(5);
        setEnvelopeModeBoxes[i]->setBounds(meterIrColumn
                                               .removeFromTop(30)
                                               .reduced(3));

        triggerConditionComboBoxes[i]->setBounds(  meterIrColumn
                                                .removeFromTop(30)
                                                .reduced(3));
        
        releaseConditionComboBoxes[i]->setBounds(  meterIrColumn
                                                 .removeFromTop(30)
                                                 .reduced(3));

        loadIRButtons[i]->setBounds(  meterIrColumn
                                              .removeFromTop(30)
                                              .reduced(3));
    
    }

}
//=====================================================================
void AnalysisEditor::positionEditPanel(const Rectangle<int>& sectionBounds)
{
    auto editPanelBounds = sectionBounds;
    editPanelBounds = editPanelBounds.removeFromLeft(300);
    
    envelopeParameterPanel = editPanelBounds;
    
    setEnvelopeComboBox.setBounds(editPanelBounds.removeFromTop(20).withSize(80, 15).translated(110, 35));
    auto left = editPanelBounds.removeFromLeft(120).reduced(10);
    auto right = editPanelBounds.removeFromTop(sectionBounds.getHeight() - 45).reduced(5).translated(0, 35);
    
    auto envelopeAttackTimeBounds = left.removeFromTop(50);
    auto envelopeDecayTimeBounds = right.removeFromTop(50);
    auto envelopeSustainTimeBounds = envelopeAttackTimeBounds.translated(0,75);
    auto envelopeReleaseTimeBounds = envelopeDecayTimeBounds.translated(0, 75);
    auto envelopeLevelBounds = envelopeSustainTimeBounds.translated(0, 75);
    
    for (auto k : envelopeAttackTimeKnobs)
        k->setBounds(envelopeAttackTimeBounds);
    for (auto k : envelopeDecayTimeKnobs)
        k->setBounds(envelopeDecayTimeBounds);
    for (auto k : envelopeSustainTimeKnobs)
        k->setBounds(envelopeSustainTimeBounds);
    for (auto k : envelopeReleaseTimeKnobs)
        k->setBounds(envelopeReleaseTimeBounds);
    for (auto k : envelopeLevelKnobs)
        k->setBounds(envelopeLevelBounds);
    
}
//=====================================================================
void AnalysisEditor::positionMasterPanel(const Rectangle<int>& sectionBounds)
{
    auto b = sectionBounds;
    auto columnWidth = sectionBounds.getWidth() / 3;

    wetGainDBSlider.setBounds(b.removeFromLeft(columnWidth).reduced(columnWidth * 0.35f, 30));
    dryGainDBSlider.setBounds(b.removeFromLeft(columnWidth).reduced(columnWidth * 0.35f, 30));
    inputGainDBSlider.setBounds(b.removeFromLeft(columnWidth).reduced(columnWidth * 0.35f, 30));
}
//=====================================================================
void AnalysisEditor::sliderValueChanged(juce::Slider *slider)
{
    //WAVEFORM VIEWING
    if (attackTimeKnobs.contains(slider))
    {
        int index = getIndexOfItemInArray(attackTimeKnobs, slider);
        processor.detectors[index].rmsEnvelope.setAttackTimeMS(slider->getValue());
    }
    
    if (releaseTimeKnobs.contains(slider))
    {
        int index = getIndexOfItemInArray(releaseTimeKnobs, slider);
        processor.detectors[index].rmsEnvelope.setReleaseTimeMS(slider->getValue());
    }
    
    if (rmsKnobs.contains(slider))
    {
        int index = getIndexOfItemInArray(rmsKnobs, slider);
        processor.detectors[index].rmsEnvelope.setBufferSizeMS(slider->getValue());
    }
    
    if (smoothingSpeedKnobs.contains(slider))
    {
        int index = getIndexOfItemInArray(smoothingSpeedKnobs, slider);
        processor.detectors[index].smoothedValue.setDurationS(slider->getValue(), 1.f);
    }
    
//ENVELOPE
    if (envelopeAttackTimeKnobs.contains(slider))
    {
        int index = getIndexOfItemInArray(envelopeAttackTimeKnobs, slider);
        processor.convolutionEnvelopes[index].setAttackTime(slider->getValue());
    }
    
    if (envelopeDecayTimeKnobs.contains(slider))
    {
        
        int index = getIndexOfItemInArray(envelopeDecayTimeKnobs, slider);
        processor.convolutionEnvelopes[index].setDecayTime(slider->getValue());
    }
    
    if (envelopeSustainTimeKnobs.contains(slider))
    {
        int index = getIndexOfItemInArray(envelopeSustainTimeKnobs, slider);
        processor.convolutionEnvelopes[index].setReleaseTime(slider->getValue());
    }
    
    if (envelopeReleaseTimeKnobs.contains(slider))
    {
        int index = getIndexOfItemInArray(envelopeReleaseTimeKnobs, slider);
        processor.convolutionEnvelopes[index].setSustainLevel(jd::dbamp(slider->getValue()));
    }
    
    if (envelopeLevelKnobs.contains(slider))
    {
        int index = getIndexOfItemInArray(envelopeLevelKnobs, slider);
    
        processor.convolutionEnvelopes[index].mul = jd::dbamp(slider->getValue());
    }
    //WET
    if (slider == &wetGainDBSlider) {

        processor.wetGainDB.setTarget(jd::dbamp(slider->getValue()));
    }
    
    if (slider == &dryGainDBSlider) {
        processor.dryGainDB.setTarget(jd::dbamp(slider->getValue()));
    }
    if (slider == &inputGainDBSlider) {
        processor.inputGainDB.setTarget(jd::dbamp(slider->getValue()));
    }
    
}
//=====================================================================
void AnalysisEditor::buttonClicked(Button* changedButton)
{
    for (int meterIndex = 0; meterIndex < NUM_DETECTORS; meterIndex++)
        if (changedButton == loadIRButtons[meterIndex])
        {
            FileChooser irFileChooser ("ChooseIR");
            if(irFileChooser.browseForFileToOpen())
            {
                File irFile = irFileChooser.getResult();
                
                processor.convolvers[meterIndex]->loadIRFromFile(irFile);
            }
        }
    
    {int index = 0;
    for (auto meter : meters)
    {
        if( changedButton == &meter->invertRangeButton)
        {
            processor.shouldReverseEnabledRange[index] = changedButton->getToggleState();
            break;
        }
        index++;
    }}
    
    {int index = 0;
    for (auto meter : meters)
    {
        if( changedButton == &meter->enableButton)
        {
            processor.convolutionEnabled[index] = changedButton->getToggleState();
            break;
        }
        index++;
    }}

    if (changedButton == &useSideChainButton)
        processor.shouldUseSidechain = changedButton->getToggleState();
}
//=====================================================================
void AnalysisEditor::comboBoxChanged(juce::ComboBox *comboBox)
{
    //Waveform
    if (comboBox == &setDisplayAnnotation)
    {
        auto newScalingMode = (WaveformDisplay::ScalingMode)comboBox->getSelectedItemIndex();
            waveformDisplay.setMode(newScalingMode);
    }
    
    if (comboBox == &setActiveDetector)
    {
        int index = 0;
        
        activeWaveform = comboBox->getSelectedId() - 1;
        
        deselectAllDetectionSettings();
        
        for (auto w :waveformViewers) {
            if (activeWaveform == NUM_DETECTORS) {
                w->setIsActive(true);
                
            } else {

                bool indexIsActive = (index == (activeWaveform));
                
                if (activeWaveform == LEVEL)
                    waveformDisplay.setMode(WaveformDisplay::LOG_AMP);
                else if
                    (activeWaveform == PITCH ) waveformDisplay.setMode(WaveformDisplay::LOG_FREQ);
                else
                    waveformDisplay.setMode(WaveformDisplay::LIN);
                
                w->setIsActive(indexIsActive );
                
                attackTimeKnobs[index]->setVisible(indexIsActive);
                attackTimeKnobs[index]->setEnabled(indexIsActive);
                
                releaseTimeKnobs[index]->setVisible(indexIsActive);
                releaseTimeKnobs[index]->setEnabled(indexIsActive);
                
                rmsKnobs[index]->setVisible(indexIsActive);
                rmsKnobs[index]->setEnabled(indexIsActive);
                
                smoothingSpeedKnobs[index]->setVisible(indexIsActive);
                smoothingSpeedKnobs[index]->setEnabled(indexIsActive);
                
                samplesPerPixelKnobs[index]->setVisible(indexIsActive);
                samplesPerPixelKnobs[index]->setEnabled(indexIsActive);
            }
            index++;
        }
    }
    
    //METER
    if (triggerConditionComboBoxes.contains(comboBox))
    {
        int index = getIndexOfItemInArray(triggerConditionComboBoxes, comboBox);
        
        auto gateCode = static_cast<GateCode>(comboBox->getSelectedItemIndex() );
        
        if (gateCode == GateCode::onExitFromBelow)
            processor.triggerConditions[index] = {1,0,0,0};
        
        if (gateCode == GateCode::onEntryToBelow)
            processor.triggerConditions[index] = {0,1,0,0};
        
        if (gateCode == GateCode::onExitToAbove)
            processor.triggerConditions[index] = {0,0,1,0};
        
        if (gateCode == GateCode::onEntryFromAbove)
            processor.triggerConditions[index] = {0,0,0,1};
        
        if (gateCode == GateCode::onEntryToRange)
            processor.triggerConditions[index] = {1,0,0,1};
        
        if (gateCode == GateCode::onExitFromRange)
            processor.triggerConditions[index] = {0,1,1,0};
   
    }
    
    if (releaseConditionComboBoxes.contains(comboBox))
    {
        int index = getIndexOfItemInArray(releaseConditionComboBoxes, comboBox);
        
        auto gateCode = static_cast<GateCode>(comboBox->getSelectedItemIndex() );
        
        if (gateCode == GateCode::onExitFromBelow)
            processor.releaseConditions[index] = {1,0,0,0};
        
        if (gateCode == GateCode::onEntryToBelow)
            processor.releaseConditions[index] = {0,1,0,0};
        
        if (gateCode == GateCode::onExitToAbove)
            processor.releaseConditions[index] = {0,0,1,0};
        
        if (gateCode == GateCode::onEntryFromAbove)
            processor.releaseConditions[index] = {0,0,0,1};
        
        if (gateCode == GateCode::onEntryToRange)
            processor.releaseConditions[index] = {1,0,0,1};
        
        if (gateCode == GateCode::onExitFromRange)
            processor.releaseConditions[index] = {0,1,1,0};
    }
    
    if (setEnvelopeModeBoxes.contains(comboBox)) {
        int envelopeMode = comboBox->getSelectedItemIndex();
        int index = getIndexOfItemInArray(setEnvelopeModeBoxes, comboBox);
        
        if (envelopeMode == EnvelopeMode::oneShot) {
            processor.convolutionEnvelopes[index].setSustainNodes({});
        } else {
            processor.convolutionEnvelopes[index].setSustainNodes({2});
        }
    }
    
    if (&matrixDetectorBox == comboBox)
    {
        int index = comboBox->getSelectedItemIndex();
        
        for(auto matrix : detectorMatrices) {
            matrix->setVisible(false);
            matrix->setEnabled(false);
        }
        detectorMatrices[index]->setVisible(true);
        detectorMatrices[index]->setEnabled(true);
    }
    

    if (comboBox == &setEnvelopeComboBox)
    {

        int index = comboBox->getSelectedItemIndex();

        deselectAllEnvelopes();
        for(auto matrix : detectorMatrices) {
            matrix->setVisible(false);
            matrix->setEnabled(false);
        }
        
        detectorMatrices[index]->setVisible(true);
        detectorMatrices[index]->setEnabled(true);

        envelopeAttackTimeKnobs[index]->setVisible(true);
        envelopeAttackTimeKnobs[index]->setEnabled(true);

        envelopeDecayTimeKnobs[index]->setVisible(true);
        envelopeDecayTimeKnobs[index]->setEnabled(true);

        envelopeReleaseTimeKnobs[index]->setVisible(true);
        envelopeReleaseTimeKnobs[index]->setEnabled(true);

        envelopeSustainTimeKnobs[index]->setVisible(true);
        envelopeSustainTimeKnobs[index]->setEnabled(true);

        envelopeLevelKnobs[index]->setVisible(true);
        envelopeLevelKnobs[index]->setEnabled(true);
        currentEnvelopeIndex = index;
    
        
    
    }
    
}
//=====================================================================
void AnalysisEditor::deselectAllEnvelopes()
{
    for (int i = 0; i < NUM_DETECTORS; i++)
    {
        envelopeAttackTimeKnobs[i]->setVisible(false);
        envelopeAttackTimeKnobs[i]->setEnabled(false);
        
        envelopeDecayTimeKnobs[i]->setVisible(false);
        envelopeDecayTimeKnobs[i]->setEnabled(false);
        
        envelopeReleaseTimeKnobs[i]->setVisible(false);
        envelopeReleaseTimeKnobs[i]->setEnabled(false);
        
        envelopeSustainTimeKnobs[i]->setVisible(false);
        envelopeSustainTimeKnobs[i]->setEnabled(false);
        
        envelopeLevelKnobs[i]->setVisible(false);
        envelopeLevelKnobs[i]->setEnabled(false);

    }
}
//=====================================================================
void AnalysisEditor::deselectAllDetectionSettings()
{
    for (int i = 0; i < NUM_DETECTORS; i++)
    {
        attackTimeKnobs[i]->setVisible(false);
        attackTimeKnobs[i]->setEnabled(false);
        
        releaseTimeKnobs[i]->setVisible(false);
        releaseTimeKnobs[i]->setEnabled(false);
        
        rmsKnobs[i]->setVisible(false);
        rmsKnobs[i]->setEnabled(false);
        
        smoothingSpeedKnobs[i]->setVisible(false);
        smoothingSpeedKnobs[i]->setEnabled(false);
        
        samplesPerPixelKnobs[i]->setVisible(false);
        samplesPerPixelKnobs[i]->setEnabled(false);
        
    }

}

