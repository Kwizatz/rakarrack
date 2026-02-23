/*
  rakarrack - guitar multi-effects processor
  SPDX-License-Identifier: GPL-2.0-only

  Qt6 GUI — MIDI Learn Dialog

  Parameter list on the left, CC assignment grid in the center,
  and preset copy/clear controls.
*/

#pragma once

#include <QDialog>

class EngineController;
class QLabel;
class QListWidget;
class QComboBox;
class QSpinBox;
class QPushButton;

class MidiLearnDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MidiLearnDialog(EngineController& engine, QWidget* parent = nullptr);
    ~MidiLearnDialog() override = default;

private Q_SLOTS:
    void onParameterSelected();
    void onAssign();
    void onAssignToAll();
    void onGetMidiMessage();
    void onCancelCapture();
    void onClearAssignments();
    void onClearPreset();
    void onFilterChanged(int index);

private:
    void setupUi();
    void populateParameterList();
    void displayAssignments();

    EngineController& m_engine;

    QListWidget* m_paramList{nullptr};
    QComboBox*   m_filterCombo{nullptr};
    QSpinBox*    m_ccSpinner{nullptr};
    QPushButton* m_getMidiBtn{nullptr};
    QPushButton* m_cancelBtn{nullptr};

    // Assignment display: up to 20 slots shown as labels
    static constexpr int kMaxAssignSlots = 20;
    std::array<QLabel*, kMaxAssignSlots> m_assignLabels{};

    int m_currentFilter{0};   // 0 = All, 1 = Chain
};
