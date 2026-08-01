/*
  Rakarrack Guitar FX

  Sustainer.h - Simple compressor/sustainer effect with easy interface, minimal controls
  Copyright (C) 2010 Ryan Billing
  Author: Ryan Billing

  This program is free software; you can redistribute it and/or modify
  it under the terms of version 3 of the GNU General Public License
  as published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License (version 2) for more details.

  You should have received a copy of the GNU General Public License (version 2)
  along with this program; if not, write to the Free Software Foundation,
  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA

*/

#ifndef SUSTAINER_H
#define SUSTAINER_H

#include "dsp_constants.hpp"
#include "Effect.hpp"

class Sustainer : public Effect
{
public:
    Sustainer ();
    ~Sustainer ();
    void cleanup () override;

    void out (float * smpsl, float * smpr) override;
    void out (float * smpsl, float * smpr, int nframes) override;
    void changepar (int npar, int value) override;
    int getpar (int npar) override;
    void setpreset (int npreset) override;



private:
    //Parametrii
    int Pvolume;	//Output Level
    int Psustain;	//Compression amount

    int timer{0}, hold{0};
    float level{0.0f}, fsustain{0.0f}, input{0.0f}, tmpgain{0.0f};
    float prls{0.0f}, compeak{0.0f}, compg{0.0f}, compenv{0.0f}, oldcompenv{0.0f};
    float calpha{0.0f}, cbeta{0.0f}, cthresh{0.0f}, cratio{0.0f}, cpthresh{0.0f};
};


#endif


