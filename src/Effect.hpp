/*
  ZynAddSubFX - a software synthesizer

  Effect.h - this class is inherited by the all effects(Reverb, Echo, ..)
  Copyright (C) 2002-2005 Nasca Octavian Paul
  Author: Nasca Octavian Paul

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

#ifndef EFFECT_H
#define EFFECT_H

#include "dsp_constants.hpp"
#include "FilterParams.hpp"


class Effect
{
public:

    virtual ~ Effect () {
    };
    virtual void setpreset (int npreset) {
    };
    virtual void changepar (int npar, int value) {
    };
    virtual int getpar (int npar) {
        return (0);
    };
    virtual void out (float * smpsl, float * smpsr) {
    };
    virtual void cleanup () {
    };
    virtual float getfreqresponse (float freq) {
        return (0);
    };				//this is only used for EQ (for user interface)

    int Ppreset{};

    float *efxoutl{};
    float *efxoutr{};

    float outvolume{};

    FilterParams *filterpars{};
};

#endif
