/*
  rakarrack - guitar multi-effects processor
  SPDX-License-Identifier: GPL-2.0-only

  EffectRegistry.cpp - Maps effect type indices onto the engine's instances.
*/

#include "EffectRegistry.hpp"
#include "global.hpp"
#include "AllEffects.hpp"

// ─── index → Effect pointer ────────────────────────────────────────

Effect* effectByIndex(RKR& rkr, int index)
{
    switch (index)
    {
    case  0: return rkr.efx_EQ1.get();
    case  1: return rkr.efx_Compressor.get();
    case  2: return rkr.efx_Distorsion.get();
    case  3: return rkr.efx_Overdrive.get();
    case  4: return rkr.efx_Echo.get();
    case  5: return rkr.efx_Chorus.get();
    case  6: return rkr.efx_Phaser.get();
    case  7: return rkr.efx_Flanger.get();
    case  8: return rkr.efx_Rev.get();
    case  9: return rkr.efx_EQ2.get();
    case 10: return rkr.efx_WhaWha.get();
    case 11: return rkr.efx_Alienwah.get();
    case 12: return rkr.efx_Cabinet.get();
    case 13: return rkr.efx_Pan.get();
    case 14: return rkr.efx_Har.get();
    case 15: return rkr.efx_MusDelay.get();
    case 16: return rkr.efx_Gate.get();
    case 17: return rkr.efx_NewDist.get();
    case 18: return rkr.efx_APhaser.get();
    case 19: return rkr.efx_Valve.get();
    case 20: return rkr.efx_DFlange.get();
    case 21: return rkr.efx_Ring.get();
    case 22: return rkr.efx_Exciter.get();
    case 23: return rkr.efx_MBDist.get();
    case 24: return rkr.efx_Arpie.get();
    case 25: return rkr.efx_Expander.get();
    case 26: return rkr.efx_Shuffle.get();
    case 27: return rkr.efx_Synthfilter.get();
    case 28: return rkr.efx_MBVvol.get();
    case 29: return rkr.efx_Convol.get();
    case 30: return rkr.efx_Looper.get();
    case 31: return rkr.efx_RyanWah.get();
    case 32: return rkr.efx_RBEcho.get();
    case 33: return rkr.efx_CoilCrafter.get();
    case 34: return rkr.efx_ShelfBoost.get();
    case 35: return rkr.efx_Vocoder.get();
    case 36: return rkr.efx_Sustainer.get();
    case 37: return rkr.efx_Sequence.get();
    case 38: return rkr.efx_Shifter.get();
    case 39: return rkr.efx_StompBox.get();
    case 40: return rkr.efx_Reverbtron.get();
    case 41: return rkr.efx_Echotron.get();
    case 42: return rkr.efx_StereoHarm.get();
    case 43: return rkr.efx_CompBand.get();
    case 44: return rkr.efx_Opticaltrem.get();
    case 45: return rkr.efx_Vibe.get();
    case 46: return rkr.efx_Infinity.get();
    default: return nullptr;
    }
}

// ─── index → Bypass flag pointer ───────────────────────────────────

int* bypassByIndex(RKR& rkr, int index)
{
    switch (index)
    {
    case  0: return &rkr.EQ1_Bypass;
    case  1: return &rkr.Compressor_Bypass;
    case  2: return &rkr.Distorsion_Bypass;
    case  3: return &rkr.Overdrive_Bypass;
    case  4: return &rkr.Echo_Bypass;
    case  5: return &rkr.Chorus_Bypass;
    case  6: return &rkr.Phaser_Bypass;
    case  7: return &rkr.Flanger_Bypass;
    case  8: return &rkr.Reverb_Bypass;
    case  9: return &rkr.EQ2_Bypass;
    case 10: return &rkr.WhaWha_Bypass;
    case 11: return &rkr.Alienwah_Bypass;
    case 12: return &rkr.Cabinet_Bypass;
    case 13: return &rkr.Pan_Bypass;
    case 14: return &rkr.Harmonizer_Bypass;
    case 15: return &rkr.MusDelay_Bypass;
    case 16: return &rkr.Gate_Bypass;
    case 17: return &rkr.NewDist_Bypass;
    case 18: return &rkr.APhaser_Bypass;
    case 19: return &rkr.Valve_Bypass;
    case 20: return &rkr.DFlange_Bypass;
    case 21: return &rkr.Ring_Bypass;
    case 22: return &rkr.Exciter_Bypass;
    case 23: return &rkr.MBDist_Bypass;
    case 24: return &rkr.Arpie_Bypass;
    case 25: return &rkr.Expander_Bypass;
    case 26: return &rkr.Shuffle_Bypass;
    case 27: return &rkr.Synthfilter_Bypass;
    case 28: return &rkr.MBVvol_Bypass;
    case 29: return &rkr.Convol_Bypass;
    case 30: return &rkr.Looper_Bypass;
    case 31: return &rkr.RyanWah_Bypass;
    case 32: return &rkr.RBEcho_Bypass;
    case 33: return &rkr.CoilCrafter_Bypass;
    case 34: return &rkr.ShelfBoost_Bypass;
    case 35: return &rkr.Vocoder_Bypass;
    case 36: return &rkr.Sustainer_Bypass;
    case 37: return &rkr.Sequence_Bypass;
    case 38: return &rkr.Shifter_Bypass;
    case 39: return &rkr.StompBox_Bypass;
    case 40: return &rkr.Reverbtron_Bypass;
    case 41: return &rkr.Echotron_Bypass;
    case 42: return &rkr.StereoHarm_Bypass;
    case 43: return &rkr.CompBand_Bypass;
    case 44: return &rkr.Opticaltrem_Bypass;
    case 45: return &rkr.Vibe_Bypass;
    case 46: return &rkr.Infinity_Bypass;
    default: return nullptr;
    }
}
