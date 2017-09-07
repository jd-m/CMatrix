//
//  JDLookAndFeel.hpp
//  jdConvolverGUI
//
//  Created by Jaiden Muschett on 23/08/2017.
//
//

#ifndef JDLookAndFeel_hpp
#define JDLookAndFeel_hpp

#include <stdio.h>
#include "../JuceLibraryCode/JuceHeader.h"

class CmatrixLookAndFeel : public LookAndFeel_V3 {
public:
    
    CmatrixLookAndFeel();
    void drawToggleButton (Graphics& g, ToggleButton& button, bool isMouseOverButton, bool isButtonDown) override;
    Font getTextButtonFont (TextButton& button, int buttonHeight) override;
};



#endif /* JDLookAndFeel_hpp */
