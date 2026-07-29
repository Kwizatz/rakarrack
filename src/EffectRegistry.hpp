/*
  rakarrack - guitar multi-effects processor
  SPDX-License-Identifier: GPL-2.0-only

  EffectRegistry.hpp - Maps effect type indices onto the engine's instances.

  The rack addresses effects by a type index in the range 0..46. That index is
  what presets, the MIDI tables, the GUI panels and the legacy efx_order chain
  all store, so the mapping from index to the RKR member holding that effect
  has to be identical everywhere. Keeping it in one place is what makes that
  true; these tables used to live privately inside EngineController.cpp.
*/

#ifndef EFFECT_REGISTRY_H
#define EFFECT_REGISTRY_H

class RKR;
class Effect;

/// The number of distinct effect type indices, i.e. valid values are 0..46.
inline constexpr int kEffectTypeCount = 47;

/// The engine's instance of the given effect type, or nullptr if out of range.
Effect* effectByIndex(RKR& rkr, int index);

/// The engine's bypass flag for the given effect type, or nullptr if out of
/// range. Non-zero means the effect is active.
int* bypassByIndex(RKR& rkr, int index);

#endif
