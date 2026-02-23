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
class ThemeManager;
class SystemTray;
class QStackedWidget;

// Dialogs
class BankDialog;
class OrderDialog;
class SettingsDialog;
class MidiLearnDialog;
class HelpBrowser;
class TriggerDialog;

/// Number of effect processing slots.
inline constexpr int kMainEffectSlots = 10;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(EngineController& engine, QWidget* parent = nullptr);
    ~MainWindow() override = default;

    /// Access the theme manager (used by SettingsDialog).
    [[nodiscard]] ThemeManager* themeManager() const { return m_theme; }

private Q_SLOTS:
    /// Called at 40 Hz to poll engine state (levels, tuner, MIDI, etc.)
    void onGuiTick();

    /// Called when the user selects an effect slot.
    void onSlotSelected(int slotIndex);

    // File actions
    void loadPreset();
    void savePreset();
    void nextPreset();
    void previousPreset();

    // Dialog launchers
    void showBankDialog();
    void showOrderDialog();
    void showSettingsDialog();
    void showMidiLearnDialog();
    void showAboutDialog();
    void showHelp();
    void showLicense();
    void showTriggerDialog();

private:
    void setupUi();
    void setupMenuBar();
    void setupShortcuts();
    void createEffectPanels();
    void connectTopBarSignals();
    void applyThemeFromEngine();

    EngineController& m_engine;
    QTimer*           m_guiTimer{nullptr};

    // Theme & tray
    ThemeManager*     m_theme{nullptr};
    SystemTray*       m_tray{nullptr};

    // Composed widgets
    TopBar*           m_topBar{nullptr};
    EffectSlotBar*    m_slotBar{nullptr};
    QStackedWidget*   m_panelStack{nullptr};
    QWidget*          m_centralWidget{nullptr};

    // Effect panels (one per slot)
    std::array<EffectPanel*, kMainEffectSlots> m_effectPanels{};

    // Persistent dialogs (created on first use)
    BankDialog*      m_bankDialog{nullptr};
    HelpBrowser*     m_helpBrowser{nullptr};
    TriggerDialog*   m_triggerDialog{nullptr};
};
