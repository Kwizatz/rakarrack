/*
  rakarrack - guitar multi-effects processor
  SPDX-License-Identifier: GPL-2.0-only

  Qt6 GUI — MIDI Learn Dialog implementation
*/

#include "MidiLearnDialog.hpp"
#include "EngineController.hpp"
#include "global.hpp"

#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>

MidiLearnDialog::MidiLearnDialog(EngineController& engine, QWidget* parent)
    : QDialog(parent)
    , m_engine(engine)
{
    setupUi();
    populateParameterList();
}

// ---------------------------------------------------------------------------
// UI Setup
// ---------------------------------------------------------------------------

void MidiLearnDialog::setupUi()
{
    setWindowTitle(tr("MIDI Learn"));
    resize(700, 500);
    setMinimumSize(600, 400);

    auto* mainLayout = new QVBoxLayout(this);

    // Filter row
    auto* filterRow = new QHBoxLayout();
    filterRow->addWidget(new QLabel(tr("Show:"), this));
    m_filterCombo = new QComboBox(this);
    m_filterCombo->addItems({tr("All Parameters"), tr("Current Chain Only")});
    connect(m_filterCombo, &QComboBox::currentIndexChanged,
            this, &MidiLearnDialog::onFilterChanged);
    filterRow->addWidget(m_filterCombo);
    filterRow->addStretch();
    mainLayout->addLayout(filterRow);

    // Main content: param list | assignment display | controls
    auto* contentLayout = new QHBoxLayout();

    // ---- Parameter list ----
    auto* leftPanel = new QVBoxLayout();
    leftPanel->addWidget(new QLabel(tr("Parameters"), this));
    m_paramList = new QListWidget(this);
    connect(m_paramList, &QListWidget::currentRowChanged,
            this, [this](int) { onParameterSelected(); });
    leftPanel->addWidget(m_paramList, 1);
    contentLayout->addLayout(leftPanel, 2);

    // ---- Assignment display ----
    auto* assignGroup = new QGroupBox(tr("CC Assignments"), this);
    auto* assignGrid = new QGridLayout(assignGroup);
    for (int i = 0; i < kMaxAssignSlots; ++i)
    {
        m_assignLabels[static_cast<std::size_t>(i)] = new QLabel(
            QStringLiteral("---"), assignGroup);
        m_assignLabels[static_cast<std::size_t>(i)]->setFrameStyle(
            QFrame::Panel | QFrame::Sunken);
        m_assignLabels[static_cast<std::size_t>(i)]->setAlignment(Qt::AlignCenter);
        m_assignLabels[static_cast<std::size_t>(i)]->setMinimumWidth(60);
        assignGrid->addWidget(
            m_assignLabels[static_cast<std::size_t>(i)],
            i / 4, i % 4);
    }
    contentLayout->addWidget(assignGroup, 1);

    // ---- Controls column ----
    auto* controlPanel = new QVBoxLayout();

    // CC number
    auto* ccRow = new QHBoxLayout();
    ccRow->addWidget(new QLabel(tr("CC#:"), this));
    m_ccSpinner = new QSpinBox(this);
    m_ccSpinner->setRange(1, 127);
    m_ccSpinner->setValue(1);
    ccRow->addWidget(m_ccSpinner);
    controlPanel->addLayout(ccRow);

    m_getMidiBtn = new QPushButton(tr("Get MIDI Message"), this);
    connect(m_getMidiBtn, &QPushButton::clicked,
            this, &MidiLearnDialog::onGetMidiMessage);
    controlPanel->addWidget(m_getMidiBtn);

    m_cancelBtn = new QPushButton(tr("Cancel Capture"), this);
    m_cancelBtn->setEnabled(false);
    connect(m_cancelBtn, &QPushButton::clicked,
            this, &MidiLearnDialog::onCancelCapture);
    controlPanel->addWidget(m_cancelBtn);

    controlPanel->addSpacing(12);

    auto* assignBtn = new QPushButton(tr("Assign"), this);
    connect(assignBtn, &QPushButton::clicked,
            this, &MidiLearnDialog::onAssign);
    controlPanel->addWidget(assignBtn);

    auto* assignAllBtn = new QPushButton(tr("Assign to All"), this);
    connect(assignAllBtn, &QPushButton::clicked,
            this, &MidiLearnDialog::onAssignToAll);
    controlPanel->addWidget(assignAllBtn);

    controlPanel->addSpacing(12);

    auto* clearBtn = new QPushButton(tr("Clear Assignments"), this);
    connect(clearBtn, &QPushButton::clicked,
            this, &MidiLearnDialog::onClearAssignments);
    controlPanel->addWidget(clearBtn);

    auto* clearPresetBtn = new QPushButton(tr("Clear Preset"), this);
    connect(clearPresetBtn, &QPushButton::clicked,
            this, &MidiLearnDialog::onClearPreset);
    controlPanel->addWidget(clearPresetBtn);

    controlPanel->addStretch();

    auto* closeBtn = new QPushButton(tr("Close"), this);
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
    controlPanel->addWidget(closeBtn);

    contentLayout->addLayout(controlPanel);

    mainLayout->addLayout(contentLayout, 1);
}

// ---------------------------------------------------------------------------
// Population
// ---------------------------------------------------------------------------

void MidiLearnDialog::populateParameterList()
{
    m_paramList->clear();
    auto& rkr = m_engine.engine();

    for (int i = 0; i < rkr.NumParams; ++i)
    {
        const auto& param = rkr.efx_params[i];

        // If filtering to chain only, check if the effect is in the active chain
        if (m_currentFilter == 1)
        {
            bool inChain = false;
            auto order = m_engine.getEffectOrder();
            for (int s = 0; s < kMaxEffectSlots; ++s)
            {
                if (order[static_cast<std::size_t>(s)] == param.Effect)
                {
                    inChain = true;
                    break;
                }
            }
            if (!inChain)
                continue;
        }

        auto* item = new QListWidgetItem(
            QString::fromLatin1(param.Nom.data()));
        item->setData(Qt::UserRole, param.Ato);  // unique param ID
        m_paramList->addItem(item);
    }
}

void MidiLearnDialog::displayAssignments()
{
    // Clear all labels
    for (auto* label : m_assignLabels)
        label->setText(QStringLiteral("---"));

    auto* item = m_paramList->currentItem();
    if (!item)
        return;

    int paramId = item->data(Qt::UserRole).toInt();
    auto& rkr = m_engine.engine();

    // Scan all 128 CCs for assignments to this parameter
    int slot = 0;
    for (int cc = 0; cc < 128 && slot < kMaxAssignSlots; ++cc)
    {
        for (int a = 0; a < 20; ++a)
        {
            if (rkr.XUserMIDI[static_cast<std::size_t>(cc)]
                             [static_cast<std::size_t>(a)] == paramId)
            {
                m_assignLabels[static_cast<std::size_t>(slot)]->setText(
                    tr("CC %1").arg(cc));
                ++slot;
                break;  // only show each CC once for this param
            }
        }
    }
}

// ---------------------------------------------------------------------------
// Slots
// ---------------------------------------------------------------------------

void MidiLearnDialog::onParameterSelected()
{
    displayAssignments();
}

void MidiLearnDialog::onAssign()
{
    auto* item = m_paramList->currentItem();
    if (!item)
        return;

    int paramId = item->data(Qt::UserRole).toInt();
    int cc = m_ccSpinner->value();
    auto& rkr = m_engine.engine();

    // Find an empty slot in XUserMIDI[cc]
    for (int a = 0; a < 20; ++a)
    {
        if (rkr.XUserMIDI[static_cast<std::size_t>(cc)]
                         [static_cast<std::size_t>(a)] == 0)
        {
            rkr.XUserMIDI[static_cast<std::size_t>(cc)]
                         [static_cast<std::size_t>(a)] = paramId;
            break;
        }
    }

    displayAssignments();
}

void MidiLearnDialog::onAssignToAll()
{
    auto* item = m_paramList->currentItem();
    if (!item)
        return;

    int paramId = item->data(Qt::UserRole).toInt();
    int cc = m_ccSpinner->value();
    auto& rkr = m_engine.engine();

    // Assign in current preset
    for (int a = 0; a < 20; ++a)
    {
        if (rkr.XUserMIDI[static_cast<std::size_t>(cc)]
                         [static_cast<std::size_t>(a)] == 0)
        {
            rkr.XUserMIDI[static_cast<std::size_t>(cc)]
                         [static_cast<std::size_t>(a)] = paramId;
            break;
        }
    }

    // Copy to all 60 bank presets
    for (int b = 0; b < 60; ++b)
    {
        for (int a = 0; a < 20; ++a)
        {
            if (rkr.presets.Bank[b].XUserMIDI[static_cast<std::size_t>(cc)]
                                             [static_cast<std::size_t>(a)] == 0)
            {
                rkr.presets.Bank[b].XUserMIDI[static_cast<std::size_t>(cc)]
                                             [static_cast<std::size_t>(a)] = paramId;
                break;
            }
        }
    }

    displayAssignments();
}

void MidiLearnDialog::onGetMidiMessage()
{
    // Tell engine to capture next MIDI CC
    m_engine.engine().RControl = 1;
    m_getMidiBtn->setEnabled(false);
    m_cancelBtn->setEnabled(true);
}

void MidiLearnDialog::onCancelCapture()
{
    m_engine.engine().RControl = 0;
    m_getMidiBtn->setEnabled(true);
    m_cancelBtn->setEnabled(false);
}

void MidiLearnDialog::onClearAssignments()
{
    auto* item = m_paramList->currentItem();
    if (!item)
        return;

    int paramId = item->data(Qt::UserRole).toInt();
    auto& rkr = m_engine.engine();

    // Remove this param from all CC assignments
    for (int cc = 0; cc < 128; ++cc)
    {
        for (int a = 0; a < 20; ++a)
        {
            if (rkr.XUserMIDI[static_cast<std::size_t>(cc)]
                             [static_cast<std::size_t>(a)] == paramId)
            {
                rkr.XUserMIDI[static_cast<std::size_t>(cc)]
                             [static_cast<std::size_t>(a)] = 0;
            }
        }
    }

    displayAssignments();
}

void MidiLearnDialog::onClearPreset()
{
    auto answer = QMessageBox::question(
        this, tr("Clear Preset MIDI"),
        tr("Clear all MIDI CC assignments for the current preset?"),
        QMessageBox::Yes | QMessageBox::No);

    if (answer == QMessageBox::Yes)
    {
        auto& rkr = m_engine.engine();
        for (auto& cc : rkr.XUserMIDI)
            cc.fill(0);
        displayAssignments();
    }
}

void MidiLearnDialog::onFilterChanged(int index)
{
    m_currentFilter = index;
    populateParameterList();
}
