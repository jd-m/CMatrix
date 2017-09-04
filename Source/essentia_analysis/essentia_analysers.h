#ifndef essentia_analyser_interface_h
#define essentia_analyser_interface_h

#include "essentia_analyser.h"

/* Names kept the same as in essentia for sake of consistency */
struct PitchYinFFT :
public Analyser<PitchYinFFT, float, float>
{
    enum Outputs {
        PITCH,
        PITCH_CONFIDENCE,
        NUM_OUTPUTS
    };
    
    static const std::string name() { return  "PitchYinFFT"; };
};

struct PitchSalience :
public Analyser<PitchSalience, float>
{
    enum Outputs {
        PITCH_SALIENCE,
        NUM_OUTPUTS
    };
    static const std::string name() { return  "PitchSalience"; };
};

template<class T>
struct SingleBufferOutputter :
public Analyser<T, std::vector<float>> {};

struct DCRemoval :
public Analyser<DCRemoval, std::vector<float>>
{
    enum Outputs {
        SIGNAL,
        NUM_OUTPUTS
    };
    static const std::string name() { return  "DCRemoval"; };
};
struct Windowing :
public SingleBufferOutputter<Windowing>
{
    enum Outputs {
        FRAME,
        NUM_OUTPUTS
    };
    static const std::string name() { return  "Windowing"; };
};
struct Spectrum :
public SingleBufferOutputter<Spectrum>
{
    enum Outputs {
        SPECTRUM,
        NUM_OUTPUTS
    };
    static const std::string name() { return  "Spectrum"; };
};

template<class T>
struct DoubleBufferOutputter :
public Analyser<T,
std::vector<float>,
std::vector<float>>
{ };

struct SpectralPeaks :
public DoubleBufferOutputter<SpectralPeaks> {
    enum Outputs {
        FREQUENCIES,
        MAGNITUDES,
        NUM_OUTPUTS
    };
    static const std::string name() { return  "SpectralPeaks"; };
};

struct HarmonicPeaks :
public DoubleBufferOutputter<HarmonicPeaks> {
    enum Outputs {
        FREQUENCIES,
        MAGNITUDES,
        NUM_OUTPUTS
    };
    static const std::string name() { return  "HarmonicPeaks"; };
};

struct Inharmonicity :
public Analyser<Inharmonicity, float>
{
    enum Outputs {
        INHARMONICITY,
        NUM_OUTPUTS
    };
    static const std::string name() { return  "Inharmonicity"; };
};

struct Dissonance :
public Analyser<Dissonance, float>
{
    enum Outputs {
        DISSONANCE,
        NUM_OUTPUTS
    };
    static const std::string name() { return  "Dissonance"; };
};

#endif /* essentia_analyser_interface_h */
