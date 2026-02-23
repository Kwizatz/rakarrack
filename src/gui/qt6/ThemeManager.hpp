/*
  rakarrack - guitar multi-effects processor
  SPDX-License-Identifier: GPL-2.0-only

  Qt6 GUI — ThemeManager

  Centralised theming: dark stylesheet, background skin tiling, font scaling.
  Owned by MainWindow; SettingsDialog calls applyFont() / applySkin() after
  the user changes Appearance settings.
*/

#pragma once

#include <QObject>
#include <QPixmap>
#include <QString>

class QWidget;

/// Manages the application look-and-feel: stylesheet, background skin, font.
class ThemeManager : public QObject
{
    Q_OBJECT

public:
    explicit ThemeManager(QObject* parent = nullptr);
    ~ThemeManager() override = default;

    // ─── Stylesheet ────────────────────────────────────────────────

    /// Load and apply the dark theme from the embedded resource.
    void applyDarkTheme();

    // ─── Background skin ───────────────────────────────────────────

    /// List of built-in skin names (matching :/skins/<name>.png).
    static QStringList availableSkins();

    /// Apply a tiled background skin to @p target.
    /// Pass an empty @p skinPath to remove the background.
    void applySkin(QWidget* target, const QString& skinPath);

    /// Returns the currently active skin path (empty = none).
    [[nodiscard]] const QString& currentSkin() const { return m_currentSkin; }

    // ─── Font ──────────────────────────────────────────────────────

    /// Apply a global font with the given family and point-size offset.
    /// @p family  "" or "Default" = system default; otherwise "Sans", "Serif", "Monospace"
    /// @p sizeOffset  added to the default point size (can be negative)
    void applyFont(const QString& family, int sizeOffset);

private:
    QString m_currentSkin;
};
