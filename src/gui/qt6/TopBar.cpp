/*
  rakarrack - guitar multi-effects processor
  SPDX-License-Identifier: GPL-2.0-only

  Qt6 GUI — TopBar widget implementation
*/

#include "TopBar.hpp"
#include "EngineController.hpp"
#include "widgets/MidiSlider.hpp"
#include "widgets/VUMeter.hpp"
#include "widgets/TunerDisplay.hpp"

#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>

// ---------------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------------

TopBar::TopBar(EngineController& engine, QWidget* parent)
    : QWidget(parent)
    , m_engine(engine)
{
    auto* mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(2, 2, 2, 2);
    mainLayout->setSpacing(4);

    // --- InOut section ---
    auto* inoutBox = new QGroupBox(tr("In/Out"), this);
    setupInOutSection(inoutBox);
    mainLayout->addWidget(inoutBox);

    // --- Presets section ---
    auto* presetsBox = new QGroupBox(tr("Presets"), this);
    presetsBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    setupPresetsSection(presetsBox);
    mainLayout->addWidget(presetsBox, 1);

    // --- Tuner section ---
    auto* tunerBox = new QGroupBox(tr("Tuner"), this);
    setupTunerSection(tunerBox);
    mainLayout->addWidget(tunerBox);

    // --- Tap Tempo section ---
    auto* tapBox = new QGroupBox(tr("Tap"), this);
    setupTapSection(tapBox);
    mainLayout->addWidget(tapBox);

    // --- CPU label ---
    m_cpuLabel = new QLabel(QStringLiteral("CPU: --"), this);
    m_cpuLabel->setFixedWidth(70);
    mainLayout->addWidget(m_cpuLabel);
}

// ---------------------------------------------------------------------------
// InOut section:  FX On, Balance, Input Level, VU L/R, Output Level, VU L/R
// ---------------------------------------------------------------------------

void TopBar::setupInOutSection(QWidget* container)
{
    auto* layout = new QHBoxLayout(container);
    layout->setSpacing(2);
    layout->setContentsMargins(4, 16, 4, 4);

    // FX On toggle
    m_fxOnButton = new QPushButton(tr("FX On"), container);
    m_fxOnButton->setCheckable(true);
    m_fxOnButton->setChecked(true);
    m_fxOnButton->setShortcut(QKeySequence(Qt::Key_R));
    connect(m_fxOnButton, &QPushButton::toggled, this,
            [this](bool on) { m_engine.setBypass(!on); });
    layout->addWidget(m_fxOnButton);

    // Balance (dry/wet)
    m_balanceSlider = new MidiSlider(Qt::Vertical, container);
    m_balanceSlider->setRange(0, 127);
    m_balanceSlider->setValue(64);
    m_balanceSlider->setToolTip(tr("Dry/Wet Balance"));
    m_balanceSlider->setFixedWidth(22);
    connect(m_balanceSlider, &QSlider::valueChanged, this,
            [this](int v) { m_engine.setBalance(v); });
    layout->addWidget(m_balanceSlider);

    // Input gain
    m_inputSlider = new MidiSlider(Qt::Vertical, container);
    m_inputSlider->setRange(0, 127);
    m_inputSlider->setValue(64);
    m_inputSlider->setToolTip(tr("Input Gain"));
    m_inputSlider->setFixedWidth(22);
    connect(m_inputSlider, &QSlider::valueChanged, this,
            [this](int v) { m_engine.setInputGain(v); });
    layout->addWidget(m_inputSlider);

    // Input VU L/R
    m_inputVuL = new VUMeter(container);
    m_inputVuR = new VUMeter(container);
    layout->addWidget(m_inputVuL);
    layout->addWidget(m_inputVuR);

    // Output volume
    m_outputSlider = new MidiSlider(Qt::Vertical, container);
    m_outputSlider->setRange(0, 127);
    m_outputSlider->setValue(64);
    m_outputSlider->setToolTip(tr("Master Volume"));
    m_outputSlider->setFixedWidth(22);
    connect(m_outputSlider, &QSlider::valueChanged, this,
            [this](int v) { m_engine.setMasterVolume(v); });
    layout->addWidget(m_outputSlider);

    // Output VU L/R
    m_outputVuL = new VUMeter(container);
    m_outputVuR = new VUMeter(container);
    layout->addWidget(m_outputVuL);
    layout->addWidget(m_outputVuR);
}

// ---------------------------------------------------------------------------
// Presets section:  Name, Counter, New/Load/Save/Bank/Order buttons
// ---------------------------------------------------------------------------

void TopBar::setupPresetsSection(QWidget* container)
{
    auto* layout = new QVBoxLayout(container);
    layout->setSpacing(2);
    layout->setContentsMargins(4, 16, 4, 4);

    // Row 1: buttons
    auto* btnRow = new QHBoxLayout;
    btnRow->setSpacing(2);

    auto addBtn = [&](const QString& text, auto signal) {
        auto* btn = new QPushButton(text, container);
        btn->setFixedHeight(22);
        connect(btn, &QPushButton::clicked, this, signal);
        btnRow->addWidget(btn);
        return btn;
    };

    addBtn(tr("New"),   &TopBar::newPresetRequested);
    addBtn(tr("Load"),  &TopBar::loadPresetRequested);
    addBtn(tr("Save"),  &TopBar::savePresetRequested);
    addBtn(tr("Bank"),  &TopBar::bankWindowRequested);
    addBtn(tr("Order"), &TopBar::orderWindowRequested);
    layout->addLayout(btnRow);

    // Row 2: preset name
    m_presetName = new QLineEdit(container);
    m_presetName->setPlaceholderText(tr("Preset Name"));
    m_presetName->setMaxLength(22);
    layout->addWidget(m_presetName);

    // Row 3: counter + author
    auto* row3 = new QHBoxLayout;
    m_presetCounter = new QSpinBox(container);
    m_presetCounter->setRange(1, 60);
    m_presetCounter->setFixedWidth(60);
    connect(m_presetCounter, &QSpinBox::valueChanged, this,
            &TopBar::presetChanged);
    row3->addWidget(m_presetCounter);

    m_authorLabel = new QLabel(container);
    m_authorLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    row3->addWidget(m_authorLabel);
    layout->addLayout(row3);
}

// ---------------------------------------------------------------------------
// Tuner section:  On toggle, cents bar, note name, frequency
// ---------------------------------------------------------------------------

void TopBar::setupTunerSection(QWidget* container)
{
    auto* layout = new QVBoxLayout(container);
    layout->setSpacing(2);
    layout->setContentsMargins(4, 16, 4, 4);

    auto* topRow = new QHBoxLayout;
    m_tunerOnButton = new QPushButton(tr("On"), container);
    m_tunerOnButton->setCheckable(true);
    m_tunerOnButton->setFixedWidth(36);
    topRow->addWidget(m_tunerOnButton);

    m_tunerBar = new TunerDisplay(container);
    topRow->addWidget(m_tunerBar, 1);
    layout->addLayout(topRow);

    auto* infoRow = new QHBoxLayout;
    m_noteLabel = new QLabel(QStringLiteral("--"), container);
    m_noteLabel->setAlignment(Qt::AlignCenter);
    m_noteLabel->setFixedWidth(40);
    QFont noteFont = m_noteLabel->font();
    noteFont.setPointSize(14);
    noteFont.setBold(true);
    m_noteLabel->setFont(noteFont);

    m_freqLabel = new QLabel(QStringLiteral("-- Hz"), container);
    m_freqLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    infoRow->addWidget(m_noteLabel);
    infoRow->addStretch();
    infoRow->addWidget(m_freqLabel);
    layout->addLayout(infoRow);
}

// ---------------------------------------------------------------------------
// Tap Tempo section:  Tap button + BPM display
// ---------------------------------------------------------------------------

void TopBar::setupTapSection(QWidget* container)
{
    auto* layout = new QVBoxLayout(container);
    layout->setSpacing(4);
    layout->setContentsMargins(4, 16, 4, 4);

    m_tapButton = new QPushButton(tr("TAP"), container);
    m_tapButton->setFixedHeight(30);
    connect(m_tapButton, &QPushButton::clicked, this,
            [this] { m_engine.tapTempo(); });
    layout->addWidget(m_tapButton);

    m_bpmLabel = new QLabel(QStringLiteral("-- BPM"), container);
    m_bpmLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(m_bpmLabel);
}

// ---------------------------------------------------------------------------
// Timer-driven update (called from MainWindow::onGuiTick)
// ---------------------------------------------------------------------------

void TopBar::updateFromEngine()
{
    // VU levels
    AudioLevels levels;
    if (m_engine.pollLevels(levels))
    {
        m_inputVuL->setLevel(levels.input_left);
        m_inputVuR->setLevel(levels.input_right);
        m_outputVuL->setLevel(levels.output_left);
        m_outputVuR->setLevel(levels.output_right);

        m_cpuLabel->setText(QStringLiteral("CPU: %1%")
                                .arg(levels.cpu_load, 0, 'f', 1));
    }

    // Tuner
    TunerData tuner;
    if (m_engine.pollTuner(tuner))
    {
        m_tunerBar->setTunerData(tuner.cents,
                                  QString::fromStdString(tuner.note_name),
                                  tuner.actual_freq);
        m_noteLabel->setText(QString::fromStdString(tuner.note_name));
        m_freqLabel->setText(QStringLiteral("%1 Hz")
                                 .arg(static_cast<double>(tuner.actual_freq), 0, 'f', 1));

        // Tint note label red when |cents| > 5
        if (std::abs(tuner.cents) > 5.0F)
            m_noteLabel->setStyleSheet(QStringLiteral("color: red;"));
        else
            m_noteLabel->setStyleSheet(QStringLiteral("color: green;"));
    }

    // Tap tempo
    TapTempoStatus tap;
    if (m_engine.pollTapTempo(tap))
    {
        if (tap.tempo_bpm > 0.0F)
            m_bpmLabel->setText(QStringLiteral("%1 BPM")
                                    .arg(static_cast<double>(tap.tempo_bpm), 0, 'f', 1));
        else
            m_bpmLabel->setText(QStringLiteral("-- BPM"));
    }
}

// ---------------------------------------------------------------------------
// Full sync (preset loaded)
// ---------------------------------------------------------------------------

void TopBar::syncFromEngine()
{
    // TODO Phase 5: sync preset name, author, counter, volume sliders
    // from engine state
}
