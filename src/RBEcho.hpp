/*
  ZynAddSubFX - a software synthesizer

  Echo.h - Echo Effect
  Copyright (C) 2002-2005 Nasca Octavian Paul
  Author: Nasca Octavian Paul

  Modified for rakarrack by Josep Andreu

  Reverse Echo by Transmogrifox

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

#ifndef RBECHO_H
#define RBECHO_H

#include "dsp_constants.hpp"
#include "delayline.hpp"
#include "Effect.hpp"

class RBEcho : public Effect
{
public:
    RBEcho ();
    ~RBEcho ();
    void out (float * smpsl, float * smpr) override;
    void out (float * smpsl, float * smpr, int nframes) override;
    void setpreset (int npreset);
    void changepar (int npar, int value);
    int getpar (int npar);
    void cleanup ();




private:

    void setvolume (int Pvolume);
    void setpanning (int Ppanning);
    void setdelay (int Pdelay);
    void setlrdelay (int Plrdelay);
    void setlrcross (int Plrcross);
    void setfb (int Pfb);
    void sethidamp (int Phidamp);
    void setreverse (int Preverse);
    void initdelays ();

    //Parametrii
    int Pvolume;	// E/R
    int Ppanning;	//Panning
    int Pdelay;
    int Plrdelay;	// L/R delay difference
    int Plrcross;	// L/R Mixing
    int Pfb;		//Feed-back
    int Phidamp;
    int Preverse;
    int Psubdiv;
    int Pes;

    int maxx_delay{0};

    float delay{0.0f}, lrdelay{0.0f}, ltime{0.0f}, rtime{0.0f};
    float fdelay{0.0f}, subdiv{0.0f}, pes{0.0f}, pingpong{0.0f}, ipingpong{0.0f};
    float rvl{0.0f}, rvr{0.0f};
    float rpanning{0.0f}, lpanning{0.0f}, lrcross{0.0f}, fb{0.0f}, hidamp{0.0f};
    float reverse{0.0f}, ireverse{0.0f}, lfeedback{0.0f}, rfeedback{0.0f};
    float oldl{0.0f}, oldr{0.0f};		//pt. lpf
    float  Srate_Attack_Coeff{0.0f};

    std::unique_ptr<delayline> ldelay, rdelay;
};


#endif
