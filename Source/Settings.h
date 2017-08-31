#ifndef Settings_h
#define Settings_h

enum AnalysisTypes {
    AMP,
    PITCH,
    PITCH_SALIENCE,
    INHARMONICITY,
    NUM_TYPES
};

namespace settings
{
    const int maxNumConvolvers { 8 };
    
};

#endif /* Settings_h */
