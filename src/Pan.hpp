/*
  rakarrack - a guitar effects software

  pan.h  -  Auto/Pan - Extra stereo definitions
  Copyright (C) 2008-2010 Josep Andreu
  Author: Josep Andreu

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

#ifndef AUTOPAN_H
#define AUTOPAN_H

#include "dsp_constants.hpp"
#include "EffectLFO.hpp"
#include "Effect.hpp"

class Pan : public Effect
{

public:
    Pan (float *efxoutl_, float *efxoutr_);
    ~Pan ();
    void out (float *smpsl, float *smpsr);
    void setpreset (int npreset);
    void changepar (int npar, int value);
    int getpar (int npar);
    void cleanup ();






private:

    void setvolume (int Pvolume);
    void setpanning (int Ppanning);
    void setextra (int Pdepth);

    int Pvolume;
    int Ppanning;
    int Pextra;
    int PAutoPan;
    int PextraON;


    float dvalue,cdvalue,sdvalue;
    float panning, mul;
    float lfol, lfor;
    float ll, lr;

    EffectLFO lfo;
};

#endif
