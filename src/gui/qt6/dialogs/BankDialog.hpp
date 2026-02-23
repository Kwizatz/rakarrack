/*
  rakarrack - guitar multi-effects processor
  SPDX-License-Identifier: GPL-2.0-only

  Qt6 GUI — Bank Manager Dialog

  60-slot preset grid (15 rows × 4 columns) with load/save/new bank operations.
*/

#pragma once

#include <QDialog>
#include <array>

class EngineController;
class QPushButton;
class QLabel;

/// Number of preset slots in a bank.
inline constexpr int kBankSlots = 60;

class BankDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BankDialog(EngineController& engine, QWidget* parent = nullptr);
    ~BankDialog() override = default;

    /// Refresh all 60 button labels from the engine's bank data.
    void refreshBank();

private Q_SLOTS:
    void onPresetClicked(int index);
    void onNewBank();
    void onLoadBank();
    void onSaveBank();
    void onLoadBuiltinBank(int which);

private:
    void setupUi();
    void createPresetGrid(QWidget* container);
    void createToolbar(QWidget* container);

    EngineController& m_engine;
    std::array<QPushButton*, kBankSlots> m_presetButtons{};
    QLabel* m_bankNameLabel{nullptr};
    int m_selectedSlot{-1};
};
