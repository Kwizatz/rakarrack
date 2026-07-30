/*
  rakarrack - guitar multi-effects processor
  SPDX-License-Identifier: GPL-2.0-only

  Qt6 GUI — EQ Panel implementation
*/

#include "EQPanel.hpp"
#include "EngineController.hpp"
#include "../widgets/MidiSlider.hpp"

#include <QComboBox>
#include <QGridLayout>
#include <QLabel>

// ---------------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------------

EQPanel::EQPanel(EngineController& engine, int effectIndex, QWidget* parent)
    : EffectPanel(engine, effectIndex, parent)
{
    auto* mainGrid = new QGridLayout;
    mainGrid->setSpacing(4);

    // ── Volume slider (param 0) ────────────────────────────────────
    auto* volLabel = new QLabel(tr("Volume"), this);
    m_volumeSlider = new MidiSlider(Qt::Horizontal, this);
    m_volumeSlider->setRange(0, 127);
    connect(m_volumeSlider, &QSlider::valueChanged, this,
            [this](int val)
            { setParam(0, val); });

    mainGrid->addWidget(volLabel,        0, 0);
    mainGrid->addWidget(m_volumeSlider,  0, 1, 1, 5);

    // ── Band header row ────────────────────────────────────────────
    const char* headers[] = {"Band", "Type", "Freq", "Gain", "Q", "Stages"};
    for (int c = 0; c < 6; ++c)
    {
        auto* h = new QLabel(tr(headers[c]), this);
        h->setAlignment(Qt::AlignCenter);
        mainGrid->addWidget(h, 1, c);
    }

    // ── Band rows ──────────────────────────────────────────────────
    for (int b = 0; b < kNumBands; ++b)
    {
        const int baseParam = 10 + b * 5;
        const int gridRow   = b + 2;   // offset by header rows

        // Band number label
        auto* numLabel = new QLabel(QString::number(b + 1), this);
        numLabel->setAlignment(Qt::AlignCenter);
        mainGrid->addWidget(numLabel, gridRow, 0);

        // Type combo (param baseParam + 0, range 0-9)
        auto* typeCombo = new QComboBox(this);
        for (int t = 0; t < 10; ++t)
            typeCombo->addItem(QString::fromLatin1(kEQFilterTypeNames[t]));
        connect(typeCombo, &QComboBox::currentIndexChanged, this,
                [this, id = baseParam](int idx)
                { setParam(id, idx); });
        mainGrid->addWidget(typeCombo, gridRow, 1);

        // Freq slider (param baseParam + 1)
        auto* freqSlider = new MidiSlider(Qt::Horizontal, this);
        freqSlider->setRange(20, 20000);
        connect(freqSlider, &QSlider::valueChanged, this,
                [this, id = baseParam + 1](int val)
                { setParam(id, val); });
        mainGrid->addWidget(freqSlider, gridRow, 2);

        // Gain slider (param baseParam + 2, 0-127 where 64 = 0 dB)
        auto* gainSlider = new MidiSlider(Qt::Horizontal, this);
        gainSlider->setRange(0, 127);
        connect(gainSlider, &QSlider::valueChanged, this,
                [this, id = baseParam + 2](int val)
                { setParam(id, val); });
        mainGrid->addWidget(gainSlider, gridRow, 3);

        // Q slider (param baseParam + 3, 0-127 where 64 = center)
        auto* qSlider = new MidiSlider(Qt::Horizontal, this);
        qSlider->setRange(0, 127);
        connect(qSlider, &QSlider::valueChanged, this,
                [this, id = baseParam + 3](int val)
                { setParam(id, val); });
        mainGrid->addWidget(qSlider, gridRow, 4);

        // Stages slider (param baseParam + 4, 0-4)
        auto* stagesSlider = new MidiSlider(Qt::Horizontal, this);
        stagesSlider->setRange(0, 4);
        connect(stagesSlider, &QSlider::valueChanged, this,
                [this, id = baseParam + 4](int val)
                { setParam(id, val); });
        mainGrid->addWidget(stagesSlider, gridRow, 5);

        m_bands[static_cast<std::size_t>(b)] = {
            typeCombo, freqSlider, gainSlider, qSlider, stagesSlider};
    }

    // Stretch data columns
    for (int c = 1; c <= 5; ++c)
        mainGrid->setColumnStretch(c, 1);

    bodyLayout()->addLayout(mainGrid);
}

// ---------------------------------------------------------------------------
// Sync from engine
// ---------------------------------------------------------------------------

void EQPanel::syncFromEngine()
{
    EffectPanel::syncFromEngine();

    // Volume
    {
        const int val = getParam(0);
        m_volumeSlider->blockSignals(true);
        m_volumeSlider->setValue(val);
        m_volumeSlider->blockSignals(false);
    }

    // Bands
    for (int b = 0; b < kNumBands; ++b)
    {
        const int base = 10 + b * 5;
        auto& br = m_bands[static_cast<std::size_t>(b)];

        auto getP = [&](int offset) {
            return getParam(base + offset);
        };

        br.type->blockSignals(true);
        br.type->setCurrentIndex(getP(0));
        br.type->blockSignals(false);

        auto syncSlider = [](MidiSlider* s, int val)
        {
            s->blockSignals(true);
            s->setValue(val);
            s->blockSignals(false);
        };

        syncSlider(br.freq,   getP(1));
        syncSlider(br.gain,   getP(2));
        syncSlider(br.q,      getP(3));
        syncSlider(br.stages, getP(4));
    }
}

// ---------------------------------------------------------------------------
// Sync to engine
// ---------------------------------------------------------------------------

void EQPanel::syncToEngine()
{
    setParam(0, m_volumeSlider->value());

    for (int b = 0; b < kNumBands; ++b)
    {
        const int base = 10 + b * 5;
        const auto& br = m_bands[static_cast<std::size_t>(b)];

        setParam(base + 0,
                                    br.type->currentIndex());
        setParam(base + 1, br.freq->value());
        setParam(base + 2, br.gain->value());
        setParam(base + 3, br.q->value());
        setParam(base + 4,
                                    br.stages->value());
    }
}
