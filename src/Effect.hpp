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

    /// Apply a value that came from a stored preset.
    ///
    /// Defaults to changepar(), which is right for almost every effect. It is
    /// not right where changepar() does something other than store the value:
    /// Looper's play parameter is a toggle that ignores the value it is given,
    /// and Echotron recomputes its length from the impulse it has loaded. Those
    /// effects override this so a saved preset can be restored exactly rather
    /// than replayed as a series of commands.
    virtual void loadpreset (int npar, int value)
    {
        changepar (npar, value);
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

    /// Size internal per-block scratch storage for blocks of at most
    /// `maxBlockSize` samples.
    ///
    /// This is the allocation hook a plugin host drives (VST3 setupProcessing,
    /// AU/LV2 prepare, ...): it is called before processing starts and again
    /// whenever the host changes the block size, so that out() itself never has
    /// to allocate.  After calling it, out() must not be passed an `nframes`
    /// larger than `maxBlockSize`.
    ///
    /// Effects that keep no per-block scratch buffers do not need to override it.
    virtual void setMaxBlockSize ([[maybe_unused]] int maxBlockSize)
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
