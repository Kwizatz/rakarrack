/*
  rakarrack - guitar multi-effects processor
  SPDX-License-Identifier: GPL-2.0-only

  Qt6 GUI — Settings Dialog

  Tabbed dialog with: Appearance, Audio, MIDI, JACK, Misc, Bank tabs.
  Replaces Fl_Preferences with QSettings-compatible approach.
*/

#pragma once

#include <QDialog>

class EngineController;
class QCheckBox;
class QComboBox;
class QDoubleSpinBox;
class QLineEdit;
class QSpinBox;
class QTabWidget;

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(EngineController& engine, QWidget* parent = nullptr);
    ~SettingsDialog() override = default;

private Q_SLOTS:
    void onAccept();
    void onReject();

private:
    void setupUi();
    QWidget* createAppearanceTab();
    QWidget* createAudioTab();
    QWidget* createMidiTab();
    QWidget* createJackTab();
    QWidget* createMiscTab();
    QWidget* createBankPathsTab();

    /// Load current engine/config values into controls.
    void loadFromEngine();

    /// Write control values back to engine/config.
    void applyToEngine();

    EngineController& m_engine;
    QTabWidget* m_tabs{nullptr};

    // ---- Appearance tab ----
    QComboBox*  m_fontCombo{nullptr};
    QSpinBox*   m_fontSizeSpinner{nullptr};
    QCheckBox*  m_enableBackground{nullptr};
    QLineEdit*  m_backgroundPath{nullptr};
    QCheckBox*  m_hideUnused{nullptr};

    // ---- Audio tab ----
    QCheckBox*      m_fxOnAtStart{nullptr};
    QCheckBox*      m_dcOffset{nullptr};
    QCheckBox*      m_preserveGain{nullptr};
    QCheckBox*      m_updateTapOnPreset{nullptr};
    QCheckBox*      m_upsample{nullptr};
    QComboBox*      m_upsampleAmount{nullptr};
    QComboBox*      m_upQuality{nullptr};
    QComboBox*      m_downQuality{nullptr};
    QDoubleSpinBox* m_looperSize{nullptr};
    QSpinBox*       m_metroVol{nullptr};
    QComboBox*      m_harQuality{nullptr};
    QComboBox*      m_steQuality{nullptr};
    QComboBox*      m_vocBands{nullptr};
    QCheckBox*      m_limiterBeforeOutput{nullptr};
    QCheckBox*      m_db6Booster{nullptr};
    QDoubleSpinBox* m_tunerA4{nullptr};
    QDoubleSpinBox* m_recNoteTrigger{nullptr};
    QComboBox*      m_recNoteOptimize{nullptr};

    // ---- MIDI tab ----
    QCheckBox*  m_autoConnectMidi{nullptr};
    QComboBox*  m_midiChannel{nullptr};
    QComboBox*  m_harChannel{nullptr};
    QComboBox*  m_midiImplRadio{nullptr};
    QCheckBox*  m_autoAssign{nullptr};

    // ---- JACK tab ----
    QCheckBox*  m_autoConnectOut{nullptr};
    QCheckBox*  m_autoConnectIn{nullptr};

    // ---- Misc tab ----
    QLineEdit*  m_username{nullptr};
    QCheckBox*  m_disableWarnings{nullptr};
    QSpinBox*   m_tapTimeout{nullptr};
    QCheckBox*  m_enableTooltips{nullptr};

    // ---- Bank Paths tab ----
    QLineEdit*  m_bankFilename{nullptr};
    QLineEdit*  m_userDir{nullptr};
};
