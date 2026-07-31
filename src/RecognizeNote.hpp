/* tuneit.c -- Detect fundamental frequency of a sound
* Copyright (C) 2004, 2005  Mario Lang <mlang@delysid.org>
*
* Modified for rakarrack by Josep Andreu
* Recognizer.h  Recognizer Audio Note definitions
*
* This is free software, placed under the terms of the
* GNU General Public License, as published by the Free Software Foundation.
* Please see the file COPYING for details.
*/


#ifndef RECOGNIZE_H_
#define RECOGNIZE_H_

#include <cmath>
#include <vector>
#include "dsp_constants.hpp"
#include "AnalogFilter.hpp"
#include "Sustainer.hpp"

class Recognize
{
public:
    Recognize (float trig);
    ~Recognize ();

    void schmittFloat (float *indatal, float *indatar);
    void sethpf(int value);
    void setlpf(int value);
    int note{};

    std::vector<signed short int> schmittBuffer;
    signed short int *schmittPointer{nullptr};
    const char **notes{nullptr};
    float trigfact{};
    float lafreq{};
    float nfreq{}, afreq{}, freq{};



private:

    void displayFrequency (float freq);
    void schmittInit (int size);
    void schmittS16LE (signed short int *indata);
    void schmittFree ();

    int ultima{};
    int blockSize{};

    std::unique_ptr<AnalogFilter> lpfl, lpfr, hpfl, hpfr;

    std::unique_ptr<Sustainer> Sus;

    std::vector<signed short int> buf{};

    /// The signal is conditioned before it is analysed, and every stage that
    /// does so works in place, so the analysis runs on these rather than on
    /// the caller's buffers.
    std::vector<float> scratchl{}, scratchr{};

};

#endif
