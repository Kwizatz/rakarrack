/*
  rakarrack - guitar multi-effects processor
  SPDX-License-Identifier: GPL-2.0-only

  Qt6 GUI — EffectSlotBar implementation
*/

#include "EffectSlotBar.hpp"
#include "EngineController.hpp"
#include "dsp_constants.hpp"

#include <QHBoxLayout>
#include <QPushButton>

// Style for the selected slot button
static constexpr auto kSelectedStyle =
    "QPushButton { background-color: #3daee9; color: white; font-weight: bold; }";
static constexpr auto kActiveStyle =
    "QPushButton { background-color: #27ae60; color: white; }";
static constexpr auto kBypassedStyle =
    "QPushButton { background-color: #555; color: #aaa; }";

/// Unicode "bullet" used as a small LED indicator on slot buttons.
static constexpr auto kLedOn  = "\xE2\x97\x89";   // ◉
static constexpr auto kLedOff = "\xE2\x97\x8B";   // ○

// ---------------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------------

EffectSlotBar::EffectSlotBar(EngineController& engine, QWidget* parent)
    : QWidget(parent)
    , m_engine(engine)
{
    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(2, 2, 2, 2);
    layout->setSpacing(2);

    for (int i = 0; i < kEffectSlots; ++i)
    {
        auto* btn = new QPushButton(QStringLiteral("Slot %1").arg(i + 1), this);
        btn->setCheckable(false);
        btn->setFixedHeight(28);
        btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

        connect(btn, &QPushButton::clicked, this,
                [this, i]
                {
                    setSelectedSlot(i);
                    emit slotSelected(i);
                });

        m_slotButtons[static_cast<std::size_t>(i)] = btn;
        layout->addWidget(btn);
    }

    setFixedHeight(36);
    syncFromEngine();
}

// ---------------------------------------------------------------------------
// Selection
// ---------------------------------------------------------------------------

void EffectSlotBar::setSelectedSlot(int slot)
{
    if (slot < 0 || slot >= kEffectSlots)
        return;

    m_selectedSlot = slot;

    // Re-apply styles: selected gets highlight, others get active/bypass color
    auto order = m_engine.getEffectOrder();
    for (int i = 0; i < kEffectSlots; ++i)
    {
        auto* btn = m_slotButtons[static_cast<std::size_t>(i)];
        int effectType = order[static_cast<std::size_t>(i)];

        // Hide buttons for unused (empty) slots
        if (effectType == EMPTY_SLOT)
        {
            btn->hide();
            continue;
        }
        btn->show();

        bool active = m_engine.isEffectEnabled(effectType);
        std::string name = m_engine.getEffectTypeName(effectType);

        // Build label:  ◉ EffectName ON  or  ○ EffectName OFF
        QString label = QStringLiteral("%1 %2 %3")
            .arg(active ? QString::fromUtf8(kLedOn) : QString::fromUtf8(kLedOff))
            .arg(QString::fromStdString(name))
            .arg(active ? tr("ON") : tr("OFF"));
        btn->setText(label);

        if (i == m_selectedSlot)
        {
            btn->setStyleSheet(QString::fromUtf8(kSelectedStyle));
        }
        else
        {
            btn->setStyleSheet(active ? QString::fromUtf8(kActiveStyle)
                                      : QString::fromUtf8(kBypassedStyle));
        }
    }
}

// ---------------------------------------------------------------------------
// Sync from engine (effect order + bypass states)
// ---------------------------------------------------------------------------

void EffectSlotBar::syncFromEngine()
{
    // setSelectedSlot already updates labels, LEDs, and colors for all buttons.
    setSelectedSlot(m_selectedSlot);
}
