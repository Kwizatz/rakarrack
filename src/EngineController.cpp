/*
  rakarrack - guitar multi-effects processor
  SPDX-License-Identifier: GPL-2.0-only

  EngineController.cpp - Thread-safe bridge between GUI and audio engine.
*/

#include "EngineController.hpp"
#include "EffectRegistry.hpp"
#include "global.hpp"
#include "AllEffects.hpp"

// ─── Construction ──────────────────────────────────────────────────

EngineController::EngineController(RKR& engine)
    : m_engine(engine)
{
}

// ─── Parameter Access ──────────────────────────────────────────────

void EngineController::setEffectParameter(int effectIndex, int paramId, int value)
{
    if (auto* efx = effectByIndex(m_engine, effectIndex))
        efx->changepar(paramId, value);
}

int EngineController::getEffectParameter(int effectIndex, int paramId) const
{
    if (auto* efx = effectByIndex(const_cast<RKR&>(m_engine), effectIndex))
        return efx->getpar(paramId);
    return 0;
}

void EngineController::setEffectPreset(int effectIndex, int preset)
{
    if (auto* efx = effectByIndex(m_engine, effectIndex))
        efx->setpreset(preset);
}

int EngineController::getEffectPreset(int effectIndex) const
{
    if (auto* efx = effectByIndex(const_cast<RKR&>(m_engine), effectIndex))
        return efx->Ppreset;
    return 0;
}

// ─── Effect Chain ──────────────────────────────────────────────────

void EngineController::setEffectOrder(std::span<const int> order)
{
    for (std::size_t i = 0; i < order.size() && i < m_engine.efx_order.size(); ++i)
        m_engine.efx_order[i] = order[i];
}

std::array<int, kMaxEffectSlots> EngineController::getEffectOrder() const
{
    std::array<int, kMaxEffectSlots> result{};
    for (int i = 0; i < kMaxEffectSlots; ++i)
        result[i] = m_engine.efx_order[i];
    return result;
}

void EngineController::setEffectEnabled(int effectIndex, bool enabled)
{
    if (auto* bp = bypassByIndex(m_engine, effectIndex))
        *bp = enabled ? 1 : 0;
}

bool EngineController::isEffectEnabled(int effectIndex) const
{
    if (auto* bp = bypassByIndex(const_cast<RKR&>(m_engine), effectIndex))
        return *bp != 0;
    return false;
}

// ─── Presets / Banks ───────────────────────────────────────────────

void EngineController::loadPreset(int bankSlot)
{
    m_engine.Bank_to_Preset(bankSlot);
}

void EngineController::savePreset(int bankSlot)
{
    m_engine.Preset_to_Bank(bankSlot);
}

void EngineController::newPreset()
{
    m_engine.New();
}

std::string EngineController::getPresetName([[maybe_unused]] int bankSlot) const
{
    // Bank names are stored in the preset bank structure
    // For now return empty — will be connected when PresetBank is refactored
    return {};
}

// ─── Global Controls ───────────────────────────────────────────────

void EngineController::setMasterVolume(int value)
{
    m_engine.Master_Volume = static_cast<float>(value);
    m_engine.calculavol(2);
}

int EngineController::getMasterVolume() const
{
    return static_cast<int>(m_engine.Master_Volume);
}

void EngineController::setInputGain(int value)
{
    m_engine.Input_Gain = static_cast<float>(value);
    m_engine.calculavol(1);
}

int EngineController::getInputGain() const
{
    return static_cast<int>(m_engine.Input_Gain);
}

void EngineController::setBypass(bool bypass)
{
    // Engine convention: Bypass == 1 means "process effects" (active).
    // Semantic wrapper: setBypass(true) means "skip effects" → Bypass = 0.
    m_engine.Bypass = bypass ? 0 : 1;
}

bool EngineController::isBypassed() const
{
    // Bypass == 0  → effects are skipped → semantically "bypassed".
    return m_engine.Bypass == 0;
}

void EngineController::setBalance(int value)
{
    m_engine.Fraction_Bypass = static_cast<float>(value) / 127.0f;
}

int EngineController::getBalance() const
{
    return static_cast<int>(m_engine.Fraction_Bypass * 127.0f);
}

void EngineController::tapTempo()
{
    m_engine.TapTempo();
}

std::string EngineController::getEffectTypeName(int effectType) const
{
    static constexpr const char* names[] = {
        "EQ",          "Compressor", "Distortion",  "Overdrive",
        "Echo",        "Chorus",     "Phaser",       "Flanger",
        "Reverb",      "EQ",         "WahWah",       "AlienWah",
        "Cabinet",     "Pan",        "Harmonizer",   "MusDelay",
        "Gate",        "NewDist",    "Ana.Phaser",   "Valve",
        "Dual Flange", "Ring",       "Exciter",      "MBDist",
        "Arpie",       "Expander",   "Shuffle",      "Synthfilter",
        "MBVvol",      "Convolotron","Looper",       "RyanWah",
        "RBEcho",      "CoilCrafter","ShelfBoost",   "Vocoder",
        "Sustainer",   "Sequence",   "Shifter",      "StompBox",
        "Reverbtron",  "Echotron",   "StereoHarm",   "CompBand",
        "Opticaltrem", "Vibe",       "Infinity"
    };
    if (effectType >= 0 && effectType < static_cast<int>(std::size(names)))
        return names[effectType];
    return "Unknown";
}

// ─── Real-Time Telemetry (GUI polls) ───────────────────────────────

bool EngineController::pollLevels(AudioLevels& out)
{
    return m_levels_rb.pop_latest(out);
}

bool EngineController::pollTuner(TunerData& out)
{
    return m_tuner_rb.pop_latest(out);
}

bool EngineController::pollLooper(LooperStatus& out)
{
    return m_looper_rb.pop_latest(out);
}

bool EngineController::pollTapTempo(TapTempoStatus& out)
{
    return m_tap_rb.pop_latest(out);
}

bool EngineController::pollChord(ChordInfo& out)
{
    return m_chord_rb.pop_latest(out);
}

// ─── RT Thread Push ────────────────────────────────────────────────

void EngineController::pushLevels(const AudioLevels& levels)
{
    (void)m_levels_rb.push(levels); // drop if full — never block RT thread
}

void EngineController::pushTuner(const TunerData& data)
{
    (void)m_tuner_rb.push(data);
}

void EngineController::pushLooper(const LooperStatus& status)
{
    (void)m_looper_rb.push(status);
}

void EngineController::pushTapTempo(const TapTempoStatus& status)
{
    (void)m_tap_rb.push(status);
}

void EngineController::pushChord(const ChordInfo& info)
{
    (void)m_chord_rb.push(info);
}
