/*
  rakarrack - guitar multi-effects processor
  SPDX-License-Identifier: GPL-2.0-only

  Qt6 GUI — MidiSlider widget (replaces FLTK SliderW)

  A themed QSlider with integer value display, keyboard modifiers for
  step-size control, mouse-wheel support, and right-click MIDI learn.
  Visually matches the FLTK SliderW: gradient knob, tiled background,
  and value label area.
*/

#pragma once

#include <QSlider>

class QPaintEvent;
class QMouseEvent;
class QWheelEvent;
class QKeyEvent;

class MidiSlider : public QSlider
{
    Q_OBJECT
    Q_PROPERTY(int midiController READ midiController WRITE setMidiController NOTIFY midiControllerChanged)

public:
    explicit MidiSlider(Qt::Orientation orientation, QWidget* parent = nullptr);
    explicit MidiSlider(QWidget* parent = nullptr);
    ~MidiSlider() override = default;

    /// The MIDI CC number bound to this slider (-1 = none).
    [[nodiscard]] int midiController() const { return m_midiCC; }
    void setMidiController(int cc);

    /// Width (horizontal) or height (vertical) of the value label area in px.
    static constexpr int kLabelAreaSize = 35;

    /// Minimum size hint accounting for label + knob.
    [[nodiscard]] QSize minimumSizeHint() const override;
    [[nodiscard]] QSize sizeHint() const override;

Q_SIGNALS:
    void midiLearnRequested(MidiSlider* slider);
    void midiControllerChanged(int cc);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private:
    /// Compute the slider track rect (excluding the value label area).
    [[nodiscard]] QRect trackRect() const;
    /// Compute the value label rect.
    [[nodiscard]] QRect labelRect() const;
    /// Map a pixel coordinate to a slider value.
    [[nodiscard]] int valueFromPos(int pos) const;
    /// Map the current value to a pixel coordinate within the track.
    [[nodiscard]] int posFromValue(int val) const;

    int  m_midiCC     = -1;
    bool m_dragging   = false;
    int  m_dragOffset = 0;
};
