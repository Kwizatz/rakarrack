/*
  rakarrack - guitar multi-effects processor
  SPDX-License-Identifier: GPL-2.0-only

  Qt6 GUI — Trigger (ACI) Dialog

  Audio-controlled interface: uses audio input level to generate
  MIDI control messages for parameter automation.
*/

#pragma once

#include <QDialog>

class EngineController;
class VUMeter;
class QComboBox;
class QSpinBox;
class QSlider;

class TriggerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TriggerDialog(EngineController& engine, QWidget* parent = nullptr);
    ~TriggerDialog() override;

private Q_SLOTS:
    void onSourceChanged(int index);
    void onGainChanged(int value);
    void onThresholdChanged(int value);
    void onMidiControlChanged(int value);
    void onMinimumChanged(int value);
    void onMaximumChanged(int value);

private:
    void setupUi();
    void loadFromEngine();

    EngineController& m_engine;

    VUMeter*   m_vuMeter{nullptr};
    QComboBox* m_source{nullptr};
    QSlider*   m_gain{nullptr};
    QSlider*   m_threshold{nullptr};
    QSpinBox*  m_midiControl{nullptr};
    QSlider*   m_minimum{nullptr};
    QSlider*   m_maximum{nullptr};
};
