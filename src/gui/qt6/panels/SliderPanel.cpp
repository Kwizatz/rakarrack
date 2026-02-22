/*
  rakarrack - guitar multi-effects processor
  SPDX-License-Identifier: GPL-2.0-only

  Qt6 GUI — SliderPanel implementation
*/

#include "SliderPanel.hpp"
#include "EngineController.hpp"
#include "../widgets/MidiSlider.hpp"

#include <QCheckBox>
#include <QComboBox>
#include <QGridLayout>
#include <QLabel>

// ---------------------------------------------------------------------------
// Construction — build a 2-column grid of controls from the descriptor table.
// ---------------------------------------------------------------------------

SliderPanel::SliderPanel(EngineController& engine, int effectIndex,
                         std::span<const ParamDesc> params,
                         QWidget* parent)
    : EffectPanel(engine, effectIndex, parent)
{
    auto* grid = new QGridLayout;
    grid->setSpacing(4);

    // Layout: 2 logical columns, each = [Label | Control]
    // Grid columns: 0=label0  1=ctrl0  2=label1  3=ctrl1
    constexpr int kColumns = 2;
    int row = 0;
    int col = 0;

    for (const auto& p : params)
    {
        QWidget* control = nullptr;

        switch (p.control)
        {
        case ParamDesc::Slider:
        {
            auto* slider = new MidiSlider(Qt::Horizontal, this);
            slider->setRange(p.minVal, p.maxVal);
            connect(slider, &QSlider::valueChanged, this,
                    [this, id = p.id](int val)
                    { m_engine.setEffectParameter(m_effectIndex, id, val); });
            control = slider;
            break;
        }
        case ParamDesc::Toggle:
        {
            auto* cb = new QCheckBox(this);
            connect(cb, &QCheckBox::toggled, this,
                    [this, id = p.id](bool checked)
                    {
                        m_engine.setEffectParameter(
                            m_effectIndex, id, checked ? 1 : 0);
                    });
            control = cb;
            break;
        }
        case ParamDesc::Choice:
        {
            auto* combo = new QComboBox(this);
            const int count = p.maxVal - p.minVal + 1;
            if (p.choices)
            {
                for (int i = 0; i < count; ++i)
                    combo->addItem(QString::fromLatin1(p.choices[i]));
            }
            else
            {
                for (int i = p.minVal; i <= p.maxVal; ++i)
                    combo->addItem(QString::number(i));
            }
            connect(combo, &QComboBox::currentIndexChanged, this,
                    [this, id = p.id, base = p.minVal](int idx)
                    { m_engine.setEffectParameter(m_effectIndex, id, base + idx); });
            control = combo;
            break;
        }
        }

        auto* label = new QLabel(tr(p.label), this);
        grid->addWidget(label,   row, col * 2);
        grid->addWidget(control, row, col * 2 + 1);

        m_bindings.push_back({p.id, p.minVal, p.control, control});

        if (++col >= kColumns)
        {
            col = 0;
            ++row;
        }
    }

    // Let control columns stretch, labels stay compact.
    for (int c = 0; c < kColumns; ++c)
        grid->setColumnStretch(c * 2 + 1, 1);

    bodyLayout()->addLayout(grid);
}

// ---------------------------------------------------------------------------
// Sync — pull values from engine → controls.
// ---------------------------------------------------------------------------

void SliderPanel::syncFromEngine()
{
    EffectPanel::syncFromEngine();   // On button

    for (const auto& b : m_bindings)
    {
        const int val = m_engine.getEffectParameter(m_effectIndex, b.paramId);

        switch (b.type)
        {
        case ParamDesc::Slider:
        {
            auto* slider = static_cast<MidiSlider*>(b.widget);
            slider->blockSignals(true);
            slider->setValue(val);
            slider->blockSignals(false);
            break;
        }
        case ParamDesc::Toggle:
        {
            auto* cb = static_cast<QCheckBox*>(b.widget);
            cb->blockSignals(true);
            cb->setChecked(val != 0);
            cb->blockSignals(false);
            break;
        }
        case ParamDesc::Choice:
        {
            auto* combo = static_cast<QComboBox*>(b.widget);
            combo->blockSignals(true);
            const int idx = val - b.minVal;
            if (idx >= 0 && idx < combo->count())
                combo->setCurrentIndex(idx);
            combo->blockSignals(false);
            break;
        }
        }
    }
}

// ---------------------------------------------------------------------------
// Sync — push control values → engine.
// ---------------------------------------------------------------------------

void SliderPanel::syncToEngine()
{
    for (const auto& b : m_bindings)
    {
        switch (b.type)
        {
        case ParamDesc::Slider:
        {
            auto* slider = static_cast<MidiSlider*>(b.widget);
            m_engine.setEffectParameter(
                m_effectIndex, b.paramId, slider->value());
            break;
        }
        case ParamDesc::Toggle:
        {
            auto* cb = static_cast<QCheckBox*>(b.widget);
            m_engine.setEffectParameter(
                m_effectIndex, b.paramId, cb->isChecked() ? 1 : 0);
            break;
        }
        case ParamDesc::Choice:
        {
            auto* combo = static_cast<QComboBox*>(b.widget);
            m_engine.setEffectParameter(
                m_effectIndex, b.paramId,
                b.minVal + combo->currentIndex());
            break;
        }
        }
    }
}
