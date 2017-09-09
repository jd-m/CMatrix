#include "AnalysisEditor.hpp"


AnalysisEditor::AnalysisEditor(Jd_cmatrixAudioProcessor& p, ButtonGrid& stepSequencerSource):
processor(p) , stepSequencer(stepSequencerSource)
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
    
    addAndMakeVisible(setModeBox);
    setModeBox.addListener(this);
    setModeBox.addItemList({"Linear Normalised", "Log Amp (dB)", "Log Freq (Hz)"}, 1);
    
    addAndMakeVisible(setActiveDetector);
    setActiveDetector.addItemList({"level", "pitch", "pitch confidence", "pitch_salnce", "inharmoncity" }, 1);
    setActiveDetector.addListener(this);
    
    //ENV ATTACK
    addAndMakeVisible(attackTimeKnob);
    attackTimeKnob.addListener(this);
    attackTimeKnob.setSliderStyle(Slider::RotaryVerticalDrag);
    attackTimeKnob.setTextBoxStyle(Slider::TextBoxBelow, false, 30, 10);
    attackTimeKnob.setRange(0.001, 1.0);
    //RMS WINDOW SIZE
    addAndMakeVisible(releaseTimeKnob);
    releaseTimeKnob.addListener(this);
    releaseTimeKnob.setSliderStyle(Slider::RotaryVerticalDrag);
    releaseTimeKnob.setTextBoxStyle(Slider::TextBoxBelow, false, 30, 10);
    releaseTimeKnob.setRange(0.001, 1.0);
    //RMS WINDOW SIZE
    addAndMakeVisible(rmsKnob);
    rmsKnob.addListener(this);
    rmsKnob.setSliderStyle(Slider::RotaryVerticalDrag);
    rmsKnob.setTextBoxStyle(Slider::TextBoxBelow, false, 30, 10);
    rmsKnob.setRange(5, 1000);
    //Smoothing Speed
    addAndMakeVisible(smoothingSpeedKnob);
    smoothingSpeedKnob.addListener(this);
    smoothingSpeedKnob.setSliderStyle(Slider::RotaryVerticalDrag);
    smoothingSpeedKnob.setTextBoxStyle(Slider::TextBoxBelow, false, 30, 10);
    smoothingSpeedKnob.setRange(0.01, 2.);
    //Smoothing Speed
    addAndMakeVisible(samplesPerPixelKnob);
    samplesPerPixelKnob.addListener(this);
    samplesPerPixelKnob.setSliderStyle(Slider::RotaryVerticalDrag);
    samplesPerPixelKnob.setTextBoxStyle(Slider::TextBoxBelow, false, 30, 10);
    samplesPerPixelKnob.setRange(32, 512);
    
    //ANALYSIS METERS
    for (int i = 0; i < NUM_DETECTORS; i++)
    {
        auto& d = processor.detectors[i];
        auto m = new AnalysisMeter(d);
        addAndMakeVisible(m);
        m->thresholdSlider.setRange(d.limits.lower, d.limits.upper);

        meters.add(m);
    }
    meters[PITCH]->thresholdSlider.setSkewFactor(0.125);
    meters[LEVEL]->thresholdSlider.setSkewFactor(0.5, true);
    
    //EnvDetectorParams
    currentState.setProperty("rmsSizeValue", 10.f, nullptr);
    currentState.setProperty("attackTimeValue", 10.f, nullptr);
    currentState.setProperty("releaseTime", 10.f, nullptr);
    currentState.setProperty("scalingMode", WaveformDisplay::LIN, nullptr);
    
    //IR SELECTION
    addAndMakeVisible(irSequenceSelectionBox);
    irSequenceSelectionBox.addListener(this);
    irSequenceSelectionBox.setText("---");
    
    addAndMakeVisible(irTriggerModeBox);
    irTriggerModeBox.addListener(this);
    irTriggerModeBox.addItemList({  "exitFromBelow",
                                    "entryToBelow",
                                    "entryToAbove",
                                    "exitFromAbove" }, 1);
    irTriggerModeBox.setSelectedItemIndex(1);
    
    stepSequencer.sequencesComboBox.addListener(this);
    
    
}
//=====================================================================
void AnalysisEditor::paint(Graphics& g)
{
    auto r = getLocalBounds();
    
    g.fillAll(Colours::lightgrey);
    
    g.setColour(Colours::black);
    g.fillRect(waveformDisplay.getBounds());
    g.drawRect(r.removeFromTop(250));
    g.drawRect(r.removeFromTop(250));
    g.drawRect(r.removeFromTop(250));

    if (processor.detectors.allEnabledDetectorsWithinRange())
        g.setColour(Colours::darkorange);
    else
        g.setColour(Colours::darkgrey);
    g.fillRect(700,400,50,50);
}
//=======================================================================
void AnalysisEditor::resized()
{
    auto r = getLocalBounds();

    auto top = r.removeFromTop(250);
    auto waveformViewerBounds = top.removeFromLeft(600);
    for (int i = 0; i  < NUM_DETECTORS; i++)
    {
        waveformViewers[i]->setBounds(waveformViewerBounds);
    }
    auto waveformViewSettingsBounds = top;
    
    auto column1 = waveformViewSettingsBounds.removeFromLeft(50);
    setModeBox.setBounds(column1.removeFromTop(50).withSize(50, 15).translated(0, 0));
    rmsKnob.setBounds(column1.removeFromTop(50).withSize(40, 40).translated(0, 0));
    attackTimeKnob.setBounds(column1.removeFromTop(50).withSize(40, 40).translated(0, 0));
    releaseTimeKnob.setBounds(column1.removeFromTop(50).withSize(40, 40).translated(0, 0));
    auto column2 = waveformViewSettingsBounds.removeFromLeft(50);
    setActiveDetector.setBounds(column2.removeFromTop(50).withSize(50, 15));

    smoothingSpeedKnob.setBounds(column2.removeFromTop(60));
    waveformDisplay.setBounds(waveformViewerBounds);
    
    auto mid = r.removeFromTop(250);
    auto metersBounds = mid.removeFromLeft(600).reduced(10);
    
    for (auto m : meters)
        m->setBounds(metersBounds.removeFromLeft(100));
    
    auto irSelectionBounds = mid;
    irSequenceSelectionBox.setBounds(irSelectionBounds.removeFromRight(120).removeFromTop(40).reduced(10));
    irTriggerModeBox.setBounds(irSelectionBounds.removeFromTop(60).reduced(10));
    
}
//=====================================================================
void AnalysisEditor::sliderValueChanged(juce::Slider *slider)
{
    if (slider == &rmsKnob)
    {
        processor.detectors[activeWaveform].rmsEnvelope.setBufferSizeMS(slider->getValue());
    }
    if (slider == &attackTimeKnob)
    {
        processor.detectors[activeWaveform].rmsEnvelope.setAttackTimeMS(slider->getValue());
    }
    if (slider == &releaseTimeKnob)
    {
        processor.detectors[activeWaveform].rmsEnvelope.setReleaseTimeMS(slider->getValue());
    }
    if (slider == &smoothingSpeedKnob)
    {
        processor.detectors[activeWaveform].smoothedValue.setDurationS(slider->getValue(), 1.f);
    }
    if (slider == &samplesPerPixelKnob)
    {
        waveformViewers[activeWaveform]->setSamplesToAverage(slider->getValue());
    }
}
//=======================================================================
void AnalysisEditor::buttonClicked(Button* button)
{

}
//=======================================================================
void AnalysisEditor::comboBoxChanged(juce::ComboBox *comboBox)
{
    if (comboBox == &setModeBox)
    {
        auto newScalingMode = (WaveformDisplay::ScalingMode)comboBox->getSelectedItemIndex();
        waveformDisplay.setMode(newScalingMode);
    }
    if (comboBox == &setActiveDetector)
    {
        int j = 0;
        activeWaveform = comboBox->getSelectedId() - 1;
        for (auto w :waveformViewers)
            w->setIsActive( j++ == (activeWaveform) );
    }

    
    if (comboBox == &stepSequencer.sequencesComboBox)
    {
        irSequenceSelectionBox.clear();
        for (int i = 0; i < comboBox->getNumItems(); i++)
        {
            int itemID = comboBox->getItemId(i);
            irSequenceSelectionBox.addItem(std::forward<String>(comboBox->getItemText(i)), itemID);
        }
    }
    
    //select sequence
    if (comboBox == &irSequenceSelectionBox)
    {

        String name = comboBox->getItemText(comboBox->getSelectedItemIndex());
        sequencer.setIRSequence(name);
    }
    
}
//=======================================================================
