/*
  rakarrack - guitar multi-effects processor
  SPDX-License-Identifier: GPL-2.0-only

  Qt6 GUI — EffectPanel base class implementation
*/

#include "EffectPanel.hpp"
#include "EngineController.hpp"

#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

// ---------------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------------

EffectPanel::EffectPanel(EngineController& engine, int effectIndex,
                         QWidget* parent)
    : QWidget(parent)
    , m_engine(engine)
    , m_effectIndex(effectIndex)
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(2, 2, 2, 2);
    m_mainLayout->setSpacing(2);

    setupHeader();

    // Body area for subclass controls
    m_bodyLayout = new QVBoxLayout;
    m_bodyLayout->setContentsMargins(0, 0, 0, 0);
    m_bodyLayout->setSpacing(2);
    m_mainLayout->addLayout(m_bodyLayout);
    m_mainLayout->addStretch(1);
}

// ---------------------------------------------------------------------------
// Header — On button + Preset combo
// ---------------------------------------------------------------------------

void EffectPanel::setupHeader()
{
    auto* header = new QHBoxLayout;
    header->setSpacing(4);

    // On/Off toggle
    m_onButton = new QPushButton(tr("On"), this);
    m_onButton->setCheckable(true);
    m_onButton->setFixedWidth(40);
    connect(m_onButton, &QPushButton::toggled, this,
            [this](bool checked)
            {
                m_engine.setEffectEnabled(m_effectIndex, checked);
                emit bypassChanged(m_effectIndex, checked);
            });

    // Preset selector
    m_presetCombo = new QComboBox(this);
    m_presetCombo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(m_presetCombo, &QComboBox::currentIndexChanged, this,
            [this](int index)
            {
                if (index >= 0)
                {
                    m_engine.setEffectPreset(m_effectIndex, index);
                    syncFromEngine();
                }
            });

    header->addWidget(m_onButton);
    header->addWidget(m_presetCombo);

    m_mainLayout->addLayout(header);
}

// ---------------------------------------------------------------------------
// Body layout accessor
// ---------------------------------------------------------------------------

QVBoxLayout* EffectPanel::bodyLayout()
{
    return m_bodyLayout;
}

// ---------------------------------------------------------------------------
// Default sync — subclasses override
// ---------------------------------------------------------------------------

void EffectPanel::syncFromEngine()
{
    // Update on/off button
    const bool active = m_engine.isEffectEnabled(m_effectIndex);
    m_onButton->blockSignals(true);
    m_onButton->setChecked(active);
    m_onButton->blockSignals(false);
}

void EffectPanel::syncToEngine()
{
    // Default: nothing — subclasses push slider values
}

// ---------------------------------------------------------------------------
// Factory — returns generic placeholder for unimplemented effects
// ---------------------------------------------------------------------------

std::unique_ptr<EffectPanel> EffectPanel::create(
    int effectType, EngineController& engine, QWidget* parent)
{
    // TODO Phase 4: switch on effectType to return concrete panel subclasses
    // For now, return a generic panel with just the header (On + Preset).
    auto panel = std::make_unique<EffectPanel>(engine, effectType, parent);
    auto* label = new QLabel(
        QStringLiteral("Effect %1 — panel coming in Phase 4").arg(effectType),
        panel.get());
    label->setAlignment(Qt::AlignCenter);
    label->setWordWrap(true);
    panel->bodyLayout()->addWidget(label);
    return panel;
}
