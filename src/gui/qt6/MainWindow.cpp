/*
  rakarrack - guitar multi-effects processor
  SPDX-License-Identifier: GPL-2.0-only

  Qt6 GUI — Main Window implementation
*/

#include "MainWindow.hpp"
#include "EngineController.hpp"
#include "SystemTray.hpp"
#include "ThemeManager.hpp"
#include "TopBar.hpp"
#include "widgets/NodeEditor.hpp"

// Dialogs
#include "dialogs/AboutDialog.hpp"
#include "dialogs/BankDialog.hpp"
#include "dialogs/HelpBrowser.hpp"
#include "dialogs/MidiLearnDialog.hpp"
#include "dialogs/SettingsDialog.hpp"
#include "dialogs/TriggerDialog.hpp"

#include "AppConfig.hpp"
#include "global.hpp"

#include <QFileDialog>
#include <QIcon>
#include <QMenuBar>
#include <QShortcut>
#include <QStatusBar>
#include <QVBoxLayout>

MainWindow::MainWindow(EngineController& engine, QWidget* parent)
    : QMainWindow(parent)
    , m_engine(engine)
{
    // Window icon from embedded resources
    QIcon appIcon;
    appIcon.addFile(QStringLiteral(":/icons/rakarrack-32.png"),  QSize(32, 32));
    appIcon.addFile(QStringLiteral(":/icons/rakarrack-64.png"),  QSize(64, 64));
    appIcon.addFile(QStringLiteral(":/icons/rakarrack-128.png"), QSize(128, 128));
    setWindowIcon(appIcon);

    // Theme manager (stylesheet + skin + font)
    m_theme = new ThemeManager(this);
    m_theme->applyDarkTheme();
    applyThemeFromEngine();

    setupUi();
    setupShortcuts();

    // System tray icon (optional — no-op if desktop has no tray)
    m_tray = new SystemTray(m_engine, this, this);

    // Initial sync of GUI from engine state (FX On, sliders, preset info)
    m_topBar->syncFromEngine();

    // 25 ms → 40 Hz GUI refresh
    m_guiTimer = new QTimer(this);
    connect(m_guiTimer, &QTimer::timeout, this, &MainWindow::onGuiTick);
    m_guiTimer->start(25);
}

// ---------------------------------------------------------------------------
// UI Setup
// ---------------------------------------------------------------------------

void MainWindow::setupUi()
{
    setWindowTitle(QStringLiteral("Rakarrack"));
    resize(1280, 800);
    setMinimumSize(800, 520);

    setupMenuBar();

    // Global controls stay above the routing workspace. The graph itself is
    // the primary view rather than a secondary dialog.
    m_centralWidget = new QWidget(this);
    m_centralWidget->setObjectName(QStringLiteral("centralWidget"));
    auto* mainLayout = new QVBoxLayout(m_centralWidget);
    mainLayout->setContentsMargins(4, 4, 4, 4);
    mainLayout->setSpacing(4);

    // --- Top Bar (InOut + Presets + Tuner + Tap) ---
    m_topBar = new TopBar(m_engine, m_centralWidget);
    mainLayout->addWidget(m_topBar);

    m_nodeEditor = new NodeEditor(m_engine, m_centralWidget);
    mainLayout->addWidget(m_nodeEditor, 1);

    setCentralWidget(m_centralWidget);

    // Connect TopBar signals to dialog launches
    connectTopBarSignals();

    // Status bar
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::setupMenuBar()
{
    // ── File menu ──────────────────────────────────────────────────
    auto* fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(tr("&New Preset"), QKeySequence(Qt::CTRL | Qt::Key_N),
                        this, [this]
                        {
                            m_engine.newPreset();
                            syncFromEngine();
                        });
    fileMenu->addAction(tr("&Load Preset..."), QKeySequence(Qt::CTRL | Qt::Key_L),
                        this, &MainWindow::loadPreset);
    fileMenu->addAction(tr("&Save Preset..."), QKeySequence(Qt::CTRL | Qt::Key_S),
                        this, &MainWindow::savePreset);
    fileMenu->addSeparator();
    fileMenu->addAction(tr("&Quit"), QKeySequence::Quit,
                        this, &QWidget::close);

    // ── View menu ──────────────────────────────────────────────────
    auto* viewMenu = menuBar()->addMenu(tr("&View"));
    viewMenu->addAction(tr("Sync from Engine"), QKeySequence(Qt::Key_F5),
                        this, &MainWindow::syncFromEngine);

    // ── Windows menu ───────────────────────────────────────────────
    auto* windowsMenu = menuBar()->addMenu(tr("&Windows"));
    windowsMenu->addAction(tr("&Bank Manager"), QKeySequence(Qt::CTRL | Qt::Key_B),
                           this, &MainWindow::showBankDialog);
    windowsMenu->addAction(tr("&MIDI Learn"),
                           this, &MainWindow::showMidiLearnDialog);
    windowsMenu->addAction(tr("&Trigger (ACI)"),
                           this, &MainWindow::showTriggerDialog);

    // ── Settings menu ──────────────────────────────────────────────
    auto* settingsMenu = menuBar()->addMenu(tr("&Settings"));
    settingsMenu->addAction(tr("&Preferences..."),
                            QKeySequence(Qt::CTRL | Qt::Key_Comma),
                            this, &MainWindow::showSettingsDialog);

    // ── Help menu ──────────────────────────────────────────────────
    auto* helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(tr("&Help Contents"), QKeySequence::HelpContents,
                        this, &MainWindow::showHelp);
    helpMenu->addAction(tr("&License"),
                        this, &MainWindow::showLicense);
    helpMenu->addSeparator();
    helpMenu->addAction(tr("&About Rakarrack..."),
                        this, &MainWindow::showAboutDialog);
}

// ---------------------------------------------------------------------------
// Keyboard shortcuts
// ---------------------------------------------------------------------------

void MainWindow::setupShortcuts()
{
    // Ctrl+Right / Ctrl+Left — next / previous preset
    auto* scNext = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_Right), this);
    connect(scNext, &QShortcut::activated, this, &MainWindow::nextPreset);

    auto* scPrev = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_Left), this);
    connect(scPrev, &QShortcut::activated, this, &MainWindow::previousPreset);

    // Ctrl+T — toggle tuner
    auto* scTuner = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_T), this);
    connect(scTuner, &QShortcut::activated, this, [this]
    {
        auto& rkr = m_engine.engine();
        rkr.Tuner_Bypass = rkr.Tuner_Bypass ? 0 : 1;
    });
}

// ---------------------------------------------------------------------------
// Timer-driven engine polling (40 Hz)
// ---------------------------------------------------------------------------

void MainWindow::onGuiTick()
{
    // Delegate level/tuner/tap updates to the TopBar
    m_topBar->updateFromEngine();

    // Update status bar with signal presence
    AudioLevels levels;
    if (m_engine.pollLevels(levels))
    {
        statusBar()->showMessage(
            levels.have_signal
                ? tr("Signal: YES")
                : tr("Signal: no"));
    }
}

// ---------------------------------------------------------------------------
// TopBar signal wiring
// ---------------------------------------------------------------------------

void MainWindow::connectTopBarSignals()
{
    connect(m_topBar, &TopBar::newPresetRequested, this,
            [this]
            {
                m_engine.newPreset();
                syncFromEngine();
            });
    connect(m_topBar, &TopBar::bankWindowRequested,
            this, &MainWindow::showBankDialog);
    connect(m_topBar, &TopBar::loadPresetRequested,
            this, &MainWindow::loadPreset);
    connect(m_topBar, &TopBar::savePresetRequested,
            this, &MainWindow::savePreset);

    // Preset counter changed → load the selected bank slot
    connect(m_topBar, &TopBar::presetChanged, this,
            [this](int index)
            {
                auto& rkr = m_engine.engine();
                rkr.Bank_to_Preset(index - 1);  // spinbox is 1-based, engine is 0-based
                syncFromEngine();
            });
}

void MainWindow::syncFromEngine()
{
    m_topBar->syncFromEngine();
    m_nodeEditor->syncFromEngine();
}

// ---------------------------------------------------------------------------
// File actions — Load / Save preset
// ---------------------------------------------------------------------------

void MainWindow::loadPreset()
{
    QString path = QFileDialog::getOpenFileName(
        this, tr("Load Preset"), QString(),
        tr("Rakarrack Presets (*.rkr);;All Files (*)"));

    if (!path.isEmpty())
    {
        auto& rkr = m_engine.engine();
        QByteArray pathBytes = path.toLocal8Bit();
        rkr.loadfile(pathBytes.data());
        syncFromEngine();
    }
}

void MainWindow::savePreset()
{
    QString path = QFileDialog::getSaveFileName(
        this, tr("Save Preset"), QString(),
        tr("Rakarrack Presets (*.rkr);;All Files (*)"));

    if (!path.isEmpty())
    {
        auto& rkr = m_engine.engine();
        QByteArray pathBytes = path.toLocal8Bit();
        rkr.savefile(pathBytes.data());
    }
}

void MainWindow::nextPreset()
{
    auto& rkr = m_engine.engine();
    int current = rkr.presets.Selected_Preset;
    if (current < 60)
    {
        rkr.Bank_to_Preset(current + 1);
        syncFromEngine();
    }
}

void MainWindow::previousPreset()
{
    auto& rkr = m_engine.engine();
    int current = rkr.presets.Selected_Preset;
    if (current > 1)
    {
        rkr.Bank_to_Preset(current - 1);
        syncFromEngine();
    }
}

// ---------------------------------------------------------------------------
// Theme — apply font + skin from engine config
// ---------------------------------------------------------------------------

void MainWindow::applyThemeFromEngine()
{
    auto& rkr = m_engine.engine();

    // Font
    QString family;
    switch (rkr.config.font)
    {
        case 1:  family = QStringLiteral("Sans");      break;
        case 2:  family = QStringLiteral("Serif");     break;
        case 3:  family = QStringLiteral("Monospace"); break;
        default: family = QStringLiteral("Default");   break;
    }
    m_theme->applyFont(family, rkr.config.relfontsize);

    // Background skin
    if (rkr.config.EnableBackgroundImage && rkr.config.BackgroundImage[0] != '\0')
    {
        QString skinPath = QString::fromLocal8Bit(
            rkr.config.BackgroundImage.data());
        m_theme->applySkin(m_centralWidget, skinPath);
    }
}

// ---------------------------------------------------------------------------
// Dialog launchers
// ---------------------------------------------------------------------------

void MainWindow::showBankDialog()
{
    if (!m_bankDialog)
        m_bankDialog = new BankDialog(m_engine, this);

    m_bankDialog->refreshBank();
    m_bankDialog->show();
    m_bankDialog->raise();
    m_bankDialog->activateWindow();
}

void MainWindow::showSettingsDialog()
{
    SettingsDialog dlg(m_engine, this);
    dlg.exec();
    // Re-apply theme in case user changed appearance settings
    applyThemeFromEngine();
}

void MainWindow::showMidiLearnDialog()
{
    MidiLearnDialog dlg(m_engine, this);
    dlg.exec();
}

void MainWindow::showAboutDialog()
{
    AboutDialog dlg(this);
    dlg.exec();
}

void MainWindow::showHelp()
{
    if (!m_helpBrowser)
        m_helpBrowser = new HelpBrowser(this);
    m_helpBrowser->showHelp();
}

void MainWindow::showLicense()
{
    if (!m_helpBrowser)
        m_helpBrowser = new HelpBrowser(this);
    m_helpBrowser->showLicense();
}

void MainWindow::showTriggerDialog()
{
    if (!m_triggerDialog)
        m_triggerDialog = new TriggerDialog(m_engine, this);

    m_triggerDialog->show();
    m_triggerDialog->raise();
    m_triggerDialog->activateWindow();
}
