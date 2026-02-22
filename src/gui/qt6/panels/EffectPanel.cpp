/*
  rakarrack - guitar multi-effects processor
  SPDX-License-Identifier: GPL-2.0-only

  Qt6 GUI — EffectPanel base class implementation
*/

#include "EffectPanel.hpp"
#include "EngineController.hpp"

// Panel subclass headers for the factory
#include "EQPanel.hpp"
#include "DynamicsPanels.hpp"
#include "DistortionPanels.hpp"
#include "ModulationPanels.hpp"
#include "DelayReverbPanels.hpp"
#include "PitchPanels.hpp"
#include "FilterPanels.hpp"
#include "MiscPanels.hpp"

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
// Factory — returns the appropriate panel subclass for each effect type.
// ---------------------------------------------------------------------------

std::unique_ptr<EffectPanel> EffectPanel::create(
    int effectType, EngineController& engine, QWidget* parent)
{
    switch (effectType)
    {
    // ── EQ ──────────────────────────────────────────────────────────
    case  0: return std::make_unique<EQPanel>(engine, 0, parent);   // EQ1
    case  9: return std::make_unique<EQPanel>(engine, 9, parent);   // EQ2
    case 12: return std::make_unique<EQPanel>(engine, 12, parent);  // Cabinet

    // ── Dynamics ────────────────────────────────────────────────────
    case  1: return std::make_unique<CompressorPanel>(engine, parent);
    case 16: return std::make_unique<GatePanel>(engine, parent);
    case 25: return std::make_unique<ExpanderPanel>(engine, parent);
    case 43: return std::make_unique<CompBandPanel>(engine, parent);

    // ── Distortion ──────────────────────────────────────────────────
    case  2: return std::make_unique<DistorsionPanel>(engine, 2, parent);  // Distorsion
    case  3: return std::make_unique<DistorsionPanel>(engine, 3, parent);  // Overdrive
    case 17: return std::make_unique<NewDistPanel>(engine, parent);
    case 19: return std::make_unique<ValvePanel>(engine, parent);
    case 23: return std::make_unique<MBDistPanel>(engine, parent);
    case 39: return std::make_unique<StompBoxPanel>(engine, parent);

    // ── Modulation ──────────────────────────────────────────────────
    case  5: return std::make_unique<ChorusPanel>(engine, 5, parent);   // Chorus
    case  7: return std::make_unique<ChorusPanel>(engine, 7, parent);   // Flanger
    case  6: return std::make_unique<PhaserPanel>(engine, parent);
    case 11: return std::make_unique<AlienwahPanel>(engine, parent);
    case 13: return std::make_unique<PanPanel>(engine, parent);
    case 18: return std::make_unique<APhaserPanel>(engine, parent);
    case 20: return std::make_unique<DualFlangePanel>(engine, parent);
    case 27: return std::make_unique<SynthfilterPanel>(engine, parent);
    case 44: return std::make_unique<OpticaltremPanel>(engine, parent);
    case 45: return std::make_unique<VibePanel>(engine, parent);
    case 46: return std::make_unique<InfinityPanel>(engine, parent);

    // ── Delay & Reverb ──────────────────────────────────────────────
    case  4: return std::make_unique<EchoPanel>(engine, parent);
    case  8: return std::make_unique<ReverbPanel>(engine, parent);
    case 15: return std::make_unique<MusicDelayPanel>(engine, parent);
    case 24: return std::make_unique<ArpiePanel>(engine, parent);
    case 29: return std::make_unique<ConvolotronPanel>(engine, parent);
    case 32: return std::make_unique<RBEchoPanel>(engine, parent);
    case 40: return std::make_unique<ReverbtronPanel>(engine, parent);
    case 41: return std::make_unique<EchotronPanel>(engine, parent);

    // ── Pitch ───────────────────────────────────────────────────────
    case 14: return std::make_unique<HarmonizerPanel>(engine, parent);
    case 21: return std::make_unique<RingPanel>(engine, parent);
    case 38: return std::make_unique<ShifterPanel>(engine, parent);
    case 42: return std::make_unique<StereoHarmPanel>(engine, parent);

    // ── Filters ─────────────────────────────────────────────────────
    case 10: return std::make_unique<WahWahPanel>(engine, parent);
    case 31: return std::make_unique<RyanWahPanel>(engine, parent);
    case 33: return std::make_unique<CoilCrafterPanel>(engine, parent);
    case 34: return std::make_unique<ShelfBoostPanel>(engine, parent);

    // ── Misc ────────────────────────────────────────────────────────
    case 22: return std::make_unique<ExciterPanel>(engine, parent);
    case 26: return std::make_unique<ShufflePanel>(engine, parent);
    case 28: return std::make_unique<MBVvolPanel>(engine, parent);
    case 30: return std::make_unique<LooperPanel>(engine, parent);
    case 35: return std::make_unique<VocoderPanel>(engine, parent);
    case 36: return std::make_unique<SustainerPanel>(engine, parent);
    case 37: return std::make_unique<SequencePanel>(engine, parent);

    default:
        break;
    }

    // Fallback: generic placeholder (should be unreachable for valid types).
    auto panel = std::make_unique<EffectPanel>(engine, effectType, parent);
    auto* label = new QLabel(
        QStringLiteral("Effect %1 — unknown type").arg(effectType),
        panel.get());
    label->setAlignment(Qt::AlignCenter);
    label->setWordWrap(true);
    panel->bodyLayout()->addWidget(label);
    return panel;
}
