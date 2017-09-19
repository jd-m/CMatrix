#ifndef WaveformViewer_hpp
#define WaveformViewer_hpp

#include <stdio.h>
#include "../JuceLibraryCode/JuceHeader.h"
#include "../essentia_analysis/essentia_analysis_gates.h"
#include "Settings.h"

class SignalDrawer  :   public Component,
                        public Timer
{
public:
    SignalDrawer();

    void paint (Graphics& g);
    
    void addSample (const float sample);
    
    void setSamplesToAverage (size_t newSamplesToAverage);
    void setLineColour (Colour newLineColour);
    void setIsActive(bool newIsActive);
    
    void clear ();
    
    void timerCallback();

private:
    bool isActive {true};
    std::vector<float> circularBuffer;
    float currentInputLevel;
    Colour lineColour;
    int volatile bufferPos { 0}, bufferSize { 2048 }, numSamplesIn { 0 }, samplesToAverage { 128 };
};
#endif /* WaveformViewer_hpp */
