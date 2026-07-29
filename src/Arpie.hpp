/*
  Arpie.h - Echo Effect w/ arpeggiated delay
  Copyright (C) 2002-2005 Nasca Octavian Paul
  Author: Nasca Octavian Paul

  Modified for rakarrack by Josep Andreu

  Arpeggiated Echo by Ryan Billing (a.k.a. Transmogrifox)

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

#ifndef ARPIE_H
#define ARPIE_H

#include "dsp_constants.hpp"
#include "Effect.hpp"

class Arpie : public Effect
{
public:
    Arpie ();
    ~Arpie ();
    void out (float * smpsl, float * smpr) override;
    void out (float * smpsl, float * smpr, int nframes) override;
    void setpreset (int npreset);
    void changepar (int npar, int value);
    int getpar (int npar);
    void cleanup ();


private:
    //Parametrii
    int Pvolume;	//Volumul or E/R
    int Ppanning;	//Panning
    int Pdelay;
    int Plrdelay;	// L/R delay difference
    int Plrcross;	// L/R Mixing
    int Pfb;		//Feed-back-ul
    int Phidamp;
    int Preverse;
    int Ppattern;
    int Pharms;
    int Psubdiv;


    void setvolume (int Pvolume);
    void setpanning (int Ppanning);
    void setdelay (int Pdelay);
    void setlrdelay (int Plrdelay);
    void setlrcross (int Plrcross);
    void setfb (int Pfb);
    void sethidamp (int Phidamp);
    void setreverse (int Preverse);
    void setpattern (int Ppattern);


    //Parametrii reali
    void initdelays ();

    int dl{0}, dr{0}, delay{0}, lrdelay{0};
    // rvfl/rvfr are subscripted at the top of the sample loop but only assigned
    // at the bottom, so the first sample of a session reads whatever they hold.
    int kl{0}, kr{0}, rvkl{0}, rvkr{0}, rvfl{0}, rvfr{0};
    int maxx_delay{0}, fade{0}, harmonic{0}, envcnt{0}, invattack{1};
    int subdiv{};
    std::vector<int> pattern;

    float panning, lrcross, fb, hidamp, reverse;
    std::vector<float> ldelay, rdelay;
    float oldl{0.0f}, oldr{0.0f};		//pt. lpf
    float  Srate_Attack_Coeff, envattack, envswell;
};


#endif
