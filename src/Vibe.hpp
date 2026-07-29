/*
  rakarrack - a guitar effects software

 Vibe.h  -  Vibe Effect definitions

  Copyright (C) 2008-2010 Ryan Billing
  Author: Josep Andreu & Ryan Billing

 This program is free software; you can redistribute it and/or modify
 it under the terms of version 2 of the GNU General Public License
 as published by the Free Software Foundation.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License (version 2) for more details.

 You should have received a copy of the GNU General Public License
 (version2)  along with this program; if not, write to the Free Software
 Foundation,
 Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA

*/


#ifndef Vibe_H
#define Vibe_H

#include "dsp_constants.hpp"
#include "EffectLFO.hpp"
#include "Effect.hpp"

class Vibe : public Effect
{

public:

    Vibe ();
    ~Vibe ();

    void out (float * smpsl, float * smpsr) override;
    void out (float * smpsl, float * smpsr, int nframes) override;
    void setvolume(int value);
    void setpanning(int value);
    void setpreset (int npreset);
    void changepar (int npar, int value);
    int getpar (int npar);
    void cleanup ();


private:
    int Pwidth;
    int Pfb;
    int Plrcross;
    int Pdepth;
    int Ppanning;
    int Pvolume;
    int Pstereo;

    float fwidth;
    float fdepth;
    float rpanning{0.0f}, lpanning{0.0f};
    float flrcross{0.0f}, fcross{0.0f};
    float fb{0.0f};
    EffectLFO lfo;

    float Ra{0.0f}, Rb{0.0f}, b{0.0f}, dTC{0.0f}, dRCl{0.0f}, dRCr{0.0f};
    float lampTC{0.0f}, ilampTC{0.0f}, minTC{0.0f}, alphal{0.0f}, alphar{0.0f};
    // Same opto smoothing as Opticaltrem: read before written, then fed back
    // into itself, so a garbage start value does not decay.
    float stepl{0.0f}, stepr{0.0f}, oldstepl{0.0f}, oldstepr{0.0f};
    float fbr{0.0f}, fbl{0.0f};
    float dalphal{0.0f}, dalphar{0.0f};
    float lstep{0.0f},rstep{0.0f};
    float cperiod{0.0f};
    float gl{0.0f}, oldgl{0.0f};
    float gr{0.0f}, oldgr{0.0f};

    class fparams
    {
    public:
        // x1/y1 carry the filter state between samples and are read before
        // they are first written.
        float x1{0.0f};
        float y1{0.0f};
        //filter coefficients
        float n0{0.0f};
        float n1{0.0f};
        float d0{0.0f};
        float d1{0.0f};
    } vc[8], vcvo[8], ecvc[8], vevo[8], bootstrap[8];

    float vibefilter(float data, fparams *ftype, int stage);
    void init_vibes();
    void modulate(float ldrl, float ldrr);
    float bjt_shape(float data);

    float R1{0.0f};
    float Rv{0.0f};
    float C2{0.0f};
    float C1[8]{};
    float beta{0.0f};  //transistor forward gain.
    float gain, k;
    float oldcvolt[8] ;
    float en1[8], en0[8], ed1[8], ed0[8];
    float cn1[8], cn0[8], cd1[8], cd0[8];
    float ecn1[8], ecn0[8], ecd1[8], ecd0[8];
    float on1[8], on0[8], od1[8], od0[8];
};

#endif
