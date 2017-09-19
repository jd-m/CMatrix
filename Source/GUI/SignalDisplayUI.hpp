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
    template<class CollectionType = std::vector<float>, class ScalingFunc>
    void drawScaledRangeMarkers(Graphics &g,
                         String unit,
                         CollectionType unscaledMarkers,
                         ScalingFunc scaleFunc = [](float x){ return x;});
//================================================================
    void setMode (ScalingMode newScalingMode);
    
    ScalingMode scalingMode;
};

#endif /* SignalDisplayUI_hpp */
