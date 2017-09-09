//
//  IRSelector.cpp
//  jd_CMatrix
//
//  Created by Jaiden Muschett on 07/09/2017.
//
//

#include "IRSelector.hpp"

IRSequencer::IRSequencer(Jd_cmatrixAudioProcessor& p,
                         HashMap<String, IRState>& sourceIRClipDefs,
                         ButtonGrid& sourceButtonSequencer):
processor(p),
irClipDefs(sourceIRClipDefs),
buttonSequencer(sourceButtonSequencer)
{

}

void IRSequencer::stepToNextEnabledValue()
{
    int numColumns = 16;
    while (numColumns > 0) {
        currentIndex %= numColumns;
        if (irSequence[currentIndex++].isEnabled)
        {
            auto element = irSequence.getReference(currentIndex);
            
            auto clip = irClipDefs[element.irClipName];
            
            std::cout << "currentIndex: " << currentIndex
            << " clipNAme " << element.irClipName << " clip num "
            << element.irClipindex
            << std::endl;
            numColumns = 0;
        }
        numColumns--;
    }
}

void IRSequencer::setIRSequence(String irSequenceName)
{
    irSequence = buttonSequencer.irSequences[irSequenceName];
    reset();
}

void IRSequencer::reset()
{
    currentIndex = 0;
}
