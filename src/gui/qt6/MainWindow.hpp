/*
  rakarrack - guitar multi-effects processor
  SPDX-License-Identifier: GPL-2.0-only

  Qt6 GUI — Main Window
*/

#pragma once

#include <QMainWindow>
#include <QTimer>

class EngineController;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(EngineController& engine, QWidget* parent = nullptr);
    ~MainWindow() override = default;

private Q_SLOTS:
    /// Called at 40 Hz to poll engine state (levels, tuner, MIDI, etc.)
    void onGuiTick();

private:
    void setupUi();
    void setupStatusBar();

    EngineController& m_engine;
    QTimer*           m_guiTimer{nullptr};
};
