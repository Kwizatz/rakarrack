/*
  rakarrack - guitar multi-effects processor
  SPDX-License-Identifier: GPL-2.0-only

  Qt6 GUI — Oscilloscope widget implementation

  Replicates the FLTK Scope draw():
    - Widget split into two halves (right channel left, left channel right)
    - Each sample mapped to a horizontal pixel position
    - Amplitude clamped to [-1,+1], scaled to ±0.5*height
    - Connected line segments in the accent color
*/

#include "Oscilloscope.hpp"

#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <algorithm>
#include <cmath>

// ---------------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------------

Oscilloscope::Oscilloscope(QWidget* parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_OpaquePaintEvent);
}

// ---------------------------------------------------------------------------
// Data input
// ---------------------------------------------------------------------------

void Oscilloscope::setData(const float* left, const float* right, int sampleCount)
{
    if (sampleCount <= 0 || (!left && !right))
    {
        m_left.clear();
        m_right.clear();
        update();
        return;
    }

    m_left.assign(left, left + sampleCount);
    m_right.assign(right, right + sampleCount);
    update();
}

// ---------------------------------------------------------------------------
// Paint
// ---------------------------------------------------------------------------

static void drawChannel(QPainter& p, const std::vector<float>& samples,
                        int ox, int oy, int cw, int ch, const QColor& color)
{
    if (samples.empty() || cw <= 0)
        return;

    const int n   = static_cast<int>(samples.size());
    const float centerY = oy + ch * 0.5F;
    const float scaleY  = ch * 0.5F;

    QPainterPath path;
    bool first = true;

    for (int i = 0; i < n; ++i)
    {
        const float px = ox + static_cast<float>(i) * cw / n;
        const float s  = std::clamp(samples[static_cast<std::size_t>(i)], -1.0F, 1.0F);
        const float py = centerY - s * scaleY;

        if (first)
        {
            path.moveTo(static_cast<double>(px), static_cast<double>(py));
            first = false;
        }
        else
        {
            path.lineTo(static_cast<double>(px), static_cast<double>(py));
        }
    }

    p.setPen(QPen(color, 1));
    p.drawPath(path);
}

void Oscilloscope::paintEvent(QPaintEvent* /*event*/)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    const int w = width();
    const int h = height();
    const QColor bg     = palette().color(QPalette::Window).darker(150);
    const QColor accent = palette().color(QPalette::Highlight);

    // Background
    p.fillRect(rect(), bg);

    if (m_left.empty() && m_right.empty())
        return;

    // Center line
    p.setPen(QPen(bg.lighter(140), 1, Qt::DotLine));
    p.drawLine(0, h / 2, w, h / 2);

    // Split into two halves with 5 px gap (matching FLTK Scope)
    const int halfW = (w / 2) - 5;

    // Left half: right channel; Right half: left channel (FLTK convention)
    drawChannel(p, m_right, 0, 0, halfW, h, accent);
    drawChannel(p, m_left,  w / 2 + 5, 0, halfW, h, accent.lighter(130));
}

// ---------------------------------------------------------------------------
// Mouse — click to dismiss
// ---------------------------------------------------------------------------

void Oscilloscope::mouseReleaseEvent(QMouseEvent* event)
{
    emit dismissed();
    event->accept();
}
