//
//  IRWaveformEditor.cpp
//  jd_CMatrix
//
//  Created by Jaiden Muschett on 08/09/2017.
//
//

#include "IRWaveformEditor.hpp"
//===================================================================
/* Pattern Element Info */
//===================================================================
IRState::IRState(int sourceSamplesPerThumbnailSample,
                 AudioFormatManager* formatManagerToUse,
                 AudioThumbnailCache* cacheToUse):
samplesPerThumbnailSample(sourceSamplesPerThumbnailSample),
formatManager(formatManagerToUse),
thumbnailCache(cacheToUse),
thumbnail(new AudioThumbnail(sourceSamplesPerThumbnailSample,
                             *formatManagerToUse,
                             *cacheToUse))
{
    
}
//===================================================================
void IRState::drawFullThumbnail(juce::Graphics &g,
                                const Rectangle<int> &area,
                                double zoomFactor)
{
    g.setColour(Colours::black);
    thumbnail->drawChannels(g, area, 0., totalDuration(), zoomFactor);
}
//===================================================================
void IRState::drawThumbnailSection(juce::Graphics &g,
                                   const Rectangle<int> &area,
                                   double zoomFactor,
                                   bool isSelected,
                                   Colour selectionColour)
{
    if (isSelected)
        g.setColour(selectionColour);
    else
        g.setColour(Colours::black);
    
    thumbnail->drawChannels(g, area, startTime, startTime + duration, zoomFactor);
}
//===================================================================
void IRState::copyStateFrom(const IRState& target)
{
    formatManager = target.formatManager;
    thumbnailCache = target.thumbnailCache;
    samplesPerThumbnailSample = target.samplesPerThumbnailSample;
    startTime = target.startTime;
    duration = target.duration;
    loadedFile = target.loadedFile;
    reader = formatManager->createReaderFor(target.loadedFile);
    thumbnail = new AudioThumbnail(target.samplesPerThumbnailSample, *formatManager, *thumbnailCache);
    thumbnail->setSource(new FileInputSource (loadedFile));
    env = target.env;
    uid = target.uid;
    name = target.name;
}
//===================================================================
/* IR Waveform Editor */
//===================================================================
IRWaveformEditor::IRWaveformEditor (IRState &sourceIRState):
currentIRState(sourceIRState)
{
    //LoadButton
    addAndMakeVisible(m_loadButton);//loadButton
    m_loadButton.setButtonText("load file.");
    m_loadButton.addListener(this);
    //SelectionSlider
    addAndMakeVisible(selectionSlider);
    selectionSlider.setSliderStyle(Slider::SliderStyle::TwoValueHorizontal);
    selectionSlider.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
    selectionSlider.addListener(this);
    //Pattern Element Info
    currentIRState.thumbnail->addChangeListener(this);
    //labels
    addAndMakeVisible(selectionStartLabel);
    addAndMakeVisible(selectionEndLabel);
    addAndMakeVisible(startTimeLabel);
    addAndMakeVisible(endTimeLabel);
    Font labelFont (12);
    selectionStartLabel.setFont(labelFont);
    selectionEndLabel.setFont(labelFont);
    startTimeLabel.setFont(labelFont);
    endTimeLabel.setFont(labelFont);
    //Debug
    addAndMakeVisible(log);
}
//===================================================================
void IRWaveformEditor::paint(juce::Graphics &g)
{
    g.drawRect(m_optionBounds);
    g.drawRect(m_waveformTopBounds);
    g.drawRect(m_waveformInfoBounds);
    currentIRState.drawFullThumbnail(g, m_waveformTopBounds, 1.);
    //Overlay
    g.setColour(Colours::darkorange);
    g.drawRoundedRectangle(m_overlayBounds.toFloat(), 10, 2);
    //grey out not selected
    Colour overlayColour = Colours::lightslategrey.withAlpha((float)0.8);
    g.setColour(overlayColour);
    
    auto outsideBoundsL = m_waveformTopBounds;
    outsideBoundsL.setRight(m_overlayBounds.getX());
    g.fillRect(outsideBoundsL);
    auto outsideBoundsR = Rectangle<int>(m_overlayBounds.getRight(),
                                         m_overlayBounds.getY(),
                                         m_waveformTopBounds.getWidth() - m_overlayBounds.getRight(),
                                         m_waveformTopBounds.getHeight());
    g.fillRect(outsideBoundsR);
    
}
//===================================================================
void IRWaveformEditor::resized()
{
    auto r = getLocalBounds();
    m_optionBounds = r.removeFromRight(200);
    m_waveformTopBounds = r.removeFromTop(200);
    m_waveformInfoBounds = r;
    
    m_loadButton.setTopLeftPosition(m_optionBounds.getTopLeft() + Point<int>(50, 25));
    m_loadButton.setSize(80, 40);
    
    auto logBounds = m_optionBounds;
    log.setBounds(logBounds.removeFromBottom(100));
    
    auto infoBounds = m_waveformInfoBounds;
    selectionSlider.setBounds(infoBounds.removeFromTop(50));
    
    startTimeLabel.setBounds(infoBounds.getX(), infoBounds.getY() , 80, 50);
    endTimeLabel.setBounds(infoBounds.getRight() - 80, infoBounds.getY(), 80, 50);
}
//===================================================================
void IRWaveformEditor::buttonClicked(juce::Button *button)
{
    if (button == &m_loadButton)
    {
        FileChooser chooser ("Select a Wave file to play...",
                             File::nonexistent,
                             "*.wav");
        if (chooser.browseForFileToOpen())
        {
            File file (chooser.getResult());
            AudioFormatManager formatManager;
            formatManager.registerBasicFormats();
            if (file.exists()){
                currentIRState.reader =  (formatManager.createReaderFor(file));
                currentIRState.loadedFile = file;
            }
            if (currentIRState.reader != nullptr)
            {
                currentIRState.thumbnail->setSource(new FileInputSource (file));
                selectionSlider.setRange(0.,  currentIRState.totalDuration());
                
                currentIRState.totalDuration();
                m_waveformTopBounds.getWidth();
                
                startTimeLabel.setText(String(0.), dontSendNotification);
                endTimeLabel.setText(String(currentIRState.totalDuration()), dontSendNotification);
                
                //                logText += "loaded!!!\n";
                //                logText += "duration\n" + String(currentIRState.totalDuration());
                log.setText(logText);
                updateSliderInfo();
                
                repaint();
            } else {
                //                logText += "failed!!!\n";
                //                log.setText(logText);
            }
        }
    }
}
//===================================================================
void IRWaveformEditor::sliderValueChanged(juce::Slider *slider)
{
    if (slider == &selectionSlider) {
        updateSelection();
        currentIRState.thumbnail->sendChangeMessage();
    }
}
//===================================================================
void IRWaveformEditor::changeListenerCallback(juce::ChangeBroadcaster *source)
{
    
    if (source == dynamic_cast<ChangeBroadcaster*>(getParentComponent()))
    {
        setSliderPos();
        updateSliderInfo();
    }
}
//===================================================================
void IRWaveformEditor::updateSelection()
{
    double duration = selectionSlider.getMaxValue() - selectionSlider.getMinValue();
    
    m_overlayBounds = Rectangle<int>(jd::linlin(selectionSlider.getMinValue(),
                                                selectionSlider.getMinimum(),
                                                selectionSlider.getMaximum(),
                                                0.,
                                                (double)m_waveformTopBounds.getWidth() ),
                                     0,
                                     jd::linlin(duration,
                                                selectionSlider.getMinimum(),
                                                selectionSlider.getMaximum(),
                                                0.,
                                                (double)m_waveformTopBounds.getWidth() ),
                                     m_waveformTopBounds.getHeight() );
    
    currentIRState.startTime = selectionSlider.getMinValue();
    currentIRState.duration = duration;
    
    //Labels
    selectionStartLabel.setTopLeftPosition(m_overlayBounds.getX(), m_waveformInfoBounds.getY() + 30);
    selectionStartLabel.setText(String(selectionSlider.getMinValue()), dontSendNotification);
    selectionStartLabel.setSize(50, 25);
    
    selectionEndLabel.setText(String(selectionSlider.getMaxValue()), dontSendNotification);
    selectionEndLabel.setTopRightPosition(m_overlayBounds.getRight(), m_waveformInfoBounds.getY() + 30);
    selectionEndLabel.setSize(50, 25);
    
    repaint();
}
//===================================================================
void IRWaveformEditor::updateSliderInfo()
{
    selectionSlider.setRange(0., currentIRState.totalDuration());
    
    //    startTimeLabel.setText(String(0.), dontSendNotification);
    //    endTimeLabel.setText(String(currentIRState.totalDuration()), dontSendNotification);
}
//===================================================================
void IRWaveformEditor::setSliderPos()
{
    selectionSlider.setMinAndMaxValues(currentIRState.startTime,
                                       currentIRState.startTime +
                                       currentIRState.duration);
    updateSelection();
}
//===================================================================
/* IR Waveform section */
//===================================================================
IRWaveformSection::IRWaveformSection(IRState &sourceIRState):
currentIRState (sourceIRState)
{
    currentIRState.thumbnail->addChangeListener(this);
    
    //    env = std::make_shared<jd::Envelope<float>>();
    addAndMakeVisible(envGui);
    addAndMakeVisible(showEnvButton);
    showEnvButton.setButtonText("mk able.");
    showEnvButton.addListener(this);
    setEnvEditable(false);
}
//===================================================================
void IRWaveformSection::paint(juce::Graphics &g)
{
    auto r = getLocalBounds();
    currentIRState.drawThumbnailSection(g, r, 1.);
}
//===================================================================
void IRWaveformSection::resized()
{
    auto r = getLocalBounds();
    envGui.setBounds(r);
    showEnvButton.setBounds(0, 0, 50, 15);
    
    envGui.addDefaultNodes();
}
//===================================================================
void IRWaveformSection::buttonClicked(juce::Button *button)
{
    if (button == &showEnvButton) {
        envIsEditable = !envIsEditable;
        setEnvEditable(envIsEditable);
    }
}
//===================================================================
void IRWaveformSection::makePatternElementInfoEnvelope()
{
    envGui.getNewEnvelope(currentIRState.env, currentIRState.duration);
}
//===================================================================
void IRWaveformSection::setEnvelopeFromCurrentInfo()
{
    envGui.makeFromEnvelope(currentIRState.env);
}
