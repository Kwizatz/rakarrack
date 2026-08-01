/*

  MBDist.h - Distorsion Effect

  Copyright (C) 2002-2005 Nasca Octavian Paul
  Author: Nasca Octavian Paul
  ZynAddSubFX - a software synthesizer

  Modified for rakarrack by Josep Andreu

  This program is free software; you can redistribute it and/or modify
  it under the terms of version 2 of the GNU General Public License
  as published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License (version 2) for more details.

  You should have received a copy of the GNU General Public License (version 2)
  along with this program; if not, write to the Free Software Foundation,
  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA

*/

#ifndef MBVVOL_H
#define MBVVOL_H

#include "dsp_constants.hpp"
#include "AnalogFilter.hpp"
#include "EffectLFO.hpp"
#include "Effect.hpp"


class MBVvol : public Effect
{
public:
    MBVvol ();
    ~MBVvol ();
    void out (float * smpsl, float * smpr) override;
    void out (float * smpsl, float * smpr, int nframes) override;
    void setMaxBlockSize (int maxBlockSize) override;
    void setpreset (int npreset) override;
    void changepar (int npar, int value) override;
    int getpar (int npar) override;
    void cleanup () override;


    std::vector<float> lowl;
    std::vector<float> lowr;
    std::vector<float> midll;
    std::vector<float> midlr;
    std::vector<float> midhl;
    std::vector<float> midhr;
    std::vector<float> highl;
    std::vector<float> highr;


private:

    void setvolume (int Pvolume);
    void setpanning (int Ppanning);
    void setCross1 (int value);
    void setCross2 (int value);
    void setCross3 (int value);
    void setCombi (int value);


    //Parametrii
    int Pvolume;	//Volumul or E/R
    int Pcombi;
    int Cross1;
    int Cross2;
    int Cross3;

    //Parametrii reali

    float coeff{0.0f};
    float lfo1l{0.0f},lfo1r{0.0f},lfo2l{0.0f},lfo2r{0.0f};
    float v1l{0.0f},v1r{0.0f},v2l{0.0f},v2r{0.0f};
    float d1{0.0f},d2{0.0f},d3{0.0f},d4{0.0f};
    float volL{0.0f},volML{0.0f},volMH{0.0f},volH{0.0f};
    float volLr{0.0f},volMLr{0.0f},volMHr{0.0f},volHr{0.0f};
    std::unique_ptr<AnalogFilter> lpf1l, lpf1r, hpf1l, hpf1r;
    std::unique_ptr<AnalogFilter> lpf2l, lpf2r, hpf2l, hpf2r;
    std::unique_ptr<AnalogFilter> lpf3l, lpf3r, hpf3l, hpf3r;

    EffectLFO lfo1,lfo2;
};


#endif
