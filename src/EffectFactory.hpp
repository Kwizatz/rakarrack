/*
  rakarrack - guitar multi-effects processor
  SPDX-License-Identifier: GPL-2.0-only

  EffectFactory.hpp - Creates effect instances by type index.

  The legacy engine owns exactly one object per effect type (RKR::efx_Chorus,
  RKR::efx_Echo, ...), which makes the type index and the instance identity the
  same thing.  The node graph needs several instances of the same type, so it
  builds them through this factory instead.

  Note that a type index is NOT the same as a C++ class: several indices are
  backed by the same class (Chorus/Flanger, EQ1/EQ2/Cabinet,
  Compressor/FLimiter, Distorsion/Overdrive).  The index is the identity the
  presets and the GUI use.
*/

#pragma once

#include "EffectRegistry.hpp"

#include <memory>

class Effect;

/// Options for the effects whose constructors take arguments.
///
/// These mirror the values RKR reads from preferences at start-up, so a graph
/// node produces an effect configured identically to the legacy engine's.
struct EffectFactoryConfig
{
    // Pitch-shift quality (smbPitchShift FFT frame size selector)
    int harQuality{4};
    int steQuality{4};

    // Per-family downsample state and resampler qualities
    int harDown{0}, harUpQ{4}, harDownQ{4};
    int revDown{0}, revUpQ{4}, revDownQ{4};
    int conDown{0}, conUpQ{4}, conDownQ{4};
    int shiDown{0}, shiUpQ{4}, shiDownQ{4};
    int seqDown{0}, seqUpQ{4}, seqDownQ{4};
    int vocDown{0}, vocUpQ{4}, vocDownQ{4};
    int steDown{0}, steUpQ{4}, steDownQ{4};

    int   vocBands{32};
    float looperSize{1.0f};

    /// Side-chain input the Vocoder reads. Owned by the engine, borrowed here.
    float* auxResampled{nullptr};
};

/// Create a new instance of the effect identified by `type`.
/// Returns nullptr for an out-of-range type.
///
/// For display names use EngineController::getEffectTypeName(), which is the
/// single source of truth shared with the GUI.
[[nodiscard]] std::unique_ptr<Effect> createEffect(int type,
                                                   const EffectFactoryConfig& cfg);
