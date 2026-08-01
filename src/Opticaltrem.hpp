/*
  rakarrack - a guitar effects software

 Opticaltrem.h  -  Opticaltrem Effect definitions

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


#ifndef Opticaltrem_H
#define Opticaltrem_H

#include "dsp_constants.hpp"
#include "EffectLFO.hpp"
#include "Effect.hpp"

class Opticaltrem : public Effect
{

public:

    Opticaltrem ();
    ~Opticaltrem ();

    void out (float * smpsl, float * smpsr) override;
    void out (float * smpsl, float * smpsr, int nframes) override;
    void setpanning(int value);
    void setpreset (int npreset) override;
    void changepar (int npar, int value) override;
    int getpar (int npar) override;
    void cleanup () override;



private:
    int Pdepth;
    int Ppanning;
    int Pinvert;  //Invert the opto and resistor relationship
 
    float Ra, Rb, R1, Rp, b, dTC, dRCl, dRCr, minTC, alphal, alphar;
    // The opto smoothing filter reads oldstepl/oldstepr before it writes them
    // and then feeds the result back in, so a garbage start value does not
    // decay -- it propagates out of the effect and into whatever follows.
    float stepl{0.0f}, stepr{0.0f}, oldstepl{0.0f}, oldstepr{0.0f};
    float fdepth;
    float lstep,rstep;
    float cperiod;
    float gl, oldgl;
    float gr, oldgr;
    float rpanning, lpanning;
    EffectLFO lfo;
};

#endif
