/*
  rakarrack - guitar multi-effects processor
  SPDX-License-Identifier: GPL-2.0-only

  Qt6 GUI — EffectSlotBar

  Horizontal row of 10 toggle buttons, one per effect slot in the chain.
  Each button shows the effect name and active/bypass state.  Clicking a
  slot selects it for detailed editing in the panel area below.
*/

#pragma once

#include <QWidget>

#include <array>

class EngineController;
class QPushButton;

/// Number of effect processing slots.
inline constexpr int kEffectSlots = 10;

class EffectSlotBar : public QWidget
{
    Q_OBJECT

public:
    explicit EffectSlotBar(EngineController& engine, QWidget* parent = nullptr);
    ~EffectSlotBar() override = default;

    /// Highlight the selected slot (0-9).
    void setSelectedSlot(int slot);

    /// Update button labels and active states from engine.
    void syncFromEngine();

Q_SIGNALS:
    /// Emitted when user clicks a slot button.
    void slotSelected(int slotIndex);

private:
    EngineController& m_engine;
    std::array<QPushButton*, kEffectSlots> m_slotButtons{};
    int m_selectedSlot{0};
};
