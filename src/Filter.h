/*
  ZynAddSubFX - a software synthesizer

  Filter.h - Filters, uses analog,formant,etc. filters
  Copyright (C) 2002-2005 Nasca Octavian Paul
  Author: Nasca Octavian Paul

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

#ifndef FILTER_H
#define FILTER_H

#include "global.h"
#include "Filter_.h"
#include "AnalogFilter.h"
#include "FormantFilter.h"
#include "SVFilter.h"
#include "FilterParams.h"


class Filter
{
public:
    Filter (FilterParams * pars);
    ~Filter ();
    void filterout (float * smp);
    void setfreq (float frequency);
    void setfreq_and_q (float frequency, float q_);
    void setq (float q_);
    float getrealfreq (float freqpitch);

private:
    unsigned char category;

    Filter_ * filter;
};


#endif
