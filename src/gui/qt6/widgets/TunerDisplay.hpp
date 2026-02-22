/*
  rakarrack - guitar multi-effects processor
  SPDX-License-Identifier: GPL-2.0-only

  Qt6 GUI — TunerDisplay widget (replaces FLTK TunerLed)

  Shows the pitch deviation in cents as a horizontal LED indicator bar.
  A small red/green marker moves left/right of centre.  When the pitch
  is within ±5 cents the marker turns green; otherwise red.

  Display-only — ignores all mouse/keyboard input.
*/

#pragma once

#include <QWidget>
#include <QString>

class TunerDisplay : public QWidget
{
    Q_OBJECT

public:
    explicit TunerDisplay(QWidget* parent = nullptr);
    ~TunerDisplay() override = default;

    /// Update the tuner state.
    /// @param cents   Deviation from nearest note (-32..+32)
    /// @param noteName  e.g. "A4", "C#3"
    /// @param frequency Detected frequency in Hz
    void setTunerData(float cents, const QString& noteName, float frequency);

    [[nodiscard]] QSize minimumSizeHint() const override { return {120, 14}; }
    [[nodiscard]] QSize sizeHint() const override { return {205, 20}; }

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    float   m_cents      = 0.0F;
    QString m_noteName;
    float   m_frequency  = 0.0F;
};
