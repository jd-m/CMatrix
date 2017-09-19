//
//  JDLookAndFeel.cpp
//  jdConvolverGUI
//
//  Created by Jaiden Muschett on 23/08/2017.
//
//

#include "LookAndFeel.hpp"

CmatrixLookAndFeel::CmatrixLookAndFeel ()
{
}

void CmatrixLookAndFeel::drawToggleButton(juce::Graphics &g, juce::ToggleButton &button, bool isMouseOverButton, bool isButtonDown)
{
    const int width = button.getWidth();
    const int height = button.getHeight();
    Rectangle<int> r (0, 0, width, height);
    
    g.setColour(Colours::darkgrey);
    g.fillRoundedRectangle(r.toFloat(), 2.f);
    g.setColour(Colours::lightgrey);
    g.fillRoundedRectangle(r.reduced(5).toFloat(), 2.f);
    
    if (isButtonDown || button.getToggleState())
    {
        g.setColour(Colours::darkorange);
        g.fillRoundedRectangle(r.reduced(10).toFloat(), 2.f);
    };
    
}

Font CmatrixLookAndFeel::getTextButtonFont (TextButton& button, int buttonHeight)
{
    return Font ("courier", jmin (15.0f, buttonHeight * 0.6f), 0);
}
