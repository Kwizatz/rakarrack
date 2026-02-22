/*
  rakarrack - guitar multi-effects processor
  SPDX-License-Identifier: GPL-2.0-only

  Qt6 GUI — EQ Panel (used for EQ1 index 0, EQ2 index 9, Cabinet index 12).

  Custom layout: volume slider + 10-band parametric table.
  Each band row has Type (combo), Freq, Gain, Q, Stages controls.
*/

#pragma once

#include "EffectPanel.hpp"
#include "ParamDesc.hpp"

#include <array>
#include <vector>

class MidiSlider;
class QComboBox;

class EQPanel : public EffectPanel
{
public:
    explicit EQPanel(EngineController& engine, int effectIndex,
                     QWidget* parent = nullptr);

    void syncFromEngine() override;
    void syncToEngine() override;

private:
    static constexpr int kNumBands = 10;

    /// Volume slider (param 0).
    MidiSlider* m_volumeSlider = nullptr;

    /// Per-band controls.
    struct BandRow
    {
        QComboBox*  type   = nullptr;   // param 10 + band*5 + 0
        MidiSlider* freq   = nullptr;   // param 10 + band*5 + 1
        MidiSlider* gain   = nullptr;   // param 10 + band*5 + 2
        MidiSlider* q      = nullptr;   // param 10 + band*5 + 3
        MidiSlider* stages = nullptr;   // param 10 + band*5 + 4
    };

    std::array<BandRow, kNumBands> m_bands{};
};
