/*
  rakarrack - guitar multi-effects processor
  SPDX-License-Identifier: GPL-2.0-only

  Qt6 GUI — SystemTray implementation
*/

#include "SystemTray.hpp"
#include "EngineController.hpp"

#include <QAction>
#include <QApplication>
#include <QIcon>
#include <QMainWindow>
#include <QMenu>
#include <QSystemTrayIcon>

SystemTray::SystemTray(EngineController& engine,
                       QMainWindow* mainWindow,
                       QObject* parent)
    : QObject(parent)
    , m_engine(engine)
    , m_mainWindow(mainWindow)
{
    if (!QSystemTrayIcon::isSystemTrayAvailable())
        return;

    // Build icon from embedded resource (multiple sizes for HiDPI)
    QIcon icon;
    icon.addFile(QStringLiteral(":/icons/rakarrack-32.png"),  QSize(32, 32));
    icon.addFile(QStringLiteral(":/icons/rakarrack-64.png"),  QSize(64, 64));
    icon.addFile(QStringLiteral(":/icons/rakarrack-128.png"), QSize(128, 128));

    m_trayIcon = new QSystemTrayIcon(icon, this);
    m_trayIcon->setToolTip(QStringLiteral("Rakarrack"));

    // Context menu
    auto* menu = new QMenu();

    auto* showHide = menu->addAction(tr("Show / Hide"));
    connect(showHide, &QAction::triggered,
            this, &SystemTray::toggleWindowVisibility);

    auto* bypass = menu->addAction(tr("Bypass Effects"));
    bypass->setCheckable(true);
    bypass->setChecked(m_engine.isBypassed());
    connect(bypass, &QAction::triggered, this, &SystemTray::toggleBypass);

    menu->addSeparator();

    auto* quit = menu->addAction(tr("Quit"));
    connect(quit, &QAction::triggered, qApp, &QApplication::quit);

    m_trayIcon->setContextMenu(menu);

    // Double-click toggles window visibility
    connect(m_trayIcon, &QSystemTrayIcon::activated,
            this, [this](QSystemTrayIcon::ActivationReason reason)
    {
        if (reason == QSystemTrayIcon::DoubleClick)
            toggleWindowVisibility();
    });

    m_trayIcon->show();
}

bool SystemTray::isAvailable() const
{
    return m_trayIcon && m_trayIcon->isVisible();
}

void SystemTray::toggleWindowVisibility()
{
    if (!m_mainWindow)
        return;

    if (m_mainWindow->isVisible())
    {
        m_mainWindow->hide();
    }
    else
    {
        m_mainWindow->show();
        m_mainWindow->raise();
        m_mainWindow->activateWindow();
    }
}

void SystemTray::toggleBypass()
{
    bool currently = m_engine.isBypassed();
    m_engine.setBypass(!currently);
}
