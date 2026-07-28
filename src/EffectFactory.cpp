/*
  rakarrack - guitar multi-effects processor
  SPDX-License-Identifier: GPL-2.0-only

  EffectFactory.cpp - Creates effect instances by type index.

  The index -> class mapping here must stay in step with effectByIndex() in
  EngineController.cpp, which is the mapping the GUI and the presets use.
*/

#include "EffectFactory.hpp"
#include "AllEffects.hpp"

std::unique_ptr<Effect> createEffect(int type, const EffectFactoryConfig& cfg)
{
    switch (type)
    {
    case  0: return std::make_unique<EQ>();                 // EQ1
    case  1: return std::make_unique<Compressor>();
    case  2: return std::make_unique<Distorsion>();
    case  3: return std::make_unique<Distorsion>();         // Overdrive
    case  4: return std::make_unique<Echo>();
    case  5: return std::make_unique<Chorus>();
    case  6: return std::make_unique<Phaser>();
    case  7: return std::make_unique<Chorus>();             // Flanger
    case  8: return std::make_unique<Reverb>();
    case  9: return std::make_unique<EQ>();                 // EQ2
    case 10: return std::make_unique<DynamicFilter>();      // WahWah
    case 11: return std::make_unique<Alienwah>();
    case 12: return std::make_unique<EQ>();                 // Cabinet
    case 13: return std::make_unique<Pan>();
    case 14: return std::make_unique<Harmonizer>(
                        static_cast<long>(cfg.harQuality),
                        cfg.harDown, cfg.harUpQ, cfg.harDownQ);
    case 15: return std::make_unique<MusicDelay>();
    case 16: return std::make_unique<Gate>();
    case 17: return std::make_unique<NewDist>();
    case 18: return std::make_unique<Analog_Phaser>();
    case 19: return std::make_unique<Valve>();
    case 20: return std::make_unique<Dflange>();
    case 21: return std::make_unique<Ring>();
    case 22: return std::make_unique<Exciter>();
    case 23: return std::make_unique<MBDist>();
    case 24: return std::make_unique<Arpie>();
    case 25: return std::make_unique<Expander>();
    case 26: return std::make_unique<Shuffle>();
    case 27: return std::make_unique<Synthfilter>();
    case 28: return std::make_unique<MBVvol>();
    case 29: return std::make_unique<Convolotron>(
                        cfg.conDown, cfg.conUpQ, cfg.conDownQ);
    case 30: return std::make_unique<Looper>(cfg.looperSize);
    case 31: return std::make_unique<RyanWah>();
    case 32: return std::make_unique<RBEcho>();
    case 33: return std::make_unique<CoilCrafter>();
    case 34: return std::make_unique<ShelfBoost>();
    case 35: return std::make_unique<Vocoder>(
                        cfg.auxResampled, cfg.vocBands,
                        cfg.vocDown, cfg.vocUpQ, cfg.vocDownQ);
    case 36: return std::make_unique<Sustainer>();
    case 37: return std::make_unique<Sequence>(
                        static_cast<long>(cfg.harQuality),
                        cfg.seqDown, cfg.seqUpQ, cfg.seqDownQ);
    case 38: return std::make_unique<Shifter>(
                        static_cast<long>(cfg.harQuality),
                        cfg.shiDown, cfg.shiUpQ, cfg.shiDownQ);
    case 39: return std::make_unique<StompBox>();
    case 40: return std::make_unique<Reverbtron>(
                        cfg.revDown, cfg.revUpQ, cfg.revDownQ);
    case 41: return std::make_unique<Echotron>();
    case 42: return std::make_unique<StereoHarm>(
                        static_cast<long>(cfg.steQuality),
                        cfg.steDown, cfg.steUpQ, cfg.steDownQ);
    case 43: return std::make_unique<CompBand>();
    case 44: return std::make_unique<Opticaltrem>();
    case 45: return std::make_unique<Vibe>();
    case 46: return std::make_unique<Infinity>();
    default: return nullptr;
    }
}
