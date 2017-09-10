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
    
    //ANALYSIS METERS
    
    for (int i = 0; i < NUM_DETECTORS; i++)
    {
        auto& d = processor.detectors[i];
        auto m = new AnalysisMeter(d);
        addAndMakeVisible(m);
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
    
        
        auto meterIRTriggerModeBox = new ComboBox();
        addAndMakeVisible(meterIRTriggerModeBox);
        meterIRTriggerModeBox->addItemList({"exitFromBelow",
                                        "entryToBelow",
                                        "entryToAbove",
                                        "exitFromAbove" }, 1);
        
        meterIRTriggerModeBoxes.add(meterIRTriggerModeBox);
        
        auto newShouldInvertEnabledRangeComboBox = new ComboBox();
        addAndMakeVisible(newShouldInvertEnabledRangeComboBox);
        newShouldInvertEnabledRangeComboBox->addItemList({"inside","outside"}, 1);
        newShouldInvertEnabledRangeComboBox->setSelectedId(1);
        newShouldInvertEnabledRangeComboBox->addListener(this);
        shouldInvertEnabledbRangeComboBoxes.add(newShouldInvertEnabledRangeComboBox);
        
        auto setEnvelopeModeBox = new ComboBox();
        addAndMakeVisible(setEnvelopeModeBox);
        setEnvelopeModeBox->addListener(this);
        setEnvelopeModeBox->addItemList({"oneshot", "sustain"}, 1);
        setEnvelopeModeBox->setSelectedId(1);
        setEnvelopeModeBoxes.add(setEnvelopeModeBox);
        
        auto newLoadIRButton = new TextButton();
        newLoadIRButton->setButtonText("load ir");
        newLoadIRButton->addListener(this);
        addAndMakeVisible(newLoadIRButton);
        loadIRButtons.set( i, newLoadIRButton);
        
        auto newEditDetectorButton = new TextButton();
        newEditDetectorButton->setButtonText("edit dtc");
        newEditDetectorButton->addListener(this);
        addAndMakeVisible(newEditDetectorButton);
        editDetectorButtons.set( i, newEditDetectorButton);
        
  
        auto newEnvelopeAttackSlider = new Slider(
                                                    Slider::SliderStyle::RotaryHorizontalVerticalDrag,
                                                    Slider::TextBoxBelow);
        newEnvelopeAttackSlider->setRange(0.1, 20.);
        addChildComponent(newEnvelopeAttackSlider);
        newEnvelopeAttackSlider->addListener(this);
        envelopeAttackTimeKnobs.add(newEnvelopeAttackSlider);
        
        auto newEnvelopeDecaySlider = new Slider(
                                                  Slider::SliderStyle::RotaryHorizontalVerticalDrag,
                                                  Slider::TextBoxBelow);
        newEnvelopeDecaySlider->setRange(0.1, 20.);
        addChildComponent(newEnvelopeDecaySlider);
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
        newEnvelopeLevelSlider->setRange(-80,6);
        addChildComponent(newEnvelopeLevelSlider);
        newEnvelopeLevelSlider->addListener(this);
        envelopeLevelKnobs.add(newEnvelopeLevelSlider);
        
    }
    
    
    {
        int i = 0;
        for (auto meterName : {"amp", "pitch", "pitch confidence", "pitch salience", "inharmonicity"})
            meters[i++]->setName(meterName);
    }
        
    meters[PITCH]->thresholdSlider.setSkewFactor(0.125);
    meters[LEVEL]->thresholdSlider.setSkewFactor(0.5, true);
    
    
    
    addAndMakeVisible(dryGainDBSlider);
    dryGainDBSlider.setRange(-120., 12.);
    dryGainDBSlider.setTextBoxStyle(Slider::TextBoxLeft, false, 30, 15);
    dryGainDBSlider.setSliderStyle(Slider::SliderStyle::LinearBarVertical);
    
    addAndMakeVisible(wetGainDBSlider);
    wetGainDBSlider.setRange(-120.f, 12.f);
    wetGainDBSlider.setTextBoxStyle(Slider::TextBoxLeft, false, 30, 15);
    wetGainDBSlider.setSliderStyle(Slider::SliderStyle::LinearBarVertical);
   
}
//=====================================================================
void AnalysisEditor::paint(Graphics& g)
{
    auto r = getLocalBounds();
    auto top = r.removeFromTop(250);
    
    g.fillAll(Colours::lightgrey);
    
    g.setColour(Colours::black);
    g.drawRect(top);
    
    
    
    g.fillRect(detectorDrawingBounds);;
    
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
    
    auto masterControlBounds = bottom;
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
    auto rmsKnobBounds = left.removeFromTop(75).reduced(5);
    
    auto right = b;
    auto releaseTimeKnob = right.removeFromTop(75).reduced(5);
    auto smoothingSpeedKnob = right.removeFromTop(75).reduced(5);
    
    for (int i = 0; i < NUM_DETECTORS; i++)
    {
        attackTimeKnobs[i]->setBounds(attackTimeKnobBounds);
        releaseTimeKnobs[i]->setBounds(releaseTimeKnob);
        rmsKnobs[i]->setBounds(rmsKnobBounds);
        smoothingSpeedKnobs[i]->setBounds(smoothingSpeedKnob);

    }
}
//=====================================================================
void AnalysisEditor::positionMeters(const Rectangle<int>& sectionBounds)
{
    
    auto meterBounds = sectionBounds;
    for (auto meter : meters)
        meter->setBounds(meterBounds.removeFromLeft(100));
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

        meterIRTriggerModeBoxes[i]->setBounds(  meterIrColumn
                                                .removeFromTop(30)
                                                .reduced(3));

        loadIRButtons[i]->setBounds(  meterIrColumn
                                              .removeFromTop(30)
                                              .reduced(3));
        editDetectorButtons[i]->setBounds(  meterIrColumn
                                    .removeFromTop(30)
                                    .reduced(3));
    }
}
//=====================================================================
void AnalysisEditor::positionEditPanel(const Rectangle<int>& sectionBounds)
{
    auto editPanelBounds = sectionBounds;
    editPanelBounds = editPanelBounds.removeFromLeft(300);
    
    auto left = editPanelBounds.removeFromLeft(100).reduced(5);
    auto right = editPanelBounds.removeFromTop(sectionBounds.getHeight() - 25).reduced(5).translated(0, 25);
    
    auto envelopeAttackTimeBounds = left.removeFromTop(50);
    auto envelopeDecayTimeBounds = right.removeFromTop(50);
    auto envelopeSustainTimeBounds = left.removeFromTop(50);
    auto envelopeReleaseTimeBounds = right.removeFromTop(50);
    auto envelopeLevelBounds = left.removeFromTop(50);
    
    
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
    
    auto left = b.removeFromLeft(100);
    wetGainDBSlider.setBounds(left.reduced(40, 20));
    auto right = b;
    dryGainDBSlider.setBounds(right.reduced(40, 20));
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
        std::cout << index << std::endl;
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
        processor.convolutionEnvelopes[index].mul = slider->getValue();
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
    
    
    if (editDetectorButtons.contains(dynamic_cast<TextButton*>(changedButton)))
    {
        
        int index = getIndexOfItemInArray(editDetectorButtons, changedButton);
        
        deselectAllEnvelopes();
            
        if (index == currentEnvelopeIndex) {
            currentEnvelopeIndex = -1;
        } else {
        
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
    
    
    if (shouldInvertEnabledbRangeButtons.contains(dynamic_cast<ToggleButton*>(changedButton)))
    {
        int index = getIndexOfItemInArray(shouldInvertEnabledbRangeButtons, changedButton);
        
        processor.shouldReverseEnabledRange[index] = changedButton->getToggleState();
    }
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
    for (auto meterIrTriggerModeBox : meterIRTriggerModeBoxes)
        if (comboBox == meterIrTriggerModeBox)
        {
            int itemID = comboBox->getSelectedItemIndex();
            setMeterTriggerMode(itemID);
        }

    if (setEnvelopeModeBoxes.contains(comboBox)) {
        int envelopeMode = comboBox->getSelectedItemIndex();
        int index = getIndexOfItemInArray(setEnvelopeModeBoxes, comboBox);
        processor.envelopeModes[index] = static_cast<EnvelopeMode>(envelopeMode);
    }
}
//=====================================================================
void AnalysisEditor::setMeterTriggerMode(int itemIndex)
{
//    processor.meterTriggerConditions.getReference(itemIndex) = static_cast<TriggerCondition>(itemIndex - 1);
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

