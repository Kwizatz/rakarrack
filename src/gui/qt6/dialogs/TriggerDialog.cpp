/*
  rakarrack - guitar multi-effects processor
  SPDX-License-Identifier: GPL-2.0-only

  Qt6 GUI — Trigger (ACI) Dialog implementation
*/

#include "TriggerDialog.hpp"
#include "EngineController.hpp"
#include "global.hpp"
#include "../widgets/VUMeter.hpp"

#include <QComboBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QSpinBox>
#include <QVBoxLayout>

TriggerDialog::TriggerDialog(EngineController& engine, QWidget* parent)
    : QDialog(parent)
    , m_engine(engine)
{
    setupUi();
    loadFromEngine();

    // Enable ACI processing while dialog is open
    m_engine.engine().ACI_Bypass = 1;
}

TriggerDialog::~TriggerDialog()
{
    // Disable ACI processing when dialog closes
    m_engine.engine().ACI_Bypass = 0;
}

// ---------------------------------------------------------------------------
// UI Setup
// ---------------------------------------------------------------------------

void TriggerDialog::setupUi()
{
    setWindowTitle(tr("Analog Control (ACI)"));
    setFixedSize(280, 320);

    auto* mainLayout = new QHBoxLayout(this);

    // VU meter on the left
    m_vuMeter = new VUMeter(this);
    m_vuMeter->setFixedWidth(24);
    mainLayout->addWidget(m_vuMeter);

    // Controls on the right
    auto* controlLayout = new QFormLayout();
    controlLayout->setSpacing(8);

    // Title
    auto* title = new QLabel(tr("Analog Control"), this);
    auto titleFont = title->font();
    titleFont.setBold(true);
    title->setFont(titleFont);
    controlLayout->addRow(title);

    // Source
    m_source = new QComboBox(this);
    m_source->addItems({tr("Aux"), tr("Left"), tr("Right")});
    connect(m_source, &QComboBox::currentIndexChanged,
            this, &TriggerDialog::onSourceChanged);
    controlLayout->addRow(tr("Source:"), m_source);

    // Gain (1-127)
    m_gain = new QSlider(Qt::Horizontal, this);
    m_gain->setRange(1, 127);
    connect(m_gain, &QSlider::valueChanged,
            this, &TriggerDialog::onGainChanged);
    controlLayout->addRow(tr("Gain:"), m_gain);

    // Threshold (-70 to 20)
    m_threshold = new QSlider(Qt::Horizontal, this);
    m_threshold->setRange(-70, 20);
    connect(m_threshold, &QSlider::valueChanged,
            this, &TriggerDialog::onThresholdChanged);
    controlLayout->addRow(tr("Threshold:"), m_threshold);

    // MIDI Control (1-127)
    m_midiControl = new QSpinBox(this);
    m_midiControl->setRange(1, 127);
    connect(m_midiControl, &QSpinBox::valueChanged,
            this, &TriggerDialog::onMidiControlChanged);
    controlLayout->addRow(tr("MIDI CC:"), m_midiControl);

    // Minimum (0-127)
    m_minimum = new QSlider(Qt::Horizontal, this);
    m_minimum->setRange(0, 127);
    connect(m_minimum, &QSlider::valueChanged,
            this, &TriggerDialog::onMinimumChanged);
    controlLayout->addRow(tr("Minimum:"), m_minimum);

    // Maximum (0-127)
    m_maximum = new QSlider(Qt::Horizontal, this);
    m_maximum->setRange(0, 127);
    connect(m_maximum, &QSlider::valueChanged,
            this, &TriggerDialog::onMaximumChanged);
    controlLayout->addRow(tr("Maximum:"), m_maximum);

    // Close button
    auto* closeBtn = new QPushButton(tr("Close"), this);
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
    controlLayout->addRow(closeBtn);

    mainLayout->addLayout(controlLayout, 1);
}

void TriggerDialog::loadFromEngine()
{
    auto& rkr = m_engine.engine();
    m_source->setCurrentIndex(rkr.Aux_Source);
    m_gain->setValue(rkr.Aux_Gain);
    m_threshold->setValue(rkr.Aux_Threshold);
    m_midiControl->setValue(rkr.Aux_MIDI);
    m_minimum->setValue(rkr.Aux_Minimum);
    m_maximum->setValue(rkr.Aux_Maximum);
}

// ---------------------------------------------------------------------------
// Slots — write directly to engine (real-time)
// ---------------------------------------------------------------------------

void TriggerDialog::onSourceChanged(int index)
{
    m_engine.engine().Aux_Source = index;
}

void TriggerDialog::onGainChanged(int value)
{
    m_engine.engine().Aux_Gain = value;
}

void TriggerDialog::onThresholdChanged(int value)
{
    m_engine.engine().Aux_Threshold = value;
}

void TriggerDialog::onMidiControlChanged(int value)
{
    m_engine.engine().Aux_MIDI = value;
}

void TriggerDialog::onMinimumChanged(int value)
{
    m_engine.engine().Aux_Minimum = value;
}

void TriggerDialog::onMaximumChanged(int value)
{
    m_engine.engine().Aux_Maximum = value;
}
