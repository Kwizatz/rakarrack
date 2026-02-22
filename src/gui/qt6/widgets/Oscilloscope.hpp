/*
  rakarrack - guitar multi-effects processor
  SPDX-License-Identifier: GPL-2.0-only

  Qt6 GUI — Oscilloscope widget (replaces FLTK Scope)

  Dual-channel waveform display.  Left half shows the right channel,
  right half shows the left channel (matching the FLTK layout).
  Click to dismiss.
*/

#pragma once

#include <QWidget>
#include <vector>

class Oscilloscope : public QWidget
{
    Q_OBJECT

public:
    explicit Oscilloscope(QWidget* parent = nullptr);
    ~Oscilloscope() override = default;

    /// Set waveform data for both channels.
    /// Samples are expected in the range [-1.0, +1.0].
    void setData(const float* left, const float* right, int sampleCount);

    [[nodiscard]] QSize minimumSizeHint() const override { return {100, 40}; }
    [[nodiscard]] QSize sizeHint() const override { return {226, 65}; }

Q_SIGNALS:
    /// Emitted when the user clicks to dismiss.
    void dismissed();

protected:
    void paintEvent(QPaintEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    std::vector<float> m_left;
    std::vector<float> m_right;
};
