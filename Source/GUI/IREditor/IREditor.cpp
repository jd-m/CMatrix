#include "IREditor.hpp"
//===================================================================
/* Pattern Element Info */
//===================================================================
IRPatternElementInfo::IRPatternElementInfo(int sourceSamplesPerThumbnailSample,
                                           AudioFormatManager* formatManagerToUse,
                                           AudioThumbnailCache* cacheToUse):
samplesPerThumbnailSample(sourceSamplesPerThumbnailSample),
formatManager(formatManagerToUse),
thumbnailCache(cacheToUse),
thumbnail(new AudioThumbnail(sourceSamplesPerThumbnailSample, *formatManagerToUse, *cacheToUse))
{
    
}
void IRPatternElementInfo::drawFullThumbnail(juce::Graphics &g, const Rectangle<int> &area, double zoomFactor)
{
    g.setColour(Colours::black);
    thumbnail->drawChannels(g, area, 0., totalDuration(), zoomFactor);
}
void IRPatternElementInfo::drawThumbnailSection(juce::Graphics &g, const Rectangle<int> &area, double zoomFactor, bool isSelected , Colour selectionColour)
{
    if (isSelected)
        g.setColour(selectionColour);
    else
        g.setColour(Colours::black);
    
    thumbnail->drawChannels(g, area, startTime, startTime + duration, zoomFactor);
}
void IRPatternElementInfo::copyStateFrom(const IRPatternElementInfo target)
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
}
//===================================================================
/* IR Waveform Editor */
//===================================================================
IRWaveformEditor::IRWaveformEditor (IRPatternElementInfo &sourceIrPatternElementInfo):
m_patternElementInfo(sourceIrPatternElementInfo)
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
    m_patternElementInfo.thumbnail->addChangeListener(this);
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
void IRWaveformEditor::paint(juce::Graphics &g)
{
    g.drawRect(m_optionBounds);
    g.drawRect(m_waveformTopBounds);
    g.drawRect(m_waveformInfoBounds);
    m_patternElementInfo.drawFullThumbnail(g, m_waveformTopBounds, 1.);
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
                m_patternElementInfo.reader =  (formatManager.createReaderFor(file));
                m_patternElementInfo.loadedFile = file;
            }
            if (m_patternElementInfo.reader != nullptr)
            {
                
                m_patternElementInfo.thumbnail->setSource(new FileInputSource (file));
                selectionSlider.setRange(0., m_patternElementInfo.totalDuration());
                sliderToIr.set(m_patternElementInfo.totalDuration(), m_waveformTopBounds.getWidth());
                
                startTimeLabel.setText(String(0.), dontSendNotification);
                endTimeLabel.setText(String(m_patternElementInfo.totalDuration()), dontSendNotification);
                
                logText += "loaded!!!\n";
                logText += "duration\n" + String(m_patternElementInfo.totalDuration());
                log.setText(logText);
                //
                updateSliderInfo();
                
                repaint();
            } else {
                logText += "failed!!!\n";
                log.setText(logText);
            }
        }
    }
}
//Slider
void IRWaveformEditor::sliderValueChanged(juce::Slider *slider)
{
    if (slider == &selectionSlider) {
        updateSelection();
        m_patternElementInfo.thumbnail->sendChangeMessage();
    }
}
//Change
void IRWaveformEditor::changeListenerCallback(juce::ChangeBroadcaster *source)
{
}
//Waveform Editor
void IRWaveformEditor::updateSelection()
{
    double duration = selectionSlider.getMaxValue() - selectionSlider.getMinValue();
    m_overlayBounds = Rectangle<int>(sliderToIr(selectionSlider.getMinValue()),
                                     0,
                                     sliderToIr(duration),
                                     m_waveformTopBounds.getHeight() );
    
    m_patternElementInfo.startTime = selectionSlider.getMinValue();
    m_patternElementInfo.duration = duration;
    
    //log
    //    log.setText(String(sliderToIr(duration)));
    //Labels
    selectionStartLabel.setTopLeftPosition(m_overlayBounds.getX(), m_waveformInfoBounds.getY() + 30);
    selectionStartLabel.setText(String(selectionSlider.getMinValue()), dontSendNotification);
    selectionStartLabel.setSize(50, 25);
    
    selectionEndLabel.setText(String(selectionSlider.getMaxValue()), dontSendNotification);
    selectionEndLabel.setTopRightPosition(m_overlayBounds.getRight(), m_waveformInfoBounds.getY() + 30);
    selectionEndLabel.setSize(50, 25);
    
    repaint();
}
void IRWaveformEditor::updateSliderInfo()
{
    selectionSlider.setRange(0., m_patternElementInfo.totalDuration());
    sliderToIr.set(m_patternElementInfo.totalDuration(), m_waveformTopBounds.getWidth());
    
    startTimeLabel.setText(String(0.), dontSendNotification);
    endTimeLabel.setText(String(m_patternElementInfo.totalDuration()), dontSendNotification);
}
void IRWaveformEditor::setSliderPos()
{
    selectionSlider.setMinAndMaxValues(m_patternElementInfo.startTime, m_patternElementInfo.startTime + m_patternElementInfo.duration);
    updateSelection();
}
//===================================================================
/* IR Waveform section */
//===================================================================
IRWaveformSection::IRWaveformSection(IRPatternElementInfo &sourceIrPatternElementInfo):
m_patternElementInfo (sourceIrPatternElementInfo)
{
    m_patternElementInfo.thumbnail->addChangeListener(this);
    
    //    env = std::make_shared<jd::Envelope<float>>();
    addAndMakeVisible(envGui);
    addAndMakeVisible(showEnvButton);
    showEnvButton.setButtonText("mk able.");
    showEnvButton.addListener(this);
    setEnvEditable(false);
}
void IRWaveformSection::paint(juce::Graphics &g)
{
    auto r = getLocalBounds();
    m_patternElementInfo.drawThumbnailSection(g, r, 1.);
}
void IRWaveformSection::resized()
{
    auto r = getLocalBounds();
    envGui.setBounds(r);
    showEnvButton.setBounds(0, 0, 50, 15);
}
void IRWaveformSection::buttonClicked(juce::Button *button)
{
    if (button == &showEnvButton) {
        envIsEditable = !envIsEditable;
        setEnvEditable(envIsEditable);
    }
}
void IRWaveformSection::makePatternElementInfoEnvelope()
{
    m_patternElementInfo.env = std::move(envGui.getNewEnvelope(m_patternElementInfo.duration));
}
void IRWaveformSection::setEnvelopeFromCurrentInfo()
{
    envGui.makeFromEnvelope(m_patternElementInfo.env);
}
