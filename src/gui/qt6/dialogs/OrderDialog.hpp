/*
  rakarrack - guitar multi-effects processor
  SPDX-License-Identifier: GPL-2.0-only

  Qt6 GUI — Effect Order Dialog

  Two-list interface: available effects on the left, current 10-slot chain
  on the right, with move/swap controls and category filtering.
*/

#pragma once

#include <QDialog>
#include <array>

class EngineController;
class QListWidget;
class QComboBox;

/// Number of effect processing slots in the chain.
inline constexpr int kOrderSlots = 10;

class OrderDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OrderDialog(EngineController& engine, QWidget* parent = nullptr);
    ~OrderDialog() override = default;

private Q_SLOTS:
    void onMoveUp();
    void onMoveDown();
    void onReplaceEffect();
    void onFilterChanged(int filterIndex);
    void onAccept();
    void onReject();

private:
    void setupUi();
    void populateOrderList();
    void populateAvailableList();

    EngineController& m_engine;

    QListWidget* m_orderList{nullptr};
    QListWidget* m_availList{nullptr};
    QComboBox*   m_filterCombo{nullptr};

    /// Working copy of effect order (modified during editing).
    std::array<int, kOrderSlots> m_newOrder{};

    /// Backup copy to restore on Cancel.
    std::array<int, kOrderSlots> m_savedOrder{};

    /// Current category filter bitmask (0 = All).
    int m_filter{0};
};
