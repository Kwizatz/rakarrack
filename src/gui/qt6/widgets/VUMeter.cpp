/*
  rakarrack - guitar multi-effects processor
  SPDX-License-Identifier: GPL-2.0-only

  Qt6 GUI — VUMeter widget implementation

  Replicates the FLTK NewVum draw_rest() rendering:
    - value normalised via  val = 1 - ((dB + 48) * 0.015873016)
    - four colour zones (green / yellow / orange / red) filled bottom-up
    - 3 px darker left + 2 px right for 3-D bevel
    - horizontal tick marks every 4 px for a segmented LED look
*/

#include "VUMeter.hpp"

#include <QPainter>
#include <algorithm>
#include <cmath>

// ---------------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------------

VUMeter::VUMeter(QWidget* parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_OpaquePaintEvent);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
}

// ---------------------------------------------------------------------------
// Level property
// ---------------------------------------------------------------------------

void VUMeter::setLevel(float dB)
{
    dB = std::clamp(dB, kMinDB, kMaxDB);
    if (dB != m_levelDB)
    {
        m_levelDB = dB;
        update();
    }
}

// ---------------------------------------------------------------------------
// Paint
// ---------------------------------------------------------------------------

void VUMeter::paintEvent(QPaintEvent* /*event*/)
{
    QPainter p(this);

    const int w = width();
    const int h = height();

    // Background
    const QColor bg = palette().color(QPalette::Window).darker(150);
    p.fillRect(rect(), bg);

    // Normalise: -48 dB → 0.0 (empty),  +15 dB → 1.0 (full)
    const float norm = std::clamp(
        (m_levelDB - kMinDB) / (kMaxDB - kMinDB), 0.0F, 1.0F);

    // Number of lit pixels (from the bottom)
    const int litH = static_cast<int>(std::round(norm * h));
    if (litH <= 0)
        return;

    // Zone thresholds as fractions of total height (from bottom)
    //   green:  0.0   – 0.508
    //   yellow: 0.508 – 0.65
    //   orange: 0.65  – 0.761
    //   red:    0.761 – 1.0
    constexpr float kYellow = 0.508F;
    constexpr float kOrange = 0.650F;
    constexpr float kRed    = 0.761F;

    auto zoneColor = [&](float frac) -> QColor {
        if (frac < kYellow) return {0, 200, 0};       // green
        if (frac < kOrange) return {230, 230, 0};      // yellow
        if (frac < kRed)    return {255, 140, 0};      // orange
        return {240, 40, 40};                           // red
    };

    // Draw filled bar bottom-up, one pixel row at a time
    const int bevelL = std::min(3, w / 4);   // darker left edge
    const int bevelR = std::min(2, w / 4);   // darker right edge
    const int inner  = w - bevelL - bevelR;

    for (int row = 0; row < litH && row < h; ++row)
    {
        const int y = h - 1 - row;
        const float frac = static_cast<float>(row) / std::max(1, h - 1);
        const QColor c = zoneColor(frac);

        // Inner bar
        p.fillRect(bevelL, y, inner, 1, c);

        // Bevelled edges (darker)
        const QColor dark = c.darker(150);
        if (bevelL > 0) p.fillRect(0, y, bevelL, 1, dark);
        if (bevelR > 0) p.fillRect(w - bevelR, y, bevelR, 1, dark);
    }

    // Tick marks every 4 px for segmented-LED appearance
    p.setPen(bg);
    for (int row = 0; row < litH && row < h; row += 4)
    {
        const int y = h - 1 - row;
        p.drawLine(0, y, w - 1, y);
    }
}
