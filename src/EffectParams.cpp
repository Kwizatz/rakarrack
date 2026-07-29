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
    // swept parameters, but it is the only way to reach state that no
    // parameter reports -- DynamicFilter's filter definitions, for one.
    effect.setpreset(settings.preset);

    for (int i = 0; i < kEffectParamSlots; ++i)
    {
        const auto idx = static_cast<std::size_t>(i);
        const int value = (idx < settings.params.size()) ? settings.params[idx] : 0;
        // loadpreset() rather than changepar(), so effects whose changepar()
        // is a command rather than a setting restore instead of firing.
        effect.loadpreset(i, value);
    }

    // Most setpreset() implementations end this way, for the same reason:
    // changing a filter's frequency or gain leaves it interpolating towards
    // the new value, and a restored effect should start settled.
    effect.cleanup();
}
