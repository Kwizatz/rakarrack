/*
  rakarrack - guitar multi-effects processor
  SPDX-License-Identifier: GPL-2.0-only

  Qt6 GUI — Main Window
*/

#pragma once

#include <QMainWindow>
#include <QTimer>

#include <array>
#include <memory>

class EngineController;
class TopBar;
class EffectSlotBar;
class EffectPanel;
class QStackedWidget;

/// Number of effect processing slots.
inline constexpr int kMainEffectSlots = 10;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(EngineController& engine, QWidget* parent = nullptr);
    ~MainWindow() override = default;

private Q_SLOTS:
    /// Called at 40 Hz to poll engine state (levels, tuner, MIDI, etc.)
    void onGuiTick();

    /// Called when the user selects an effect slot.
    void onSlotSelected(int slotIndex);

private:
    void setupUi();
    void setupMenuBar();
    void createEffectPanels();

    EngineController& m_engine;
    QTimer*           m_guiTimer{nullptr};

    // Composed widgets
    TopBar*           m_topBar{nullptr};
    EffectSlotBar*    m_slotBar{nullptr};
    QStackedWidget*   m_panelStack{nullptr};

    // Effect panels (one per slot)
    std::array<EffectPanel*, kMainEffectSlots> m_effectPanels{};
};
