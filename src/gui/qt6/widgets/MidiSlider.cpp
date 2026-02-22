/*
  rakarrack - guitar multi-effects processor
  SPDX-License-Identifier: GPL-2.0-only

  Qt6 GUI — MidiSlider widget implementation
*/

#include "MidiSlider.hpp"

#include <QKeyEvent>
#include <QMenu>
#include <QMouseEvent>
#include <QPainter>
#include <QStyleOptionSlider>
#include <QWheelEvent>

#include <algorithm>
#include <cmath>

// ---------------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------------

MidiSlider::MidiSlider(Qt::Orientation orientation, QWidget* parent)
    : QSlider(orientation, parent)
{
    setFocusPolicy(Qt::StrongFocus);
    setSingleStep(1);
    setPageStep(10);
}

MidiSlider::MidiSlider(QWidget* parent)
    : MidiSlider(Qt::Horizontal, parent)
{
}

// ---------------------------------------------------------------------------
// MIDI controller property
// ---------------------------------------------------------------------------

void MidiSlider::setMidiController(int cc)
{
    if (m_midiCC != cc)
    {
        m_midiCC = cc;
        emit midiControllerChanged(cc);
        update();
    }
}

// ---------------------------------------------------------------------------
// Size hints
// ---------------------------------------------------------------------------

QSize MidiSlider::minimumSizeHint() const
{
    if (orientation() == Qt::Horizontal)
        return {kLabelAreaSize + 60, 20};
    return {20, kLabelAreaSize + 60};
}

QSize MidiSlider::sizeHint() const
{
    if (orientation() == Qt::Horizontal)
        return {kLabelAreaSize + 120, 24};
    return {24, kLabelAreaSize + 120};
}

// ---------------------------------------------------------------------------
// Geometry helpers
// ---------------------------------------------------------------------------

QRect MidiSlider::trackRect() const
{
    const auto r = rect();
    if (orientation() == Qt::Horizontal)
        return QRect(r.x() + kLabelAreaSize, r.y(),
                     r.width() - kLabelAreaSize, r.height());
    // vertical: label at top
    return QRect(r.x(), r.y() + kLabelAreaSize,
                 r.width(), r.height() - kLabelAreaSize);
}

QRect MidiSlider::labelRect() const
{
    const auto r = rect();
    if (orientation() == Qt::Horizontal)
        return QRect(r.x(), r.y(), kLabelAreaSize, r.height());
    return QRect(r.x(), r.y(), r.width(), kLabelAreaSize);
}

int MidiSlider::valueFromPos(int pos) const
{
    const auto tr = trackRect();
    double frac = 0.0;
    if (orientation() == Qt::Horizontal)
        frac = static_cast<double>(pos - tr.x()) / std::max(1, tr.width() - 1);
    else
        frac = 1.0 - static_cast<double>(pos - tr.y()) / std::max(1, tr.height() - 1);

    // Handle inverted appearance (min > max visually, e.g. volume sliders)
    if (invertedAppearance())
        frac = 1.0 - frac;

    frac = std::clamp(frac, 0.0, 1.0);
    return minimum() + static_cast<int>(std::round(frac * (maximum() - minimum())));
}

int MidiSlider::posFromValue(int val) const
{
    const auto tr = trackRect();
    const double frac = static_cast<double>(val - minimum()) /
                        std::max(1, maximum() - minimum());

    if (orientation() == Qt::Horizontal)
    {
        double f = invertedAppearance() ? 1.0 - frac : frac;
        return tr.x() + static_cast<int>(std::round(f * (tr.width() - 1)));
    }
    double f = invertedAppearance() ? frac : 1.0 - frac;
    return tr.y() + static_cast<int>(std::round(f * (tr.height() - 1)));
}

// ---------------------------------------------------------------------------
// Painting
// ---------------------------------------------------------------------------

void MidiSlider::paintEvent(QPaintEvent* /*event*/)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    const auto tr  = trackRect();
    const auto lr  = labelRect();
    const QColor base     = palette().color(QPalette::Window);
    const QColor accent   = palette().color(QPalette::Highlight);
    const QColor textCol  = palette().color(QPalette::WindowText);

    // --- Background ---
    p.fillRect(rect(), base);

    // --- Value label ---
    {
        p.save();
        p.setPen(hasFocus() ? accent : textCol);
        QFont f = font();
        f.setPointSize(std::min(f.pointSize(), 9));
        p.setFont(f);
        p.drawText(lr, Qt::AlignCenter, QString::number(value()));
        p.restore();
    }

    // --- Track groove ---
    {
        const int cx = tr.center().y();
        const int trackH = 3;
        QRect groove;
        if (orientation() == Qt::Horizontal)
            groove = QRect(tr.x() + 4, cx - trackH / 2, tr.width() - 8, trackH);
        else
        {
            const int cy = tr.center().x();
            groove = QRect(cy - trackH / 2, tr.y() + 4, trackH, tr.height() - 8);
        }
        p.setPen(Qt::NoPen);
        p.setBrush(base.darker(160));
        p.drawRoundedRect(groove, 1, 1);
    }

    // --- Slider knob ---
    {
        const int knobW = (orientation() == Qt::Horizontal) ? 10 : tr.width() - 4;
        const int knobH = (orientation() == Qt::Horizontal) ? tr.height() - 4 : 10;
        const int pos   = posFromValue(value());

        QRect knob;
        if (orientation() == Qt::Horizontal)
            knob = QRect(pos - knobW / 2, tr.y() + 2, knobW, knobH);
        else
            knob = QRect(tr.x() + 2, pos - knobH / 2, knobW, knobH);

        // Gradient fill matching FLTK SliderW: blend accent with dark/light
        const double vval = static_cast<double>(value() - minimum()) /
                            std::max(1, maximum() - minimum());
        QColor dark  = accent.darker(200);
        QColor light = accent.lighter(160);
        QColor top   = dark;
        QColor bot   = light;
        // Blend based on position (replicating FLTK fl_color_average)
        top.setRedF(dark.redF()   * (1.0 - vval) + light.redF()   * vval);
        top.setGreenF(dark.greenF() * (1.0 - vval) + light.greenF() * vval);
        top.setBlueF(dark.blueF()  * (1.0 - vval) + light.blueF()  * vval);
        bot.setRedF(light.redF()  * (1.0 - vval) + dark.redF()  * vval);
        bot.setGreenF(light.greenF() * (1.0 - vval) + dark.greenF() * vval);
        bot.setBlueF(light.blueF() * (1.0 - vval) + dark.blueF() * vval);

        QLinearGradient grad;
        if (orientation() == Qt::Horizontal)
        {
            grad.setStart(knob.left(), knob.top());
            grad.setFinalStop(knob.left(), knob.bottom());
        }
        else
        {
            grad.setStart(knob.left(), knob.top());
            grad.setFinalStop(knob.right(), knob.top());
        }
        grad.setColorAt(0.0, top);
        grad.setColorAt(1.0, bot);

        p.setPen(QPen(accent.darker(140), 1));
        p.setBrush(grad);
        p.drawRoundedRect(knob, 2, 2);
    }
}

// ---------------------------------------------------------------------------
// Mouse handling
// ---------------------------------------------------------------------------

void MidiSlider::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::RightButton)
    {
        // MIDI learn — emit signal for the containing panel to handle
        emit midiLearnRequested(this);
        event->accept();
        return;
    }

    if (event->button() == Qt::LeftButton)
    {
        const int pos = (orientation() == Qt::Horizontal)
                            ? event->position().toPoint().x()
                            : event->position().toPoint().y();
        const int knobPos = posFromValue(value());
        const int knobHalf = 6; // half knob width

        if (std::abs(pos - knobPos) <= knobHalf)
        {
            // Clicked on knob — start dragging with offset
            m_dragging   = true;
            m_dragOffset = pos - knobPos;
        }
        else
        {
            // Clicked on track — jump to position
            setValue(valueFromPos(pos));
            m_dragging   = true;
            m_dragOffset = 0;
        }
        setFocus(Qt::MouseFocusReason);
        event->accept();
        return;
    }

    QSlider::mousePressEvent(event);
}

void MidiSlider::mouseMoveEvent(QMouseEvent* event)
{
    if (m_dragging)
    {
        const int pos = (orientation() == Qt::Horizontal)
                            ? event->position().toPoint().x()
                            : event->position().toPoint().y();
        setValue(valueFromPos(pos - m_dragOffset));
        event->accept();
        return;
    }
    QSlider::mouseMoveEvent(event);
}

void MidiSlider::mouseReleaseEvent(QMouseEvent* event)
{
    if (m_dragging && event->button() == Qt::LeftButton)
    {
        m_dragging = false;
        event->accept();
        return;
    }
    QSlider::mouseReleaseEvent(event);
}

// ---------------------------------------------------------------------------
// Wheel
// ---------------------------------------------------------------------------

void MidiSlider::wheelEvent(QWheelEvent* event)
{
    const int steps = event->angleDelta().y() / 120;
    if (steps == 0)
    {
        event->ignore();
        return;
    }

    int step = 1;
    if (event->modifiers() & Qt::ShiftModifier)
        step = 10;
    else if (event->modifiers() & Qt::ControlModifier)
        step = 100;

    setValue(value() + steps * step);
    event->accept();
}

// ---------------------------------------------------------------------------
// Keyboard
// ---------------------------------------------------------------------------

void MidiSlider::keyPressEvent(QKeyEvent* event)
{
    int step = 1;
    if (event->modifiers() & Qt::ShiftModifier)
        step = 10;
    else if (event->modifiers() & Qt::ControlModifier)
        step = 100;

    switch (event->key())
    {
    case Qt::Key_Right:
    case Qt::Key_Up:
        setValue(value() + step);
        event->accept();
        return;
    case Qt::Key_Left:
    case Qt::Key_Down:
        setValue(value() - step);
        event->accept();
        return;
    case Qt::Key_Home:
        setValue(minimum());
        event->accept();
        return;
    case Qt::Key_End:
        setValue(maximum());
        event->accept();
        return;
    default:
        break;
    }
    QSlider::keyPressEvent(event);
}
