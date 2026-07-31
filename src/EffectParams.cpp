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
    //
    // Only when the effect is not already on that preset, though. A freshly
    // built effect ran setpreset() in its own constructor, so replaying the
    // same number calls it twice, and setpreset() is not idempotent for
    // several effects: Gate, Convolotron and Echotron all came out different.
    // The engine's own preset loader does not replay it either, and matching
    // that is what makes a rebuilt effect sound like the one it came from.
    if (settings.preset != effect.Ppreset)
        effect.setpreset(settings.preset);

    // Settle here rather than at the end. Changing a filter's frequency or
    // gain leaves it interpolating towards the new value, and cleanup() clears
    // that pending interpolation -- so clearing last would skip a crossfade
    // the engine does perform. The engine's own preset loader clears state and
    // then writes the parameters; a distortion stage never forgets a
    // difference in its starting state, so restoring in that order is what
    // makes a rebuilt effect sound like the one it was captured from.
    effect.cleanup();

    // Every slot, not just the ones capture kept. Trailing zeros are trimmed
    // for storage, but they are still values the source held: replaying the
    // preset above may have put something non-zero in those slots, and the
    // engine's own loader writes the full range. Skipping them would leave a
    // rebuilt effect carrying settings from the preset rather than from the
    // patch. Indices past an effect's range fall through changepar()'s switch
    // and cost nothing.
    //
    // No parameters at all means something different from "all zero": a
    // hand-written layout that names only a preset should get that preset,
    // not a silenced effect.
    if (settings.params.empty())
        return;

    for (int i = 0; i < kEffectParamSlots; ++i)
    {
        const std::size_t slot = static_cast<std::size_t>(i);
        const int value = (slot < settings.params.size()) ? settings.params[slot] : 0;

        // loadpreset() rather than changepar(), so effects whose changepar()
        // is a command rather than a setting restore instead of firing.
        effect.loadpreset(i, value);
    }
}
