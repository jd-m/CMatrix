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
    
};

struct PitchSalience :
public Analyser<PitchSalience, float>
{
    enum Outputs {
        PITCH_SALIENCE,
        NUM_OUTPUTS
    };
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
};
struct Windowing :
public SingleBufferOutputter<Windowing>
{
    enum Outputs {
        FRAME,
        NUM_OUTPUTS
    };
};
struct Spectrum :
public SingleBufferOutputter<Spectrum>
{
    enum Outputs {
        SPECTRUM,
        NUM_OUTPUTS
    };
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
};

struct HarmonicPeaks :
public DoubleBufferOutputter<HarmonicPeaks> {
    enum Outputs {
        FREQUENCIES,
        MAGNITUDES,
        NUM_OUTPUTS
    };
};

struct Inharmonicity :
public Analyser<Inharmonicity, float>
{
    enum Outputs {
        INHARMONICITY,
        NUM_OUTPUTS
    };
};

struct Dissonance :
public Analyser<Dissonance, float>
{
    enum Outputs {
        DISSONANCE,
        NUM_OUTPUTS
    };
};

#endif /* essentia_analyser_interface_h */
