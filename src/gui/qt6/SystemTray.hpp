/*
  rakarrack - guitar multi-effects processor
  SPDX-License-Identifier: GPL-2.0-only

  Qt6 GUI — SystemTray

  Optional system tray icon with context menu.
  Allows Show/Hide window, Bypass toggle, and Quit.
*/

#pragma once

#include <QObject>

class EngineController;
class QMainWindow;
class QSystemTrayIcon;

class SystemTray : public QObject
{
    Q_OBJECT

public:
    /// Construct the tray icon.  Does nothing if the desktop has no system tray.
    explicit SystemTray(EngineController& engine,
                        QMainWindow* mainWindow,
                        QObject* parent = nullptr);
    ~SystemTray() override = default;

    /// Returns true if the tray icon is visible.
    [[nodiscard]] bool isAvailable() const;

private Q_SLOTS:
    void toggleWindowVisibility();
    void toggleBypass();

private:
    EngineController& m_engine;
    QMainWindow*      m_mainWindow{nullptr};
    QSystemTrayIcon*  m_trayIcon{nullptr};
};
