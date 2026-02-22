/*
  rakarrack - guitar multi-effects processor
  SPDX-License-Identifier: GPL-2.0-only

  Qt6 GUI — VUMeter widget (replaces FLTK NewVum)

  Vertical bar-graph level meter with segmented LED look and four color
  zones (green → yellow → orange → red).  Display-only — ignores all
  mouse/keyboard input.

  Value range: -48 dB (empty) to +15 dB (clipping), matching the FLTK
  NewVum widget.
*/

#pragma once

#include <QWidget>

class VUMeter : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(float level READ level WRITE setLevel)

public:
    explicit VUMeter(QWidget* parent = nullptr);
    ~VUMeter() override = default;

    /// Current level in dB.  Clamped to [minDB, maxDB].
    [[nodiscard]] float level() const { return m_levelDB; }
    void setLevel(float dB);

    /// dB range matching FLTK NewVum.
    static constexpr float kMinDB =  -48.0F;
    static constexpr float kMaxDB =   15.0F;

    [[nodiscard]] QSize minimumSizeHint() const override { return {12, 60}; }
    [[nodiscard]] QSize sizeHint() const override { return {18, 120}; }

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    float m_levelDB = kMinDB;
};
