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
