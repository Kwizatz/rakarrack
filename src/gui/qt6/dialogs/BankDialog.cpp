/*
  rakarrack - guitar multi-effects processor
  SPDX-License-Identifier: GPL-2.0-only

  Qt6 GUI — Bank Manager Dialog implementation
*/

#include "BankDialog.hpp"
#include "EngineController.hpp"
#include "global.hpp"

#include <QFileDialog>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

BankDialog::BankDialog(EngineController& engine, QWidget* parent)
    : QDialog(parent)
    , m_engine(engine)
{
    setupUi();
    refreshBank();
}

// ---------------------------------------------------------------------------
// UI Setup
// ---------------------------------------------------------------------------

void BankDialog::setupUi()
{
    setWindowTitle(tr("Bank Manager"));
    resize(800, 600);
    setMinimumSize(640, 480);

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(8, 8, 8, 8);
    mainLayout->setSpacing(6);

    // Toolbar row
    auto* toolbar = new QWidget(this);
    createToolbar(toolbar);
    mainLayout->addWidget(toolbar);

    // Bank name label
    m_bankNameLabel = new QLabel(this);
    m_bankNameLabel->setAlignment(Qt::AlignCenter);
    auto font = m_bankNameLabel->font();
    font.setBold(true);
    font.setPointSize(font.pointSize() + 2);
    m_bankNameLabel->setFont(font);
    mainLayout->addWidget(m_bankNameLabel);

    // Preset grid
    auto* gridContainer = new QWidget(this);
    createPresetGrid(gridContainer);
    mainLayout->addWidget(gridContainer, 1);
}

void BankDialog::createToolbar(QWidget* container)
{
    auto* layout = new QHBoxLayout(container);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(4);

    auto* newBtn = new QPushButton(tr("New"), container);
    connect(newBtn, &QPushButton::clicked, this, &BankDialog::onNewBank);
    layout->addWidget(newBtn);

    auto* loadBtn = new QPushButton(tr("Load Bank"), container);
    connect(loadBtn, &QPushButton::clicked, this, &BankDialog::onLoadBank);
    layout->addWidget(loadBtn);

    auto* saveBtn = new QPushButton(tr("Save Bank"), container);
    connect(saveBtn, &QPushButton::clicked, this, &BankDialog::onSaveBank);
    layout->addWidget(saveBtn);

    layout->addSpacing(16);

    // Built-in bank buttons: Default, Extra, Extra1, User
    static constexpr std::array<const char*, 4> bankLabels = {"1", "2", "3", "U"};
    for (int i = 0; i < 4; ++i)
    {
        auto* btn = new QPushButton(QString::fromLatin1(bankLabels[static_cast<std::size_t>(i)]),
                                    container);
        btn->setFixedWidth(32);
        btn->setToolTip(i < 3 ? tr("Built-in Bank %1").arg(i + 1) : tr("User Bank"));
        connect(btn, &QPushButton::clicked, this, [this, i] { onLoadBuiltinBank(i); });
        layout->addWidget(btn);
    }

    layout->addStretch();

    auto* closeBtn = new QPushButton(tr("Close"), container);
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
    layout->addWidget(closeBtn);
}

void BankDialog::createPresetGrid(QWidget* container)
{
    auto* grid = new QGridLayout(container);
    grid->setContentsMargins(4, 4, 4, 4);
    grid->setSpacing(3);

    // 15 rows × 4 columns = 60 presets
    constexpr int cols = 4;
    constexpr int rows = 15;

    for (int i = 0; i < kBankSlots; ++i)
    {
        int row = i / cols;
        int col = i % cols;

        auto* btn = new QPushButton(QString::number(i + 1), container);
        btn->setMinimumHeight(28);
        btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

        connect(btn, &QPushButton::clicked, this, [this, i] { onPresetClicked(i); });

        grid->addWidget(btn, row, col);
        m_presetButtons[static_cast<std::size_t>(i)] = btn;
    }

    // Stretch rows equally
    for (int r = 0; r < rows; ++r)
        grid->setRowStretch(r, 1);
    for (int c = 0; c < cols; ++c)
        grid->setColumnStretch(c, 1);
}

// ---------------------------------------------------------------------------
// Refresh
// ---------------------------------------------------------------------------

void BankDialog::refreshBank()
{
    auto& rkr = m_engine.engine();

    for (int i = 0; i < kBankSlots; ++i)
    {
        const char* name = rkr.presets.Bank[i].Preset_Name.data();
        QString label = QString::number(i + 1) + QStringLiteral(". ")
                        + QString::fromLatin1(name);
        m_presetButtons[static_cast<std::size_t>(i)]->setText(label);
    }

    m_bankNameLabel->setText(
        QString::fromLatin1(rkr.presets.Bank_Saved.data()));
}

// ---------------------------------------------------------------------------
// Slots
// ---------------------------------------------------------------------------

void BankDialog::onPresetClicked(int index)
{
    m_selectedSlot = index;
    m_engine.engine().Bank_to_Preset(index);
    refreshBank();
}

void BankDialog::onNewBank()
{
    auto answer = QMessageBox::question(
        this, tr("New Bank"),
        tr("Clear all 60 preset slots?"),
        QMessageBox::Yes | QMessageBox::No);

    if (answer == QMessageBox::Yes)
    {
        m_engine.engine().New_Bank();
        refreshBank();
    }
}

void BankDialog::onLoadBank()
{
    QString path = QFileDialog::getOpenFileName(
        this, tr("Load Bank"), QString(),
        tr("Rakarrack Banks (*.rkrb);;All Files (*)"));

    if (!path.isEmpty())
    {
        QByteArray ba = path.toLocal8Bit();
        m_engine.engine().loadbank(ba.data());
        refreshBank();
    }
}

void BankDialog::onSaveBank()
{
    QString path = QFileDialog::getSaveFileName(
        this, tr("Save Bank"), QString(),
        tr("Rakarrack Banks (*.rkrb);;All Files (*)"));

    if (!path.isEmpty())
    {
        QByteArray ba = path.toLocal8Bit();
        m_engine.engine().savebank(ba.data());
    }
}

void BankDialog::onLoadBuiltinBank(int which)
{
    auto& rkr = m_engine.engine();

    // Banks 0-2 are built-in (loaded from compiled-in resources),
    // bank 3 is the user bank.
    switch (which)
    {
    case 0: // Default.rkrb
    case 1: // Extra.rkrb
    case 2: // Extra1.rkrb
        rkr.loadnames();
        rkr.presets.a_bank = which;
        for (int i = 0; i < kBankSlots; ++i)
        {
            std::memcpy(rkr.presets.Bank[i].Preset_Name.data(),
                        rkr.presets.B_Names[which][i].Preset_Name.data(),
                        rkr.presets.Bank[i].Preset_Name.size());
        }
        break;

    case 3: // User bank
    {
        const char* udir = rkr.presets.UDirFilename.data();
        if (udir[0] != '\0')
        {
            // loadbank expects char* (legacy API)
            char buf[256]{};
            snprintf(buf, sizeof(buf), "%s", udir);
            rkr.loadbank(buf);
        }
        break;
    }
    default:
        break;
    }

    refreshBank();
}
