/*
  rakarrack - a guitar effects software

  dsp_constants.hpp - DSP constants, utility functions, and audio runtime globals
  Copyright (C) 2008-2010 Josep Andreu
  Author: Josep Andreu & Ryan Billing

 This program is free software; you can redistribute it and/or modify
 it under the terms of version 2 of the GNU General Public License
 as published by the Free Software Foundation.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License (version 2) for more details.

 You should have received a copy of the GNU General Public License
 (version2)  along with this program; if not, write to the Free Software Foundation,
 Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA

*/

#ifndef DSP_CONSTANTS_HPP
#define DSP_CONSTANTS_HPP

#include <memory>
#include <array>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <string>
#include <type_traits>

#include "config.hpp"

// Math constants
inline constexpr float D_PI = 6.283185f;
inline constexpr float PI = 3.141598f;
inline constexpr float LOG_10 = 2.302585f;
inline constexpr float LOG_2 = 0.693147f;
inline constexpr float LN2R = 1.442695041f;
inline constexpr float CNST_E = 2.71828182845905f;
inline constexpr float AMPLITUDE_INTERPOLATION_THRESHOLD = 0.0001f;

// Effect parameter limits
inline constexpr int FF_MAX_VOWELS = 6;
inline constexpr int FF_MAX_FORMANTS = 12;
inline constexpr int FF_MAX_SEQUENCE = 8;
inline constexpr int MAX_FILTER_STAGES = 5;
inline constexpr int POLY = 8;
inline constexpr float DENORMAL_GUARD = 1e-18f;
inline constexpr float D_NOTE = 1.059463f;
inline constexpr float LOG_D_NOTE = 0.057762f;
inline constexpr float D_NOTE_SQRT = 1.029302f;
inline constexpr int MAX_PEAKS = 8;
inline constexpr int MAX_ALIENWAH_DELAY = 100;
inline constexpr float ATTACK = 0.175f;
inline constexpr int MAX_DELAY = 2;
inline constexpr int MAXHARMS = 8;
inline constexpr int MAX_PHASER_STAGES = 12;
inline constexpr float MAX_CHORUS_DELAY = 250.0f;
inline constexpr float LN2 = 1.0f;
inline constexpr float MUG_CORR_FACT = 0.4f;
inline constexpr float Thi = 0.67f;
inline constexpr float Tlo = -0.65f;
inline constexpr float Tlc = -0.6139445f;
inline constexpr float Thc = 0.6365834f;
inline constexpr float CRUNCH_GAIN = 100.0f;
inline constexpr float DIV_TLC_CONST = 0.002f;
inline constexpr float DIV_THC_CONST = 0.0016666f;
inline constexpr float D_FLANGE_MAX_DELAY = 0.055f;
inline constexpr float LFO_CONSTANT = 9.765625e-04f;
inline constexpr float LOG_FMAX = 10.0f;
inline constexpr float MINDEPTH = 20.0f;
inline constexpr float MAXDEPTH = 15000.0f;
inline constexpr int MAX_EQ_BANDS = 16;
inline constexpr int CLOSED = 1;
inline constexpr int OPENING = 2;
inline constexpr int OPEN = 3;
inline constexpr int CLOSING = 4;
inline constexpr float ENV_TR = 0.0001f;
inline constexpr int HARMONICS = 11;
inline constexpr int REV_COMBS = 8;
inline constexpr int REV_APS = 4;
inline constexpr int MAX_SFILTER_STAGES = 12;

// Utility functions
inline constexpr float RND() { return static_cast<float>(rand()) / (RAND_MAX + 1.0f); }
inline constexpr float RND1() { return static_cast<float>(rand()) / (static_cast<float>(RAND_MAX) + 1.0f); }
inline void F2I(float f, int &i) { i = (f > 0) ? static_cast<int>(f) : static_cast<int>(f - 1.0f); }
inline float dB2rap(float dB) { return expf(dB * LOG_10 / 20.0f); }
inline float rap2dB(float rap) { return 20.0f * logf(rap) / LOG_10; }
template<typename T, typename U, typename V>
inline constexpr auto CLAMP(T x, U low, V high) { return (x > high) ? static_cast<std::common_type_t<T,U,V>>(high) : ((x < low) ? static_cast<std::common_type_t<T,U,V>>(low) : static_cast<std::common_type_t<T,U,V>>(x)); }
inline constexpr float INTERPOLATE_AMPLITUDE(float a, float b, float x, float size) { return a + (b - a) * x / size; }
inline bool ABOVE_AMPLITUDE_THRESHOLD(float a, float b) { return (2.0f * fabsf(b - a) / (fabsf(b + a + 0.0000000001f))) > AMPLITUDE_INTERPOLATION_THRESHOLD; }
inline constexpr uint32_t SwapFourBytes(uint32_t data) { return ((data >> 24) & 0x000000ff) | ((data >> 8) & 0x0000ff00) | ((data << 8) & 0x00ff0000) | ((data << 24) & 0xff000000); }

// Fast power-of-2 approximation
union ls_pcast32 {
    float f;
    int32_t i;
};

static constexpr std::array<float, 5> a = { 1.00000534060469f, 0.693057900547259f, 0.239411678986933f, 0.0532229404911678f, 0.00686649174914722f };
static constexpr std::array<float, 25> pw2 = {1.0f, 2.0f, 4.0f, 8.0f, 16.0f, 32.0f, 64.0f, 128.0f, 256.0f, 512.0f, 1024.0f, 2048.0f, 4096.0f, 8192.0f, 16384.0f, 32768.0f, 65536.0f, 131072.0f, 262144.0f, 524288.0f, 1048576.0f, 2097152.0f, 4194304.0f, 8388608.0f, 16777216.0f};
static constexpr std::array<float, 25> ipw2 = {1.0f, 5.0e-01f, 2.5e-01f, 1.25e-01f, 6.25e-02f, 3.125e-02f, 1.5625e-02f, 7.8125e-03f, 3.90625e-03f, 1.953125e-03f, 9.765625e-04f, 4.8828125e-04f, 2.44140625e-04f, 1.220703125e-04f, 6.103515625e-05f, 3.0517578125e-05f, 1.52587890625e-05f, 7.62939453125e-06f, 3.814697265625e-06f, 1.9073486328125e-06f, 9.5367431640625e-07f, 4.76837158203125e-07f, 2.38418579101562e-07f, 1.19209289550781e-07f, 5.96046447753906e-08f};

static inline float f_pow2(float x)
{
    float y = 0.0f;

    if(x >=24) return pw2[24];
    else if (x <= -24.0f) return ipw2[24];
    else {
        float whole =  ceilf(x);
        int xint = (int) whole;
        x = x - whole;

        if (xint>=0) {
            y = pw2[xint]*(x*(x*(x*(x*a[4] + a[3]) + a[2]) + a[1]) + a[0]);

        } else  {

            y = ipw2[-xint]*(x*(x*(x*(x*a[4] + a[3]) + a[2]) + a[1]) + a[0]);

        }

        return y;
    }

}

inline float f_exp(float x) { return f_pow2(x * LN2R); }

// Audio runtime globals (defined in process.cpp)
extern int PERIOD;
extern unsigned int SAMPLE_RATE;
extern float fPERIOD;
extern float fSAMPLE_RATE;
extern float cSAMPLE_RATE;
extern float val_sum;
extern float aFreq;
extern int reconota;
extern int Wave_res_amount;
extern int Wave_up_q;
extern int Wave_down_q;
extern std::array<int, POLY> note_active;
extern std::array<int, POLY> rnote;
extern std::array<int, POLY> gate;
extern std::array<int, 50> pdata;
extern std::array<float, 12> r__ratio;
extern std::array<float, 12> freqs;
extern std::array<float, 12> lfreqs;

// Program state globals also used by some effects
extern int error_num;
extern int preset;

#endif
