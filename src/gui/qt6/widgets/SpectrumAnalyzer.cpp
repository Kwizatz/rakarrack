/*
  rakarrack - guitar multi-effects processor
  SPDX-License-Identifier: GPL-2.0-only

  Qt6 GUI — SpectrumAnalyzer widget implementation
*/

#include "SpectrumAnalyzer.hpp"

#include <QMouseEvent>
#include <QPainter>
#include <algorithm>
#include <cmath>

// ---------------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------------

SpectrumAnalyzer::SpectrumAnalyzer(QWidget* parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_OpaquePaintEvent);
    m_bands.fill(0.0F);
}

// ---------------------------------------------------------------------------
// Data input
// ---------------------------------------------------------------------------

void SpectrumAnalyzer::setData(std::span<const float> bands)
{
    const int n = std::min(static_cast<int>(bands.size()), kBandCount);
    for (int i = 0; i < n; ++i)
        m_bands[static_cast<std::size_t>(i)] = std::clamp(bands[static_cast<std::size_t>(i)], 0.0F, 1.0F);
    for (int i = n; i < kBandCount; ++i)
        m_bands[static_cast<std::size_t>(i)] = 0.0F;
    update();
}

// ---------------------------------------------------------------------------
// Paint
// ---------------------------------------------------------------------------

void SpectrumAnalyzer::paintEvent(QPaintEvent* /*event*/)
{
    QPainter p(this);

    const int w = width();
    const int h = height();
    const QColor bg     = palette().color(QPalette::Window).darker(150);
    const QColor accent = palette().color(QPalette::Highlight);

    // Background
    p.fillRect(rect(), bg);

    if (w < kBandCount * 2)
        return; // too narrow to draw

    // Bar width: divide available width equally, leave 2 px gap
    const int barW  = std::max(1, (w / kBandCount) - 2);
    const int gap   = 2;
    const int totalBarW = barW + gap;
    const int xOff  = (w - totalBarW * kBandCount) / 2; // centre

    for (int i = 0; i < kBandCount; ++i)
    {
        const float mag  = m_bands[static_cast<std::size_t>(i)];
        const int   barH = static_cast<int>(std::round(mag * h));
        if (barH <= 0)
            continue;

        const int x = xOff + i * totalBarW;
        const int y = h - barH;

        p.fillRect(x, y, barW, barH, accent);
    }
}

// ---------------------------------------------------------------------------
// Mouse — click to dismiss
// ---------------------------------------------------------------------------

void SpectrumAnalyzer::mouseReleaseEvent(QMouseEvent* event)
{
    emit dismissed();
    event->accept();
}
