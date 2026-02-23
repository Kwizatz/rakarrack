/*
  rakarrack - guitar multi-effects processor
  SPDX-License-Identifier: GPL-2.0-only

  Qt6 GUI — Main Window implementation
*/

#include "MainWindow.hpp"
#include "EngineController.hpp"
#include "EffectSlotBar.hpp"
#include "TopBar.hpp"
#include "panels/EffectPanel.hpp"

// Dialogs
#include "dialogs/AboutDialog.hpp"
#include "dialogs/BankDialog.hpp"
#include "dialogs/HelpBrowser.hpp"
#include "dialogs/MidiLearnDialog.hpp"
#include "dialogs/OrderDialog.hpp"
#include "dialogs/SettingsDialog.hpp"
#include "dialogs/TriggerDialog.hpp"

#include <QMenuBar>
#include <QStackedWidget>
#include <QStatusBar>
#include <QVBoxLayout>

MainWindow::MainWindow(EngineController& engine, QWidget* parent)
    : QMainWindow(parent)
    , m_engine(engine)
{
    setupUi();

    // 25 ms → 40 Hz GUI refresh, matching the FLTK tick() rate
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
    resize(1024, 768);
    setMinimumSize(640, 480);

    setupMenuBar();

    // Central widget with vertical layout:
    //   TopBar  |  EffectSlotBar  |  EffectPanel stack
    auto* central = new QWidget(this);
    auto* mainLayout = new QVBoxLayout(central);
    mainLayout->setContentsMargins(4, 4, 4, 4);
    mainLayout->setSpacing(4);

    // --- Top Bar (InOut + Presets + Tuner + Tap) ---
    m_topBar = new TopBar(m_engine, central);
    mainLayout->addWidget(m_topBar);

    // --- Effect Slot Bar (10 buttons) ---
    m_slotBar = new EffectSlotBar(m_engine, central);
    connect(m_slotBar, &EffectSlotBar::slotSelected,
            this, &MainWindow::onSlotSelected);
    mainLayout->addWidget(m_slotBar);

    // --- Effect Panel Stack ---
    m_panelStack = new QStackedWidget(central);
    createEffectPanels();
    mainLayout->addWidget(m_panelStack, 1);  // stretch factor 1

    setCentralWidget(central);

    // Connect TopBar signals to dialog launches
    connectTopBarSignals();

    // Status bar
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::setupMenuBar()
{
    auto* fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(tr("&New Preset"), QKeySequence(Qt::CTRL | Qt::Key_N),
                        this, [this] { m_engine.newPreset(); });
    fileMenu->addSeparator();
    fileMenu->addAction(tr("&Quit"), QKeySequence::Quit,
                        this, &QWidget::close);

    auto* viewMenu = menuBar()->addMenu(tr("&View"));
    viewMenu->addAction(tr("Sync from Engine"), QKeySequence(Qt::Key_F5),
                        this, [this]
                        {
                            m_slotBar->syncFromEngine();
                            for (auto* panel : m_effectPanels)
                            {
                                if (panel)
                                    panel->syncFromEngine();
                            }
                        });

    auto* windowsMenu = menuBar()->addMenu(tr("&Windows"));
    windowsMenu->addAction(tr("&Bank Manager"), QKeySequence(Qt::CTRL | Qt::Key_B),
                           this, &MainWindow::showBankDialog);
    windowsMenu->addAction(tr("Effect &Order"), QKeySequence(Qt::CTRL | Qt::Key_O),
                           this, &MainWindow::showOrderDialog);
    windowsMenu->addAction(tr("&MIDI Learn"),
                           this, &MainWindow::showMidiLearnDialog);
    windowsMenu->addAction(tr("&Trigger (ACI)"),
                           this, &MainWindow::showTriggerDialog);

    auto* settingsMenu = menuBar()->addMenu(tr("&Settings"));
    settingsMenu->addAction(tr("&Preferences..."),
                            QKeySequence(Qt::CTRL | Qt::Key_Comma),
                            this, &MainWindow::showSettingsDialog);

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
// Effect Panels — one per slot
// ---------------------------------------------------------------------------

void MainWindow::createEffectPanels()
{
    auto order = m_engine.getEffectOrder();

    for (int i = 0; i < kMainEffectSlots; ++i)
    {
        int effectType = order[static_cast<std::size_t>(i)];
        auto panel = EffectPanel::create(effectType, m_engine);

        m_effectPanels[static_cast<std::size_t>(i)] = panel.get();
        m_panelStack->addWidget(panel.release());  // stack takes ownership
    }
}

// ---------------------------------------------------------------------------
// Slot selection
// ---------------------------------------------------------------------------

void MainWindow::onSlotSelected(int slotIndex)
{
    if (slotIndex >= 0 && slotIndex < kMainEffectSlots)
    {
        m_panelStack->setCurrentIndex(slotIndex);

        auto* panel = m_effectPanels[static_cast<std::size_t>(slotIndex)];
        if (panel)
            panel->syncFromEngine();
    }
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
    connect(m_topBar, &TopBar::bankWindowRequested,
            this, &MainWindow::showBankDialog);
    connect(m_topBar, &TopBar::orderWindowRequested,
            this, &MainWindow::showOrderDialog);
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

void MainWindow::showOrderDialog()
{
    // OrderDialog is modal — each open gets a fresh copy with current state
    OrderDialog dlg(m_engine, this);
    if (dlg.exec() == QDialog::Accepted)
    {
        // Rebuild panels if order changed
        createEffectPanels();
        m_slotBar->syncFromEngine();
    }
}

void MainWindow::showSettingsDialog()
{
    SettingsDialog dlg(m_engine, this);
    dlg.exec();
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
