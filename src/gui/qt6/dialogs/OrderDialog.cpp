/*
  rakarrack - guitar multi-effects processor
  SPDX-License-Identifier: GPL-2.0-only

  Qt6 GUI — Effect Order Dialog implementation
*/

#include "OrderDialog.hpp"
#include "EngineController.hpp"
#include "global.hpp"

#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>

// Category filter bitmasks (matching efx_names[].Type in process.cpp)
namespace
{
struct FilterEntry
{
    const char* label;
    int mask;
};

constexpr std::array<FilterEntry, 9> kFilters = {{
    {"All",              0},
    {"Distortion",       1},
    {"Modulation",       2},
    {"Time",             4},
    {"Emulation",        8},
    {"Filters",         16},
    {"Dynamics",        32},
    {"Processing & EQ", 64},
    {"Synthesis",      128},
}};
} // namespace

OrderDialog::OrderDialog(EngineController& engine, QWidget* parent)
    : QDialog(parent)
    , m_engine(engine)
{
    setupUi();

    // Take a snapshot of the current order
    auto order = m_engine.getEffectOrder();
    for (int i = 0; i < kOrderSlots; ++i)
    {
        m_newOrder[static_cast<std::size_t>(i)] = order[static_cast<std::size_t>(i)];
        m_savedOrder[static_cast<std::size_t>(i)] = order[static_cast<std::size_t>(i)];
    }

    populateOrderList();
    populateAvailableList();
}

// ---------------------------------------------------------------------------
// UI Setup
// ---------------------------------------------------------------------------

void OrderDialog::setupUi()
{
    setWindowTitle(tr("Effect Order"));
    resize(560, 440);
    setMinimumSize(480, 360);

    auto* mainLayout = new QVBoxLayout(this);

    // Title
    auto* title = new QLabel(tr("Put Order in your Rack"), this);
    auto font = title->font();
    font.setBold(true);
    font.setPointSize(font.pointSize() + 2);
    title->setFont(font);
    title->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(title);

    // Two-panel layout: Available | Controls | Order
    auto* panelLayout = new QHBoxLayout();

    // ---- Available effects list ----
    auto* leftPanel = new QVBoxLayout();
    leftPanel->addWidget(new QLabel(tr("Available Effects"), this));
    m_availList = new QListWidget(this);
    leftPanel->addWidget(m_availList, 1);

    // Category filter combo
    m_filterCombo = new QComboBox(this);
    for (const auto& f : kFilters)
        m_filterCombo->addItem(QString::fromLatin1(f.label), f.mask);
    connect(m_filterCombo, &QComboBox::currentIndexChanged,
            this, &OrderDialog::onFilterChanged);
    leftPanel->addWidget(m_filterCombo);

    panelLayout->addLayout(leftPanel, 1);

    // ---- Center controls ----
    auto* centerPanel = new QVBoxLayout();
    centerPanel->addStretch();

    auto* replaceBtn = new QPushButton(QStringLiteral("\u2194"), this);  // ↔
    replaceBtn->setToolTip(tr("Replace selected slot with selected effect"));
    replaceBtn->setFixedSize(40, 40);
    connect(replaceBtn, &QPushButton::clicked, this, &OrderDialog::onReplaceEffect);
    centerPanel->addWidget(replaceBtn, 0, Qt::AlignCenter);

    centerPanel->addSpacing(16);

    auto* upBtn = new QPushButton(QStringLiteral("\u2191"), this);  // ↑
    upBtn->setToolTip(tr("Move selected slot up"));
    upBtn->setFixedSize(40, 40);
    connect(upBtn, &QPushButton::clicked, this, &OrderDialog::onMoveUp);
    centerPanel->addWidget(upBtn, 0, Qt::AlignCenter);

    auto* downBtn = new QPushButton(QStringLiteral("\u2193"), this);  // ↓
    downBtn->setToolTip(tr("Move selected slot down"));
    downBtn->setFixedSize(40, 40);
    connect(downBtn, &QPushButton::clicked, this, &OrderDialog::onMoveDown);
    centerPanel->addWidget(downBtn, 0, Qt::AlignCenter);

    centerPanel->addStretch();
    panelLayout->addLayout(centerPanel);

    // ---- Current order list ----
    auto* rightPanel = new QVBoxLayout();
    rightPanel->addWidget(new QLabel(tr("Current Chain (10 Slots)"), this));
    m_orderList = new QListWidget(this);
    rightPanel->addWidget(m_orderList, 1);
    panelLayout->addLayout(rightPanel, 1);

    mainLayout->addLayout(panelLayout, 1);

    // ---- OK / Cancel ----
    auto* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();

    auto* okBtn = new QPushButton(tr("OK"), this);
    okBtn->setDefault(true);
    connect(okBtn, &QPushButton::clicked, this, &OrderDialog::onAccept);
    buttonLayout->addWidget(okBtn);

    auto* cancelBtn = new QPushButton(tr("Cancel"), this);
    connect(cancelBtn, &QPushButton::clicked, this, &OrderDialog::onReject);
    buttonLayout->addWidget(cancelBtn);

    mainLayout->addLayout(buttonLayout);
}

// ---------------------------------------------------------------------------
// List population
// ---------------------------------------------------------------------------

void OrderDialog::populateOrderList()
{
    m_orderList->clear();
    auto& rkr = m_engine.engine();

    for (int i = 0; i < kOrderSlots; ++i)
    {
        int effectPos = m_newOrder[static_cast<std::size_t>(i)];

        // Find the name for this effect position
        QString name = tr("Unknown");
        for (int e = 0; e < 70; ++e)
        {
            if (rkr.efx_names[e].Pos == effectPos)
            {
                name = QString::fromLatin1(rkr.efx_names[e].Nom.data());
                break;
            }
        }

        m_orderList->addItem(
            QString::number(i + 1) + QStringLiteral(". ") + name);
    }
}

void OrderDialog::populateAvailableList()
{
    m_availList->clear();
    auto& rkr = m_engine.engine();

    for (int e = 0; e < 70; ++e)
    {
        if (rkr.efx_names[e].Nom[0] == '\0')
            continue;

        // Apply category filter
        if (m_filter != 0 && (rkr.efx_names[e].Type & m_filter) == 0)
            continue;

        // Check if this effect is already in the chain
        bool inChain = false;
        for (int s = 0; s < kOrderSlots; ++s)
        {
            if (m_newOrder[static_cast<std::size_t>(s)] == rkr.efx_names[e].Pos)
            {
                inChain = true;
                break;
            }
        }

        if (!inChain)
        {
            auto* item = new QListWidgetItem(
                QString::fromLatin1(rkr.efx_names[e].Nom.data()));
            item->setData(Qt::UserRole, rkr.efx_names[e].Pos);
            m_availList->addItem(item);
        }
    }
}

// ---------------------------------------------------------------------------
// Slots
// ---------------------------------------------------------------------------

void OrderDialog::onMoveUp()
{
    int row = m_orderList->currentRow();
    if (row <= 0)
        return;

    std::swap(m_newOrder[static_cast<std::size_t>(row)],
              m_newOrder[static_cast<std::size_t>(row - 1)]);
    populateOrderList();
    m_orderList->setCurrentRow(row - 1);
}

void OrderDialog::onMoveDown()
{
    int row = m_orderList->currentRow();
    if (row < 0 || row >= kOrderSlots - 1)
        return;

    std::swap(m_newOrder[static_cast<std::size_t>(row)],
              m_newOrder[static_cast<std::size_t>(row + 1)]);
    populateOrderList();
    m_orderList->setCurrentRow(row + 1);
}

void OrderDialog::onReplaceEffect()
{
    int orderRow = m_orderList->currentRow();
    auto* availItem = m_availList->currentItem();

    if (orderRow < 0 || !availItem)
        return;

    int newEffectPos = availItem->data(Qt::UserRole).toInt();
    m_newOrder[static_cast<std::size_t>(orderRow)] = newEffectPos;

    populateOrderList();
    populateAvailableList();
    m_orderList->setCurrentRow(orderRow);
}

void OrderDialog::onFilterChanged(int filterIndex)
{
    if (filterIndex >= 0 && filterIndex < static_cast<int>(kFilters.size()))
        m_filter = kFilters[static_cast<std::size_t>(filterIndex)].mask;
    else
        m_filter = 0;

    populateAvailableList();
}

void OrderDialog::onAccept()
{
    // Apply the new order to the engine
    m_engine.setEffectOrder(m_newOrder);
    accept();
}

void OrderDialog::onReject()
{
    // Restore saved order (no-op if engine wasn't changed)
    reject();
}
