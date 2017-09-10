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
    setActiveDetector.addItemList({"level", "pitch", "pitch confidence", "pitch_salnce", "inharmoncity" }, 1);
    setActiveDetector.addListener(this);
    
    //ENV ATTACK
    addAndMakeVisible(attackTimeKnob);
    attackTimeKnob.addListener(this);
    attackTimeKnob.setSliderStyle(Slider::RotaryVerticalDrag);
    attackTimeKnob.setTextBoxStyle(Slider::TextBoxBelow, false, 40, 20);
    attackTimeKnob.setRange(0.001, 1.0);
    
    //RMS WINDOW SIZE
    addAndMakeVisible(releaseTimeKnob);
    releaseTimeKnob.addListener(this);
    releaseTimeKnob.setSliderStyle(Slider::RotaryVerticalDrag);
    releaseTimeKnob.setTextBoxStyle(Slider::TextBoxBelow, false, 40, 20);
    releaseTimeKnob.setRange(0.001, 1.0);
    
    //RMS WINDOW SIZE
    addAndMakeVisible(rmsKnob);
    rmsKnob.addListener(this);
    rmsKnob.setSliderStyle(Slider::RotaryVerticalDrag);
    rmsKnob.setTextBoxStyle(Slider::TextBoxBelow, false, 40, 20);
    rmsKnob.setRange(1, 1000);
    
    //Smoothing Speed
    addAndMakeVisible(smoothingSpeedKnob);
    smoothingSpeedKnob.addListener(this);
    smoothingSpeedKnob.setSliderStyle(Slider::RotaryVerticalDrag);
    smoothingSpeedKnob.setTextBoxStyle(Slider::TextBoxBelow, false, 40, 20);
    smoothingSpeedKnob.setRange(0.01, 2.);
    
    //Smoothing Speed
    addAndMakeVisible(samplesPerPixelKnob);
    samplesPerPixelKnob.addListener(this);
    samplesPerPixelKnob.setSliderStyle(Slider::RotaryVerticalDrag);
    samplesPerPixelKnob.setTextBoxStyle(Slider::TextBoxBelow, false, 40, 20);
    samplesPerPixelKnob.setRange(32, 512);
    
    //ANALYSIS METERS
    
    for (int i = 0; i < NUM_DETECTORS; i++)
    {
        auto& d = processor.detectors[i];
        auto m = new AnalysisMeter(d);
        addAndMakeVisible(m);
        m->thresholdSlider.setRange(d.limits.lower, d.limits.upper);
    
        meters.add(m);

        
        meterEnvelopeModes.set(i, oneShot);
        
        auto meterIRTriggerModeBox = new ComboBox();
        addAndMakeVisible(meterIRTriggerModeBox);
        meterIRTriggerModeBox->addItemList({"exitFromBelow",
                                        "entryToBelow",
                                        "entryToAbove",
                                        "exitFromAbove" }, 1);
        
        meterIRTriggerModeBoxes.add(meterIRTriggerModeBox);
        
        auto meterIRSelectionBox = new ComboBox();
        addAndMakeVisible(meterIRSelectionBox);
        meterIRSelectionBox->setText("---");
        meterIRSelectionBox->addListener(this);
        meterIRSelectionBoxes.add(meterIRSelectionBox);
        
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
    }
    
    
    auto meterNames = {"amp", "pitch", "pitch confidence", "pitch salience", "inharmonicity"};
    {int i = 0;
    for (auto meterName : meterNames) {
        meters[i]->setName(meterName);
    }
    }
        
    meters[PITCH]->thresholdSlider.setSkewFactor(0.125);
    meters[LEVEL]->thresholdSlider.setSkewFactor(0.5, true);
    
    
    
    addAndMakeVisible(masterFader);
    masterFader.setRange(0., 1.);
    masterFader.setSliderStyle(Slider::SliderStyle::LinearBar);
   
}
//=====================================================================
void AnalysisEditor::paint(Graphics& g)
{
    auto r = getLocalBounds();
    
    g.fillAll(Colours::lightgrey);
    
    g.setColour(Colours::black);
    g.fillRect(waveformDisplay.getBounds());
    
    auto top = r.removeFromTop(250);
    g.drawRect(top);
    
    auto waveformBounds = top.removeFromLeft(600);
    g.drawRect(waveformBounds);
    
    auto analysisSettingBounds = top;
    g.drawRect(analysisSettingBounds);
    
    auto middle = r.removeFromTop(250);
    g.drawRect(middle);
    
    auto bottom = r.removeFromTop(250);
    g.drawRect(bottom);
    
    auto masterControlBounds = bottom.removeFromRight(200);
    g.drawRect(masterControlBounds);
    
}
//=====================================================================
void AnalysisEditor::resized()
{
    auto r = getLocalBounds();
    auto top = r.removeFromTop(250);
    
    auto detectorDrawingBounds = top.removeFromLeft(600);
    positionDetectorDrawing(detectorDrawingBounds);
    
    auto analysisSettingBounds = top;
    positionDetectorSettings(analysisSettingBounds);
    
    auto middle = r.removeFromTop(250);
    auto meterBounds = middle.removeFromLeft(600);
    positionMeters(meterBounds);
    
    auto bottom = r.removeFromTop(250);
    positionMeterButtons(bottom.removeFromLeft(600));
    
    auto masterControlBounds = bottom;
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
    
    setDisplayAnnotation.setBounds(b.removeFromTop(20)
                                   .removeFromLeft(75));
    
    auto left = b.removeFromLeft(90);
    attackTimeKnob.setBounds(left.removeFromTop(75).reduced(5));
    rmsKnob.setBounds(left.removeFromTop(75).reduced(5));
    
    auto right = b;
    releaseTimeKnob.setBounds(right.removeFromTop(75).reduced(5));
    smoothingSpeedKnob.setBounds(right.removeFromTop(75).reduced(5));
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

    for (int i = 0; i < meterIRSelectionBoxes.size(); i++)
    {
        auto meterIrColumn = meterIrOptionBounds.removeFromLeft(100).reduced(5);

        meterIRSelectionBoxes[i]->setBounds( meterIrColumn
                                       .removeFromTop(30)
                                       .reduced(3));

        setEnvelopeModeBoxes[i]->setBounds(meterIrColumn
                                               .removeFromTop(30)
                                               .reduced(3));

        meterIRTriggerModeBoxes[i]->setBounds(  meterIrColumn
                                                .removeFromTop(30)
                                                .reduced(3));

        loadIRButtons[i]->setBounds(  meterIrColumn
                                              .removeFromTop(30)
                                              .reduced(3));
    }
}
//=====================================================================
void AnalysisEditor::positionMasterPanel(const Rectangle<int>& sectionBounds())
{}
//=====================================================================
void AnalysisEditor::sliderValueChanged(juce::Slider *slider)
{
    if (slider == &rmsKnob)
        processor.detectors[activeWaveform].rmsEnvelope.setBufferSizeMS(slider->getValue());
    if (slider == &attackTimeKnob)
        processor.detectors[activeWaveform].rmsEnvelope.setAttackTimeMS(slider->getValue());
    if (slider == &releaseTimeKnob)
        processor.detectors[activeWaveform].rmsEnvelope.setReleaseTimeMS(slider->getValue());
    if (slider == &smoothingSpeedKnob)
        processor.detectors[activeWaveform].smoothedValue.setDurationS(slider->getValue(), 1.f);
    if (slider == &samplesPerPixelKnob)
        waveformViewers[activeWaveform]->setSamplesToAverage(slider->getValue());
}
//=====================================================================
void AnalysisEditor::buttonClicked(Button* changedButton)
{
    for (int meterIndex = 0; meterIndex < NUM_DETECTORS; meterIndex++)
    {
        if (changedButton == loadIRButtons[meterIndex])
        {
            FileChooser irFileChooser ("ChooseIR");
            if(irFileChooser.browseForFileToOpen())
            {
                File irFile = irFileChooser.getResult();
                
                processor.convolvers[meterIndex]->loadIRFromFile(irFile);
            }
            
        }
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
        int j = 0;
        activeWaveform = comboBox->getSelectedId() - 1;
        for (auto w :waveformViewers)
            w->setIsActive( j++ == (activeWaveform) );
    }
    
    //METER
    for (auto meterIrTriggerModeBox : meterIRTriggerModeBoxes)
        if (comboBox == meterIrTriggerModeBox)
        {
            int itemID = comboBox->getSelectedItemIndex();
            setMeterTriggerMode(itemID);
        }
    
    
    int i = 0;
    for (auto meterIrSelectionModeBox : meterIRSelectionBoxes) {
        if (comboBox == meterIrSelectionModeBox)
        {
            int newEnvelopeMode = comboBox->getSelectedItemIndex();
            meterEnvelopeModes.getReference(i) = static_cast<EnvelopeMode>(newEnvelopeMode);
        }
        i++;
    }
    
    for (auto setEnvelopeModeBox : setEnvelopeModeBoxes)
        if (comboBox == setEnvelopeModeBox)
        {
            int itemID = comboBox->getSelectedItemIndex();

        }
}
//=====================================================================
void AnalysisEditor::setMeterTriggerMode(int itemIndex)
{
//    processor.meterTriggerConditions.getReference(itemIndex) = static_cast<TriggerCondition>(itemIndex - 1);
}
