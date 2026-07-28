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
    virtual ~Effect()
    {
    }
    virtual void setpreset ([[maybe_unused]] int npreset)
    {
    }
    virtual void changepar ([[maybe_unused]] int npar, [[maybe_unused]] int value)
    {
    }
    virtual int getpar ([[maybe_unused]] int npar) 
    {
        return (0);
    }
    virtual void out ([[maybe_unused]] float * smpsl, [[maybe_unused]] float * smpsr)
    {
    }

    /// Process `nframes` samples in place.
    ///
    /// This is the block-size-aware entry point.  Effects that have been
    /// migrated off the global PERIOD block size override this and use
    /// `nframes` throughout, which is what a plugin host (VST3/CLAP/AU/LV2)
    /// requires since it chooses the block length per process() call.
    ///
    /// The default implementation forwards to the legacy fixed-size out(),
    /// so effects that have not been migrated yet keep working unchanged.
    virtual void out (float * smpsl, float * smpsr, [[maybe_unused]] int nframes)
    {
        out (smpsl, smpsr);
    }

    virtual void cleanup ()
    {
    }
    virtual float getfreqresponse ([[maybe_unused]] float freq)
    {
        return (0);
    }				//this is only used for EQ (for user interface)

    int Ppreset{};

    float outvolume{};
    FilterParams *filterpars{};
};

#endif
