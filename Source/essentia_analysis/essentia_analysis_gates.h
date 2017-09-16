//
//  essentia_analysis_gates.h
//  jd_CMatrix
//
//  Created by Jaiden Muschett on 03/09/2017.
//
//

#ifndef essentia_analysis_gates_h
#define essentia_analysis_gates_h
#include "jdHeader.h"
#include "Settings.h"

using RangeDetector = jd::GateDouble<float>;

struct DetectorUnit {
    void init (float sampleRate, float controlRate, int blockSize)
    {
        rangeChecker.init(controlRate, blockSize);
        rmsEnvelope.init(controlRate, blockSize);
        smoothedValue.setSampleRate(sampleRate);
        smoothedValue.setDurationS(0.01,1.f);
    }
    //===============================================================
    void setRange(float newLower, float newUpper) {
        rangeChecker.setThresholds(newLower, newUpper);
    }
    //===============================================================
    void setLimits(float newLower, float newUpper)
    {
        limits = {newLower, newUpper};
    }
    //===============================================================
    void setRangeFromNormalised(float newLowerNormalised, float newUpperNormalised)
    {
        auto newLower = jd::linlin(newLowerNormalised, 0.f,1.f,limits.lower, limits.upper);
        auto newUpper = jd::linlin(newUpperNormalised, 0.f,1.f,limits.lower, limits.upper);
        setRange(newLower, newUpper);
    }
    //===============================================================
    void setRMSWindowSize(float newRmsSizeMS) {
        rmsEnvelope.setBufferSizeMS(newRmsSizeMS);
    }
    //===============================================================
    void setAttackCoeff(float attackTimeMS) {
        rmsEnvelope.setAttackTimeMS(attackTimeMS);
    }
    //===============================================================
    void setReleaseCoeff(float releaseTimeMS) {
        rmsEnvelope.setReleaseTimeMS(releaseTimeMS);
    }
    //===============================================================
    void setInputScalingFunc(jd::FloatConversionFunc<float> newInputScalingFunc)
    {
        scaleInput = newInputScalingFunc;
    }
    //===============================================================
    void setOutputScalingFunc(jd::FloatConversionFunc<float> newOutputScalingFunc)
    {
        scaleOutput = newOutputScalingFunc;
    }
    //===============================================================
    void setInput(float inputSample) {
        using namespace jd;
        if (isEnabled) {
            float envelope = rmsEnvelope.processedSample(inputSample);
            output = shouldConvertInput ? scaleInput(envelope) : envelope;
            gateCode = rangeChecker.checkThreshold(output);
            smoothedValue.setTarget(output);
            smoothedValue.updateTarget();
        } else {
            smoothedValue.setTarget(0.f);
        }
    }
    //===============================================================
    void setEnabled(bool setShouldEnable)
    {
        isEnabled = setShouldEnable;
    }
    //===============================================================
    void applySmoothing() {
        output = smoothedValue.nextValue();
    }
    //===============================================================
    float smoothedEnvelope() {
        return smoothedValue.currentValue();
    }
    //===============================================================
    float normalisedScaled(float value) {
        if (shouldConvertOutput)
        {
            return jd::linlin(scaleOutput(jd::clip(value,
                                                   limits.lower,
                                                   limits.upper)),
                              scaleOutput(limits.lower),
                              scaleOutput(limits.upper),
                              0.f,
                              1.f);
        } else {
            return jd::linlin((value),
                              (limits.lower),
                              (limits.upper),
                              0.f,
                              1.f);
        
        }
            
    }
    //===============================================================
    float normalisedScaledOutput() {
        return normalisedScaled(output);
    }
    //===============================================================
    bool isWithinRange() const
    {
        return rangeChecker.isWithinRange();
    }
    int getGateCode ()
    {
        return gateCode;
    }
    
    bool crossedThresholdOnLastCheck()
    {
        return getGateCode() > -1;
    }
    //===============================================================
    template<class Func>
    void performOnEnvelope(Func funcToPerform)
    {
        funcToPerform(rmsEnvelope);
    }
    //===============================================================
    template<class Func>
    void performOnRangeChecker(Func funcToPerform)
    {
        funcToPerform(rangeChecker);
    }
    
    RangeDetector rangeChecker;
    jd::RMSEnvelopeFollower<float> rmsEnvelope;
    jd::FloatConversionFunc<float> scaleInput = [](float x){return x;};
    jd::FloatConversionFunc<float> scaleOutput = [](float x){return x;};
    jd::Range<float> limits { 0.f, 1.f };
    jd::SmoothedValue<float> smoothedValue;
    bool shouldSmooth {true};
    bool isEnabled { true };
    float output {0};
    bool shouldConvertInput {false};
    bool shouldConvertOutput {false};
    int gateCode {-1};
};

class DetectorChain {
public:
    
    DetectorUnit& operator [] (int index)
    {
        return detectors [index];
    }
    
    bool allEnabledDetectorsWithinRange()
    {
        bool allWithinSoFar {true};
        int numEnabled {0};
        for (const auto& d : detectors) {
            if (d.isEnabled) {
                allWithinSoFar = allWithinSoFar &&
                d.isWithinRange();
                numEnabled++;
            }
        }
        return numEnabled > 0 ? allWithinSoFar : false;
        
    }
    std::array<DetectorUnit, NUM_DETECTORS> detectors;
};

#endif /* essentia_analysis_gates_h */
