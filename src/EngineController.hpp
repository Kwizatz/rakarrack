/*
  rakarrack - guitar multi-effects processor
  SPDX-License-Identifier: GPL-2.0-only

  EngineController.hpp - Thread-safe interface between GUI and audio engine.

  The GUI layer talks to the engine exclusively through this class.
  Parameter changes are communicated via atomic operations;
  real-time telemetry (levels, tuner, spectrum) flows through lock-free
  ring buffers from the RT thread to the GUI poll timer.
*/

#pragma once

#include "RingBuffer.hpp"
#include <array>
#include <atomic>
#include <cstdint>
#include <span>
#include <string>

// Forward declaration — the GUI never includes global.hpp directly.
class RKR;

/// Snapshot of audio levels pushed from the RT thread every period.
struct AudioLevels
{
    float input_left{-48.0f};
    float input_right{-48.0f};
    float output_left{-48.0f};
    float output_right{-48.0f};
    float aux{-48.0f};
    float cpu_load{0.0f};
    int   have_signal{0};
    int   limiter_clipping{0};
    int   limiter_limiting{0};
};

/// Snapshot of tuner data pushed from the RT thread.
struct TunerData
{
    int   note_index{-1};       ///< Index into Tuner::notes[]
    float nearest_freq{0.0f};   ///< Frequency of nearest note
    float actual_freq{0.0f};    ///< Detected pitch frequency
    float cents{0.0f};          ///< Deviation in cents
    char  note_name[8]{};       ///< e.g. "A4", "C#3"
};

/// Snapshot of looper status.
struct LooperStatus
{
    int playing{0};
    int stopped{0};
    int quarter{0};
    int bar{0};
};

/// Snapshot of tap tempo state.
struct TapTempoStatus
{
    int   display_flag{0};      ///< 1=update, 2=deactivate
    float tempo_bpm{120.0f};
};

/// Snapshot of chord recognition.
struct ChordInfo
{
    int  changed{0};
    char name[32]{};
};

/// Command from GUI → Engine for parameter changes.
struct ParamCommand
{
    int effect_index{0};
    int param_id{0};
    int value{0};
};

/// Number of effect slots in the processing chain.
inline constexpr int kMaxEffectSlots = 10;

/// Total number of effect types.
inline constexpr int kNumEffectTypes = 47;

/// Thread-safe controller wrapping the RKR audio engine.
///
/// Usage pattern:
///   - GUI thread calls get/set methods and polls ring buffers via a timer.
///   - RT thread (JACK callback) calls pushLevels()/pushTuner() after each period.
///   - EngineController owns the RKR instance.
class EngineController
{
public:
    explicit EngineController(RKR& engine);
    ~EngineController() = default;

    // Non-copyable, non-movable (owns references to engine internals)
    EngineController(const EngineController&) = delete;
    EngineController& operator=(const EngineController&) = delete;

    // ─── Parameter Access (GUI thread) ─────────────────────────────

    /// Set an effect parameter. Thread-safe (queued for RT thread).
    void setEffectParameter(int effectIndex, int paramId, int value);

    /// Get an effect parameter. Reads current engine state.
    [[nodiscard]] int getEffectParameter(int effectIndex, int paramId) const;

    /// Set effect preset.
    void setEffectPreset(int effectIndex, int preset);

    /// Get effect preset.
    [[nodiscard]] int getEffectPreset(int effectIndex) const;

    // ─── Effect Chain (GUI thread) ──────────────────────────────────

    /// Set the effect processing order (10 slots).
    void setEffectOrder(std::span<const int> order);

    /// Get the current effect order.
    [[nodiscard]] std::array<int, kMaxEffectSlots> getEffectOrder() const;

    /// Enable/disable an effect in the chain.
    void setEffectEnabled(int effectIndex, bool enabled);

    /// Check if an effect is enabled.
    [[nodiscard]] bool isEffectEnabled(int effectIndex) const;

    // ─── Presets / Banks (GUI thread) ───────────────────────────────

    void loadPreset(int bankSlot);
    void savePreset(int bankSlot);
    void newPreset();
    [[nodiscard]] std::string getPresetName(int bankSlot) const;

    // ─── Global Controls (GUI thread) ───────────────────────────────

    void setMasterVolume(int value);
    [[nodiscard]] int getMasterVolume() const;

    void setInputGain(int value);
    [[nodiscard]] int getInputGain() const;

    void setBypass(bool bypass);
    [[nodiscard]] bool isBypassed() const;

    // ─── Real-Time Telemetry (GUI thread reads, RT thread writes) ──

    /// Poll the latest audio levels from the RT thread.
    [[nodiscard]] bool pollLevels(AudioLevels& out);

    /// Poll the latest tuner data.
    [[nodiscard]] bool pollTuner(TunerData& out);

    /// Poll looper status.
    [[nodiscard]] bool pollLooper(LooperStatus& out);

    /// Poll tap tempo status.
    [[nodiscard]] bool pollTapTempo(TapTempoStatus& out);

    /// Poll chord recognition.
    [[nodiscard]] bool pollChord(ChordInfo& out);

    // ─── RT Thread Interface (called from JACK callback) ───────────

    /// Push audio levels snapshot. Called once per JACK period.
    void pushLevels(const AudioLevels& levels);

    /// Push tuner data. Called when tuner is active.
    void pushTuner(const TunerData& data);

    /// Push looper status.
    void pushLooper(const LooperStatus& status);

    /// Push tap tempo status.
    void pushTapTempo(const TapTempoStatus& status);

    /// Push chord info.
    void pushChord(const ChordInfo& info);

    // ─── Direct Engine Access (escape hatch during migration) ──────

    /// Direct access to the underlying engine.
    /// Use sparingly — prefer the typed API above.
    [[nodiscard]] RKR& engine() noexcept { return m_engine; }
    [[nodiscard]] const RKR& engine() const noexcept { return m_engine; }

private:
    RKR& m_engine;

    // Ring buffers: RT thread → GUI thread (power-of-2 sizes)
    RingBuffer<AudioLevels, 16>    m_levels_rb;
    RingBuffer<TunerData, 16>      m_tuner_rb;
    RingBuffer<LooperStatus, 8>    m_looper_rb;
    RingBuffer<TapTempoStatus, 8>  m_tap_rb;
    RingBuffer<ChordInfo, 8>       m_chord_rb;
};
