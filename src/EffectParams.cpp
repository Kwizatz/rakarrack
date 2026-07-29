/*
  rakarrack - guitar multi-effects processor
  SPDX-License-Identifier: GPL-2.0-only

  EffectParams.cpp - Capturing and restoring an effect instance's settings.
*/

#include "EffectParams.hpp"
#include "Effect.hpp"

#include <cstddef>

EffectSettings captureEffectSettings(Effect& effect)
{
    EffectSettings settings;
    settings.preset = effect.Ppreset;

    settings.params.assign(static_cast<std::size_t>(kEffectParamSlots), 0);
    for (int i = 0; i < kEffectParamSlots; ++i)
        settings.params[static_cast<std::size_t>(i)] = effect.getpar(i);

    // Unused slots read back as zero, and there are a lot of them at the top of
    // the range. Trimming keeps saved presets readable; applying pads back out.
    while (!settings.params.empty() && settings.params.back() == 0)
        settings.params.pop_back();

    return settings;
}

void applyEffectSettings(Effect& effect, const EffectSettings& settings)
{
    // Replay the preset first. Most of what it does is then overwritten by the
    // stored parameters, but it is the only way to reach state that no
    // parameter reports -- DynamicFilter's filter definitions, for one.
    effect.setpreset(settings.preset);

    // Only the slots that were captured. Sweeping the whole index space would
    // mean calling changepar() a hundred-odd extra times, and that is not free:
    // StompBox runs init_tone() at the end of every changepar(), and that
    // rewrites the tone coefficients it just read. Replaying the preset above
    // has already put the effect in the same starting state the source was in,
    // so the trailing slots trimmed by capture need no writing.
    for (std::size_t i = 0; i < settings.params.size(); ++i)
    {
        // loadpreset() rather than changepar(), so effects whose changepar()
        // is a command rather than a setting restore instead of firing.
        effect.loadpreset(static_cast<int>(i), settings.params[i]);
    }

    // Most setpreset() implementations end this way, for the same reason:
    // changing a filter's frequency or gain leaves it interpolating towards
    // the new value, and a restored effect should start settled.
    effect.cleanup();
}
