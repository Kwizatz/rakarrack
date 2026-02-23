/*
  rakarrack - guitar multi-effects processor
  SPDX-License-Identifier: GPL-2.0-only

  Qt6 GUI — ThemeManager implementation
*/

#include "ThemeManager.hpp"

#include <QApplication>
#include <QFile>
#include <QFont>
#include <QPalette>
#include <QPixmap>
#include <QStringList>
#include <QWidget>

ThemeManager::ThemeManager(QObject* parent)
    : QObject(parent)
{
}

// ---------------------------------------------------------------------------
// Stylesheet
// ---------------------------------------------------------------------------

void ThemeManager::applyDarkTheme()
{
    QFile qss(QStringLiteral(":/styles/dark.qss"));
    if (qss.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qApp->setStyleSheet(QString::fromUtf8(qss.readAll()));
        qss.close();
    }
}

// ---------------------------------------------------------------------------
// Background skin
// ---------------------------------------------------------------------------

QStringList ThemeManager::availableSkins()
{
    return {
        QStringLiteral(":/skins/bg.png"),
        QStringLiteral(":/skins/bg1.png"),
        QStringLiteral(":/skins/bg2.png"),
        QStringLiteral(":/skins/bg3.png"),
        QStringLiteral(":/skins/bg4.png"),
        QStringLiteral(":/skins/bg5.png"),
        QStringLiteral(":/skins/bg6.png"),
        QStringLiteral(":/skins/bg_gray_furr.png"),
    };
}

void ThemeManager::applySkin(QWidget* target, const QString& skinPath)
{
    m_currentSkin = skinPath;

    if (!target)
        return;

    if (skinPath.isEmpty())
    {
        // Remove tiled background — revert to stylesheet default
        target->setStyleSheet(QString());
        return;
    }

    // Attempt to load from Qt resource or filesystem
    QPixmap px(skinPath);
    if (px.isNull())
    {
        target->setStyleSheet(QString());
        return;
    }

    // Use CSS border-image to tile the skin across the widget.
    // border-image with "repeat" tiles the image automatically.
    target->setStyleSheet(
        QStringLiteral("QWidget#centralWidget { "
                        "border-image: url(%1) 0 0 0 0 repeat repeat; }")
            .arg(skinPath));
}

// ---------------------------------------------------------------------------
// Font
// ---------------------------------------------------------------------------

void ThemeManager::applyFont(const QString& family, int sizeOffset)
{
    QFont font = QApplication::font();

    if (!family.isEmpty() && family != QStringLiteral("Default"))
    {
        font.setFamily(family);
    }

    // Default point size is typically 10; offset adjusts from there
    int baseSize = 10;
    int newSize  = baseSize + sizeOffset;

    if (newSize < 6)   newSize = 6;
    if (newSize > 24)  newSize = 24;

    font.setPointSize(newSize);
    QApplication::setFont(font);
}
