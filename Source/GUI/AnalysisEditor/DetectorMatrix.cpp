//
//  DetectorMatrix.cpp
//  jd_CMatrix
//
//  Created by Jaiden Muschett on 10/09/2017.
//
//

#include "DetectorMatrix.hpp"
//===========================================================================
DetectorMatrix::DetectorMatrix(Jd_cmatrixAudioProcessor&p,
                               int sourceDetectorIndex):
processor(p),
detectorIndex(sourceDetectorIndex)
{
    
    for (int i = 0; i < numColumns; i++)
    {
        auto newSetRequireWithinRangeButton = new ToggleButton ();
        addAndMakeVisible(newSetRequireWithinRangeButton);
        newSetRequireWithinRangeButton->addListener(this);
        
        if (detectorIndex == i) {
            newSetRequireWithinRangeButton->setEnabled(false);
            newSetRequireWithinRangeButton->setToggleState(false, dontSendNotification);
        }
        
        setRequireWithinRangeButtons.add(newSetRequireWithinRangeButton);
        
        auto newSetRequireOutsideRangeButton = new ToggleButton ();
        addAndMakeVisible(newSetRequireOutsideRangeButton);
        newSetRequireOutsideRangeButton->addListener(this);
        
        if (detectorIndex == i) {
            newSetRequireOutsideRangeButton->setEnabled(false);
            newSetRequireOutsideRangeButton->setToggleState(false, dontSendNotification);
        }
        
        setRequireOutsideRangeButtons.add(newSetRequireOutsideRangeButton);
    }
    //require nothing from self
    processor.requirementsOfOtherDetectors[detectorIndex][detectorIndex] = RequiredDetectorState::none;
    
    setLookAndFeel(&lookAndFeel);
    
    setSize(400,100);
}
//===========================================================================
void DetectorMatrix::paintOverChildren(juce::Graphics &g)
{
    
    auto r = getLocalBounds();
    
    g.drawRoundedRectangle(r.toFloat(), 5.f, 4);
    g.setColour(Colours::darkgrey.withAlpha(0.5f));
    
    for (int i = 0; i < numColumns; i++)
    {
        if (i == detectorIndex){
            g.fillRect(setRequireWithinRangeButtons[i]->getBounds());
            g.fillRect(setRequireOutsideRangeButtons[i]->getBounds());
        }
    }
    
    g.setColour(Colours::lightgrey);
    g.drawText("matrix", r.removeFromTop(25), Justification::centred);
    
}
//===========================================================================
void DetectorMatrix::resized()
{
    auto r = getLocalBounds();
    
    r.removeFromTop(25);
    auto boxHeight = r.getHeight()/numColumns;
    auto boxWidth = r.getWidth()/2;
    auto left = r.removeFromLeft(boxWidth);
    auto right = r.removeFromRight(boxWidth);
    
    for (auto setRequireWithinRangeButton : setRequireWithinRangeButtons)
        setRequireWithinRangeButton->setBounds(left.removeFromTop(boxHeight).reduced(5));
    
    for (auto setRequireOutsideRangeButton : setRequireOutsideRangeButtons)
        setRequireOutsideRangeButton->setBounds(right.removeFromTop(boxHeight).reduced(5));
    
}
//===========================================================================
void DetectorMatrix::buttonClicked(Button* changedButton)
{
    if (setRequireWithinRangeButtons.contains(dynamic_cast<ToggleButton*>(changedButton)))
    {
        int index = getIndexAtItem(setRequireWithinRangeButtons, changedButton);
        
        setRequireOutsideRangeButtons[index]->setToggleState(false, dontSendNotification);
        
        if (changedButton->getToggleState()) {
            processor.requirementsOfOtherDetectors[detectorIndex][index] = RequiredDetectorState::withinRange;
        }  else {
            processor.requirementsOfOtherDetectors[detectorIndex][index] = RequiredDetectorState::none;
        }

    }
    
    if (setRequireOutsideRangeButtons.contains(dynamic_cast<ToggleButton*>(changedButton)))
    {
        int index = getIndexAtItem(setRequireOutsideRangeButtons, changedButton);
        
        setRequireWithinRangeButtons[index]->setToggleState(false, dontSendNotification);
        
        if (changedButton->getToggleState()) {
            processor.requirementsOfOtherDetectors[detectorIndex][index] = RequiredDetectorState::outsideRange;
        } else {
        processor.requirementsOfOtherDetectors[detectorIndex][index] = RequiredDetectorState::none;
        }
    }
}
