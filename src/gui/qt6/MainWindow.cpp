/*
  rakarrack - guitar multi-effects processor
  SPDX-License-Identifier: GPL-2.0-only

  Qt6 GUI — Main Window implementation
*/

#include "MainWindow.hpp"
#include "EngineController.hpp"

#include <QLabel>
#include <QMenuBar>
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

void MainWindow::setupUi()
{
    setWindowTitle(QStringLiteral("Rakarrack"));
    resize(1024, 768);

    // Central placeholder — will be replaced with effect panel stack
    auto* central = new QWidget(this);
    auto* layout  = new QVBoxLayout(central);

    auto* placeholder = new QLabel(
        QStringLiteral("Rakarrack Qt6 — Engine connected.\n"
                       "Effect panels coming in Phase 2–4."),
        central);
    placeholder->setAlignment(Qt::AlignCenter);
    layout->addWidget(placeholder);

    setCentralWidget(central);

    // Menu bar skeleton
    auto* fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(tr("&New Preset"), this, [this] { m_engine.newPreset(); });
    fileMenu->addSeparator();
    fileMenu->addAction(tr("&Quit"), QKeySequence::Quit, this, &QWidget::close);

    setupStatusBar();
}

void MainWindow::setupStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::onGuiTick()
{
    // Poll audio levels from the RT thread via lock-free ring buffer
    AudioLevels levels;
    if (m_engine.pollLevels(levels))
    {
        // TODO Phase 2: Update VU meter widgets
        // For now, update the status bar with CPU load
        statusBar()->showMessage(
            tr("CPU: %1%  Signal: %2")
                .arg(levels.cpu_load, 0, 'f', 1)
                .arg(levels.have_signal ? tr("YES") : tr("no")));
    }

    // Poll tuner
    TunerData tuner;
    if (m_engine.pollTuner(tuner))
    {
        // TODO Phase 2: Update TunerDisplay widget
    }

    // Poll tap tempo
    TapTempoStatus tap;
    if (m_engine.pollTapTempo(tap))
    {
        // TODO Phase 3: Update tempo display
    }
}
