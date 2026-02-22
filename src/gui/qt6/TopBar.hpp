/*
  rakarrack - guitar multi-effects processor
  SPDX-License-Identifier: GPL-2.0-only

  Qt6 GUI — TopBar widget

  Horizontal toolbar composing: InOut panel (VU meters + volume sliders),
  Presets panel (bank/preset selector + buttons), Tuner panel, and
  Tap Tempo panel.  Laid out left-to-right matching the FLTK Principal
  top bar.
*/

#pragma once

#include <QWidget>

class EngineController;
class VUMeter;
class TunerDisplay;
class MidiSlider;

class QComboBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QSpinBox;

class TopBar : public QWidget
{
    Q_OBJECT

public:
    explicit TopBar(EngineController& engine, QWidget* parent = nullptr);
    ~TopBar() override = default;

    /// Update VU meters, tuner, CPU, etc.  Called from MainWindow timer.
    void updateFromEngine();

    /// Sync all controls from current engine state (after preset load).
    void syncFromEngine();

Q_SIGNALS:
    void newPresetRequested();
    void loadPresetRequested();
    void savePresetRequested();
    void bankWindowRequested();
    void orderWindowRequested();
    void presetChanged(int index);

private:
    void setupInOutSection(QWidget* container);
    void setupPresetsSection(QWidget* container);
    void setupTunerSection(QWidget* container);
    void setupTapSection(QWidget* container);

    EngineController& m_engine;

    // InOut section
    QPushButton* m_fxOnButton    = nullptr;
    MidiSlider*  m_balanceSlider = nullptr;
    MidiSlider*  m_inputSlider   = nullptr;
    MidiSlider*  m_outputSlider  = nullptr;
    VUMeter*     m_inputVuL      = nullptr;
    VUMeter*     m_inputVuR      = nullptr;
    VUMeter*     m_outputVuL     = nullptr;
    VUMeter*     m_outputVuR     = nullptr;

    // Presets section
    QLineEdit*   m_presetName    = nullptr;
    QSpinBox*    m_presetCounter = nullptr;
    QLabel*      m_authorLabel   = nullptr;

    // Tuner section
    QPushButton* m_tunerOnButton = nullptr;
    TunerDisplay* m_tunerBar    = nullptr;
    QLabel*      m_noteLabel     = nullptr;
    QLabel*      m_freqLabel     = nullptr;

    // Tap section
    QPushButton* m_tapButton     = nullptr;
    QLabel*      m_bpmLabel      = nullptr;

    // Status
    QLabel*      m_cpuLabel      = nullptr;
};
