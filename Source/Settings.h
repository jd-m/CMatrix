#ifndef Settings_h
#define Settings_h
#include "jdHeader.h"

enum ARDetectors {
    LEVEL,
    NUM_AR_DETECTORS
};

enum KRDetectors {
    PITCH = NUM_AR_DETECTORS,
    PITCH_CONFIDENCE,
    PITCH_SALIENCE,
    INHARMONICITY,
    NUM_DETECTORS
};

constexpr auto NUM_KR_DETECTORS {  NUM_DETECTORS - NUM_AR_DETECTORS };

namespace util
{
    const int maxNumConvolvers { 7 };
    const std::array<int, NUM_AR_DETECTORS> audioRateDetectors { LEVEL };
    const std::array<int, NUM_KR_DETECTORS> controlRateDetectors { PITCH,
        PITCH_CONFIDENCE,
        PITCH_SALIENCE,
        INHARMONICITY };
    
    const std::array<float,5> freqScale {20.f, 100.f, 500.f, 2000.f, 10000.f };
    const std::array<float,5> logAmpScale {-60.f, -20.f,-6.f,0.f, 6.f };
    const std::array<float,5> linAmpScale {0.f , 0.25f, 0.5f, 0.75f, 1.f};
    
    const auto freqLimits { jd::Range<float>::fromArray(freqScale) };
    const auto logAmpLimits { jd::Range<float>::fromArray(linAmpScale) };
    
};

#endif /* Settings_h */
