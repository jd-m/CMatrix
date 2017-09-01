#ifndef AnalysisChain_hpp
#define AnalysisChain_hpp

#include "essentia_analysis.hpp"


class AnalysisChain {
public:
    
    enum AnalysisTypes {
        AMP,
        PITCH,
        PITCH_SALIENCE,
        INHARMONICITY,
        NUM_TYPES
    };

    void processBlock(const float* input, float* output, int numSamples)
    {
        for (int i = 0; i < NUM_TYPES; i++)
        {
            if (shouldUseType[i]);
                
        }
    }
    
    std::vector<char> shouldUseType;//vector<char?>
    
};
#endif /* AnalysisChain_hpp */
