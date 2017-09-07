//
//  SignalDisplayUI.hpp
//  jd_CMatrix
//
//  Created by Jaiden Muschett on 06/09/2017.
//
//

#ifndef SignalDisplayUI_hpp
#define SignalDisplayUI_hpp

#include <stdio.h>
#include "WaveformViewer.hpp"

class WaveformDisplay : public Component {
public:
    enum ScalingMode {
        LIN,
        LOG_AMP,
        LOG_FREQ
    };
//================================================================
    void paint(Graphics& g) override;
    void resized() override;
//================================================================
    template<class C = std::vector<float>, class InputScalingFunc, class OutputScalingFunc>
    void drawScaledRange(Graphics &g,
                         String unit,
                         C unscaledMarkers,
                         InputScalingFunc scaleInput = [](float x){ return x;},
                         OutputScalingFunc scaleOutput = [](float x){ return x;});
//================================================================
    void setMode (ScalingMode newScalingMode);
    
    ScalingMode scalingMode;
};

#endif /* SignalDisplayUI_hpp */
