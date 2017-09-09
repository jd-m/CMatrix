//
//  IRSelector.hpp
//  jd_CMatrix
//
//  Created by Jaiden Muschett on 07/09/2017.
//
//

#ifndef IRSelector_hpp
#define IRSelector_hpp

#include "IrSequencer.hpp"
#include "PluginProcessor.h"
//=====================================================================
//=========================================================================
/*              IR SEQUENCER
 Each Convolution Chain has an instance of an IR SEQUENCER which it
 uses to step through a sequence stored by the button grid;
 */
//=========================================================================
class IRSequencer {
public:
    IRSequencer(Jd_cmatrixAudioProcessor& p,
                HashMap<String, IRState>& sourceIRClipDefs,
                ButtonGrid& sourceButtonSequencer
                );
    void stepToNextEnabledValue();
    
    void setIRSequence(String irSequenceName);
    
    void reset();
    
    Jd_cmatrixAudioProcessor& processor;
    HashMap<String, IRState>& irClipDefs;
    IRSequence irSequence;
    ButtonGrid& buttonSequencer;
    
    int currentIndex {0};
    int numColumns { 16 };
    
};




//=====================================================================
#endif /* IRSelector_hpp */
