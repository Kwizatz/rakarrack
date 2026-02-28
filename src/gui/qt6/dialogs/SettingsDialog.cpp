/*
  rakarrack - guitar multi-effects processor
  SPDX-License-Identifier: GPL-2.0-only

  Qt6 GUI — Settings Dialog implementation
*/

#include "SettingsDialog.hpp"
#include "EngineController.hpp"
#include "AppConfig.hpp"
#include "global.hpp"

#include <QCheckBox>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFileDialog>
#include <QFormLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QTabWidget>
#include <QVBoxLayout>

SettingsDialog::SettingsDialog(EngineController& engine, QWidget* parent)
    : QDialog(parent)
    , m_engine(engine)
{
    setupUi();
    loadFromEngine();
}

// ---------------------------------------------------------------------------
// UI Setup
// ---------------------------------------------------------------------------

void SettingsDialog::setupUi()
{
    setWindowTitle(tr("Settings"));
    resize(640, 528);
    setMinimumSize(560, 480);

    auto* mainLayout = new QVBoxLayout(this);

    m_tabs = new QTabWidget(this);
    m_tabs->addTab(createAppearanceTab(), tr("Appearance"));
    m_tabs->addTab(createAudioTab(),      tr("Audio"));
    m_tabs->addTab(createMidiTab(),       tr("MIDI"));
    m_tabs->addTab(createJackTab(),       tr("JACK"));
    m_tabs->addTab(createMiscTab(),       tr("Miscellaneous"));
    m_tabs->addTab(createBankPathsTab(),  tr("Bank Paths"));
    mainLayout->addWidget(m_tabs, 1);

    // Button box
    auto* buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::accepted, this, &SettingsDialog::onAccept);
    connect(buttons, &QDialogButtonBox::rejected, this, &SettingsDialog::onReject);
    mainLayout->addWidget(buttons);
}

// ---------------------------------------------------------------------------
// Appearance Tab
// ---------------------------------------------------------------------------

QWidget* SettingsDialog::createAppearanceTab()
{
    auto* page = new QWidget();
    auto* layout = new QFormLayout(page);

    m_fontCombo = new QComboBox(page);
    m_fontCombo->setEditable(false);
    // Populate with common families — user can extend via system fonts
    m_fontCombo->addItems({QStringLiteral("Default"),
                           QStringLiteral("Sans"), QStringLiteral("Serif"),
                           QStringLiteral("Monospace")});
    layout->addRow(tr("Font:"), m_fontCombo);

    m_fontSizeSpinner = new QSpinBox(page);
    m_fontSizeSpinner->setRange(-5, 10);
    m_fontSizeSpinner->setSuffix(tr(" pt offset"));
    layout->addRow(tr("Font Size:"), m_fontSizeSpinner);

    m_hideUnused = new QCheckBox(tr("Hide inactive effect widgets"), page);
    layout->addRow(m_hideUnused);

    // Background image
    auto* bgGroup = new QGroupBox(tr("Background Image"), page);
    auto* bgLayout = new QHBoxLayout(bgGroup);
    m_enableBackground = new QCheckBox(tr("Enable"), bgGroup);
    bgLayout->addWidget(m_enableBackground);
    m_backgroundPath = new QLineEdit(bgGroup);
    m_backgroundPath->setReadOnly(true);
    bgLayout->addWidget(m_backgroundPath, 1);
    auto* browseBtn = new QPushButton(tr("Browse..."), bgGroup);
    connect(browseBtn, &QPushButton::clicked, this, [this]
    {
        QString path = QFileDialog::getOpenFileName(
            this, tr("Select Background Image"), QString(),
            tr("PNG Images (*.png);;All Files (*)"));
        if (!path.isEmpty())
            m_backgroundPath->setText(path);
    });
    bgLayout->addWidget(browseBtn);
    layout->addRow(bgGroup);

    return page;
}

// ---------------------------------------------------------------------------
// Audio Tab
// ---------------------------------------------------------------------------

QWidget* SettingsDialog::createAudioTab()
{
    auto* page = new QWidget();
    auto* layout = new QFormLayout(page);

    m_fxOnAtStart = new QCheckBox(tr("FX enabled at startup"), page);
    layout->addRow(m_fxOnAtStart);

    m_dcOffset = new QCheckBox(tr("Filter input DC offset"), page);
    layout->addRow(m_dcOffset);

    m_preserveGain = new QCheckBox(tr("Preserve gain/master on preset change"), page);
    layout->addRow(m_preserveGain);

    m_updateTapOnPreset = new QCheckBox(tr("Update TapTempo on preset change"), page);
    layout->addRow(m_updateTapOnPreset);

    // Upsampling
    auto* upGroup = new QGroupBox(tr("Upsampling"), page);
    auto* upLayout = new QFormLayout(upGroup);

    m_upsample = new QCheckBox(tr("Enable"), upGroup);
    upLayout->addRow(m_upsample);

    m_upsampleAmount = new QComboBox(upGroup);
    for (int i = 2; i <= 12; ++i)
        m_upsampleAmount->addItem(QStringLiteral("x%1").arg(i), i);
    upLayout->addRow(tr("Amount:"), m_upsampleAmount);

    static const QStringList qualityNames = {
        tr("Best"), tr("Medium"), tr("Fastest"),
        tr("Zero Order"), tr("Linear")
    };

    m_upQuality = new QComboBox(upGroup);
    m_upQuality->addItems(qualityNames);
    upLayout->addRow(tr("Up Quality:"), m_upQuality);

    m_downQuality = new QComboBox(upGroup);
    m_downQuality->addItems(qualityNames);
    upLayout->addRow(tr("Down Quality:"), m_downQuality);

    layout->addRow(upGroup);

    // Looper
    m_looperSize = new QDoubleSpinBox(page);
    m_looperSize->setRange(0.5, 30.0);
    m_looperSize->setSingleStep(0.5);
    m_looperSize->setSuffix(tr(" seconds"));
    layout->addRow(tr("Looper Size:"), m_looperSize);

    m_metroVol = new QSpinBox(page);
    m_metroVol->setRange(0, 127);
    layout->addRow(tr("Metronome Volume:"), m_metroVol);

    // Quality settings
    m_harQuality = new QComboBox(page);
    m_harQuality->addItems({QStringLiteral("4"), QStringLiteral("8"),
                            QStringLiteral("16"), QStringLiteral("32")});
    layout->addRow(tr("Harmonizer Quality:"), m_harQuality);

    m_steQuality = new QComboBox(page);
    m_steQuality->addItems({QStringLiteral("4"), QStringLiteral("8"),
                            QStringLiteral("16"), QStringLiteral("32")});
    layout->addRow(tr("Stereo Harm Quality:"), m_steQuality);

    m_vocBands = new QComboBox(page);
    m_vocBands->addItems({QStringLiteral("16"), QStringLiteral("32"),
                          QStringLiteral("64"), QStringLiteral("128"),
                          QStringLiteral("256")});
    layout->addRow(tr("Vocoder Bands:"), m_vocBands);

    m_limiterBeforeOutput = new QCheckBox(tr("Limiter before output volume"), page);
    layout->addRow(m_limiterBeforeOutput);

    m_db6Booster = new QCheckBox(tr("+6dB final limiter"), page);
    layout->addRow(m_db6Booster);

    // Tuner
    m_tunerA4 = new QDoubleSpinBox(page);
    m_tunerA4->setRange(420.0, 460.0);
    m_tunerA4->setSingleStep(1.0);
    m_tunerA4->setSuffix(tr(" Hz"));
    layout->addRow(tr("Tuner A4 Calibration:"), m_tunerA4);

    m_recNoteTrigger = new QDoubleSpinBox(page);
    m_recNoteTrigger->setRange(0.01, 0.99);
    m_recNoteTrigger->setSingleStep(0.01);
    layout->addRow(tr("Note Trigger:"), m_recNoteTrigger);

    m_recNoteOptimize = new QComboBox(page);
    m_recNoteOptimize->addItems({tr("Normal"), tr("Guitar"), tr("Voice")});
    layout->addRow(tr("Note Optimize:"), m_recNoteOptimize);

    return page;
}

// ---------------------------------------------------------------------------
// MIDI Tab
// ---------------------------------------------------------------------------

QWidget* SettingsDialog::createMidiTab()
{
    auto* page = new QWidget();
    auto* layout = new QFormLayout(page);

    m_autoConnectMidi = new QCheckBox(tr("Auto-connect MIDI In"), page);
    layout->addRow(m_autoConnectMidi);

    m_midiChannel = new QComboBox(page);
    for (int ch = 1; ch <= 16; ++ch)
        m_midiChannel->addItem(QString::number(ch), ch);
    layout->addRow(tr("Receive Channel:"), m_midiChannel);

    m_harChannel = new QComboBox(page);
    for (int ch = 1; ch <= 16; ++ch)
        m_harChannel->addItem(QString::number(ch), ch);
    layout->addRow(tr("Harmonizer Channel:"), m_harChannel);

    m_midiImplRadio = new QComboBox(page);
    m_midiImplRadio->addItems({tr("Default"), tr("MIDI Learn")});
    layout->addRow(tr("MIDI Implementation:"), m_midiImplRadio);

    m_autoAssign = new QCheckBox(tr("Auto-assign MIDI Learn"), page);
    layout->addRow(m_autoAssign);

    return page;
}

// ---------------------------------------------------------------------------
// JACK Tab
// ---------------------------------------------------------------------------

QWidget* SettingsDialog::createJackTab()
{
    auto* page = new QWidget();
    auto* layout = new QFormLayout(page);

    m_autoConnectOut = new QCheckBox(tr("Auto-connect output ports"), page);
    layout->addRow(m_autoConnectOut);

    m_autoConnectIn = new QCheckBox(tr("Auto-connect input ports"), page);
    layout->addRow(m_autoConnectIn);

    // Note: JACK port selection would require scanning JACK ports at runtime.
    // For now, provide the auto-connect toggles. Port enumeration can be
    // added in a future iteration.
    auto* note = new QLabel(
        tr("JACK port selection requires a running JACK server.\n"
           "Use auto-connect for automatic routing."),
        page);
    note->setWordWrap(true);
    layout->addRow(note);

    return page;
}

// ---------------------------------------------------------------------------
// Misc Tab
// ---------------------------------------------------------------------------

QWidget* SettingsDialog::createMiscTab()
{
    auto* page = new QWidget();
    auto* layout = new QFormLayout(page);

    m_username = new QLineEdit(page);
    layout->addRow(tr("Username:"), m_username);

    m_disableWarnings = new QCheckBox(tr("Disable warning popups"), page);
    layout->addRow(m_disableWarnings);

    m_tapTimeout = new QSpinBox(page);
    m_tapTimeout->setRange(1, 30);
    m_tapTimeout->setSuffix(tr(" seconds"));
    layout->addRow(tr("Tap Tempo Timeout:"), m_tapTimeout);

    m_enableTooltips = new QCheckBox(tr("Enable tooltips"), page);
    layout->addRow(m_enableTooltips);

    return page;
}

// ---------------------------------------------------------------------------
// Bank Paths Tab
// ---------------------------------------------------------------------------

QWidget* SettingsDialog::createBankPathsTab()
{
    auto* page = new QWidget();
    auto* layout = new QFormLayout(page);

    // Bank filename
    auto* bankRow = new QHBoxLayout();
    m_bankFilename = new QLineEdit(page);
    bankRow->addWidget(m_bankFilename, 1);
    auto* bankBrowse = new QPushButton(tr("Browse..."), page);
    connect(bankBrowse, &QPushButton::clicked, this, [this]
    {
        QString path = QFileDialog::getOpenFileName(
            this, tr("Select Bank File"), QString(),
            tr("Rakarrack Banks (*.rkrb);;All Files (*)"));
        if (!path.isEmpty())
            m_bankFilename->setText(path);
    });
    bankRow->addWidget(bankBrowse);
    layout->addRow(tr("Bank File:"), bankRow);

    // User directory
    auto* dirRow = new QHBoxLayout();
    m_userDir = new QLineEdit(page);
    dirRow->addWidget(m_userDir, 1);
    auto* dirBrowse = new QPushButton(tr("Browse..."), page);
    connect(dirBrowse, &QPushButton::clicked, this, [this]
    {
        QString dir = QFileDialog::getExistingDirectory(
            this, tr("Select User Directory"));
        if (!dir.isEmpty())
            m_userDir->setText(dir);
    });
    dirRow->addWidget(dirBrowse);
    layout->addRow(tr("User Directory:"), dirRow);

    return page;
}

// ---------------------------------------------------------------------------
// State transfer
// ---------------------------------------------------------------------------

void SettingsDialog::loadFromEngine()
{
    auto& rkr = m_engine.engine();

    // Appearance
    m_fontSizeSpinner->setValue(rkr.config.relfontsize);
    m_enableBackground->setChecked(rkr.config.EnableBackgroundImage != 0);
    m_backgroundPath->setText(
        QString::fromLatin1(rkr.config.BackgroundImage.data()));
    m_hideUnused->setChecked(rkr.deachide != 0);

    // Audio
    m_fxOnAtStart->setChecked(rkr.init_state != 0);
    m_dcOffset->setChecked(rkr.DC_Offset != 0);
    m_preserveGain->setChecked(rkr.actuvol != 0);
    m_updateTapOnPreset->setChecked(rkr.Tap_Updated != 0);
    // Upsampling amount is stored as (value-2) since range is 2-12
    if (rkr.UpAmo >= 2 && rkr.UpAmo <= 12)
        m_upsampleAmount->setCurrentIndex(rkr.UpAmo - 2);
    m_upQuality->setCurrentIndex(rkr.UpQual);
    m_downQuality->setCurrentIndex(rkr.DownQual);
    m_looperSize->setValue(static_cast<double>(rkr.looper_size));
    m_metroVol->setValue(rkr.Metro_Vol);
    // Quality combos: values are 4/8/16/32 → indices 0-3
    m_harQuality->setCurrentIndex(
        rkr.HarQual == 4 ? 0 : rkr.HarQual == 8 ? 1 : rkr.HarQual == 16 ? 2 : 3);
    m_steQuality->setCurrentIndex(
        rkr.SteQual == 4 ? 0 : rkr.SteQual == 8 ? 1 : rkr.SteQual == 16 ? 2 : 3);
    // Vocoder bands: 16/32/64/128/256 → indices 0-4
    int vocIdx = 0;
    if (rkr.VocBands == 32)  vocIdx = 1;
    if (rkr.VocBands == 64)  vocIdx = 2;
    if (rkr.VocBands == 128) vocIdx = 3;
    if (rkr.VocBands == 256) vocIdx = 4;
    m_vocBands->setCurrentIndex(vocIdx);
    m_limiterBeforeOutput->setChecked(rkr.config.flpos != 0);
    m_db6Booster->setChecked(rkr.db6booster != 0);
    m_tunerA4->setValue(440.0);  // Default A4; engine tracks via afreq_old
    m_recNoteTrigger->setValue(static_cast<double>(rkr.rtrig));
    m_recNoteOptimize->setCurrentIndex(rkr.RCOpti);

    // MIDI
    m_autoConnectMidi->setChecked(rkr.config.aconnect_MI != 0);
    m_midiImplRadio->setCurrentIndex(rkr.MIDIway);
    m_autoAssign->setChecked(rkr.autoassign != 0);

    // JACK
    m_autoConnectOut->setChecked(rkr.config.aconnect_JA != 0);
    m_autoConnectIn->setChecked(rkr.config.aconnect_JIA != 0);

    // Misc
    m_username->setText(
        QString::fromLatin1(rkr.presets.UserRealName.data()));
    m_disableWarnings->setChecked(rkr.mess_dis != 0);
    m_tapTimeout->setValue(rkr.t_timeout);
    m_enableTooltips->setChecked(rkr.ena_tool != 0);

    // Bank Paths
    m_bankFilename->setText(
        QString::fromLatin1(rkr.presets.BankFilename.data()));
    m_userDir->setText(
        QString::fromLatin1(rkr.presets.UDirFilename.data()));
}

void SettingsDialog::applyToEngine()
{
    auto& rkr = m_engine.engine();

    // Appearance
    rkr.config.relfontsize = m_fontSizeSpinner->value();
    rkr.config.EnableBackgroundImage = m_enableBackground->isChecked() ? 1 : 0;
    {
        QByteArray ba = m_backgroundPath->text().toLocal8Bit();
        snprintf(rkr.config.BackgroundImage.data(), rkr.config.BackgroundImage.size(),
                 "%s", ba.constData());
    }
    rkr.deachide = m_hideUnused->isChecked() ? 1 : 0;

    // Audio
    rkr.init_state   = m_fxOnAtStart->isChecked()      ? 1 : 0;
    rkr.DC_Offset    = m_dcOffset->isChecked()          ? 1 : 0;
    rkr.actuvol      = m_preserveGain->isChecked()      ? 1 : 0;
    rkr.Tap_Updated  = m_updateTapOnPreset->isChecked() ? 1 : 0;
    rkr.UpAmo        = m_upsampleAmount->currentData().toInt();
    rkr.UpQual       = m_upQuality->currentIndex();
    rkr.DownQual     = m_downQuality->currentIndex();
    rkr.looper_size  = static_cast<float>(m_looperSize->value());
    rkr.Metro_Vol    = m_metroVol->value();

    static constexpr int qualVals[] = {4, 8, 16, 32};
    int qi = m_harQuality->currentIndex();
    rkr.HarQual = (qi >= 0 && qi < 4) ? qualVals[qi] : 4;
    qi = m_steQuality->currentIndex();
    rkr.SteQual = (qi >= 0 && qi < 4) ? qualVals[qi] : 4;

    static constexpr int vocBandVals[] = {16, 32, 64, 128, 256};
    int vi = m_vocBands->currentIndex();
    rkr.VocBands = (vi >= 0 && vi < 5) ? vocBandVals[vi] : 32;

    rkr.config.flpos  = m_limiterBeforeOutput->isChecked() ? 1 : 0;
    rkr.db6booster    = m_db6Booster->isChecked() ? 1 : 0;

    float tunerFreq = static_cast<float>(m_tunerA4->value());
    rkr.update_freqs(tunerFreq);

    rkr.rtrig  = static_cast<float>(m_recNoteTrigger->value());
    rkr.RCOpti = m_recNoteOptimize->currentIndex();

    // MIDI
    rkr.config.aconnect_MI = m_autoConnectMidi->isChecked() ? 1 : 0;
    rkr.MIDIway    = m_midiImplRadio->currentIndex();
    rkr.autoassign = m_autoAssign->isChecked() ? 1 : 0;

    // JACK
    rkr.config.aconnect_JA  = m_autoConnectOut->isChecked() ? 1 : 0;
    rkr.config.aconnect_JIA = m_autoConnectIn->isChecked()  ? 1 : 0;

    // Misc
    {
        QByteArray ba = m_username->text().toLocal8Bit();
        snprintf(rkr.presets.UserRealName.data(), rkr.presets.UserRealName.size(),
                 "%s", ba.constData());
    }
    rkr.mess_dis  = m_disableWarnings->isChecked() ? 1 : 0;
    rkr.t_timeout = m_tapTimeout->value();
    rkr.ena_tool  = m_enableTooltips->isChecked() ? 1 : 0;

    // Bank Paths
    {
        QByteArray ba = m_bankFilename->text().toLocal8Bit();
        snprintf(rkr.presets.BankFilename.data(), rkr.presets.BankFilename.size(),
                 "%s", ba.constData());
    }
    {
        QByteArray ba = m_userDir->text().toLocal8Bit();
        snprintf(rkr.presets.UDirFilename.data(), rkr.presets.UDirFilename.size(),
                 "%s", ba.constData());
    }
}

// ---------------------------------------------------------------------------
// Slots
// ---------------------------------------------------------------------------

void SettingsDialog::onAccept()
{
    applyToEngine();
    accept();
}

void SettingsDialog::onReject()
{
    reject();
}
