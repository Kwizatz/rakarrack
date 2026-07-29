/*
  rakarrack - guitar multi-effects processor
  SPDX-License-Identifier: GPL-2.0-only

  EffectParams.hpp - Capturing and restoring an effect instance's settings.

  The legacy preset keyed parameters by effect type: lv[type][slot]. That works
  only while there is exactly one instance of each effect, which is the
  assumption a graph breaks -- two Chorus nodes have to be able to differ.

  Effects already expose getpar()/changepar() over an integer index space, so a
  dense sweep of that space captures an instance's settings without needing to
  know anything about the effect. Two things do not fit through that interface,
  and both are handled here:

    - Some effects hold state only setpreset() establishes, the clearest being
      the filter definitions in DynamicFilter's filterpars. Nothing reports
      those through getpar(), so the preset index is recorded too and replayed
      first; the swept parameters then override whatever it set.

    - A few effects treat changepar() as a command rather than a setting.
      Effect::loadpreset() is the way past that; see the comment there.

  Depends only on Effect, so it stays usable without the engine.
*/

#ifndef EFFECT_PARAMS_H
#define EFFECT_PARAMS_H

#include <vector>

class Effect;

/// Size of the parameter index space swept by capture and apply.
///
/// The widest user is EQ, whose bands run to index 10 + 5*16 - 1 = 89. Indices
/// no effect uses read back as zero and are ignored on the way in, so the only
/// cost of a generous bound is a few unused entries.
inline constexpr int kEffectParamSlots = 128;

/// Everything needed to reproduce one effect instance's configuration.
struct EffectSettings
{
    /// The preset the effect was built from, replayed before `params`.
    int preset{0};

    /// One entry per parameter slot, trailing zeros trimmed.
    std::vector<int> params;

    friend bool operator==(const EffectSettings&, const EffectSettings&) = default;
};

/// Read an effect's configuration.
[[nodiscard]] EffectSettings captureEffectSettings(Effect& effect);

/// Write a configuration back into `effect`.
///
/// Every slot is written, not just the ones present in `params`: a slot that
/// holds zero is still a real setting, and leaving it out would let the target
/// keep its own default instead. Slots beyond `params` are written as zero for
/// the same reason. Applied in ascending index order, which matters for
/// effects like EQ where a band's type resets the rest of that band.
void applyEffectSettings(Effect& effect, const EffectSettings& settings);

#endif
