/*
  rakarrack - guitar multi-effects processor
  SPDX-License-Identifier: GPL-2.0-only

  Qt6 GUI — TunerDisplay widget implementation

  Replicates the FLTK TunerLed draw_rest() logic:
    - value normalised to 0.0–1.0 (0.5 = in tune)
    - hidden at extremes (< 0.1 or > 0.9)
    - 7 px wide indicator bar at the normalised position
    - green when |cents| ≤ 5, red otherwise
    - optional note name + frequency text drawn alongside
*/

#include "TunerDisplay.hpp"

#include <QPainter>
#include <algorithm>
#include <cmath>

// ---------------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------------

TunerDisplay::TunerDisplay(QWidget* parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_OpaquePaintEvent);
}

// ---------------------------------------------------------------------------
// Data input
// ---------------------------------------------------------------------------

void TunerDisplay::setTunerData(float cents, const QString& noteName, float frequency)
{
    m_cents     = cents;
    m_noteName  = noteName;
    m_frequency = frequency;
    update();
}

// ---------------------------------------------------------------------------
// Paint
// ---------------------------------------------------------------------------

void TunerDisplay::paintEvent(QPaintEvent* /*event*/)
{
    QPainter p(this);

    const int w = width();
    const int h = height();
    const QColor bg = palette().color(QPalette::Window).darker(140);

    // Background
    p.fillRect(rect(), bg);

    // Normalise cents -32..+32 → 0.0..1.0
    constexpr float kMinCents = -32.0F;
    constexpr float kMaxCents =  32.0F;
    const float clamped = std::clamp(m_cents, kMinCents, kMaxCents);
    const float val = (clamped - kMinCents) / (kMaxCents - kMinCents);

    // Draw centre tick mark
    {
        const int cx = w / 2;
        p.setPen(QPen(bg.lighter(180), 1));
        p.drawLine(cx, 0, cx, h);
    }

    // Indicator bar — hide at extremes (matching FLTK TunerLed)
    if (val > 0.1F && val < 0.9F)
    {
        constexpr int kIndicatorW = 7;
        const int posX = static_cast<int>(std::round(val * (w - 1))) - kIndicatorW / 2;
        const int iy   = 2;
        const int ih   = h - 4;

        // Green if within ±5 cents, red otherwise
        const QColor col = (std::abs(m_cents) <= 5.0F)
                               ? QColor(0, 220, 0)
                               : QColor(240, 60, 60);

        p.fillRect(posX, iy, kIndicatorW, ih, col);
    }

    // Note name + frequency text
    if (!m_noteName.isEmpty())
    {
        p.setPen(palette().color(QPalette::WindowText));
        QFont f = font();
        f.setPointSize(std::max(7, h / 2));
        p.setFont(f);

        const QString txt = QStringLiteral("%1  %2 Hz")
                                .arg(m_noteName)
                                .arg(static_cast<double>(m_frequency), 0, 'f', 1);

        // Draw text at right edge
        const QRect textR(w * 2 / 3, 0, w / 3 - 2, h);
        p.drawText(textR, Qt::AlignVCenter | Qt::AlignRight, txt);
    }
}
