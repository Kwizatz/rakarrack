/*
  rakarrack - guitar multi-effects processor
  SPDX-License-Identifier: GPL-2.0-only

  Qt6 GUI — SpectrumAnalyzer widget (replaces FLTK Analyzer)

  28-band frequency display drawn as vertical bars.  Receives magnitude
  data via setData() and paints with QPainter.  Click to dismiss
  (matching FLTK behaviour).
*/

#pragma once

#include <QWidget>
#include <array>
#include <span>

class SpectrumAnalyzer : public QWidget
{
    Q_OBJECT

public:
    /// Number of frequency bands (matches FLTK Analyzer).
    static constexpr int kBandCount = 28;

    /// Center frequencies for the 28 bands (Hz).
    static constexpr std::array<int, kBandCount> kFrequencies = {
        22, 31, 39, 62, 79, 125, 158, 200, 251, 317,
        400, 503, 634, 800, 1000, 1200, 1500, 2000, 2500, 3200,
        4000, 5000, 6000, 8000, 10000, 12000, 16000, 20000
    };

    explicit SpectrumAnalyzer(QWidget* parent = nullptr);
    ~SpectrumAnalyzer() override = default;

    /// Set the 28-band magnitude data.  Values are 0.0 – 1.0 (linear).
    void setData(std::span<const float> bands);

    [[nodiscard]] QSize minimumSizeHint() const override { return {140, 24}; }
    [[nodiscard]] QSize sizeHint() const override { return {340, 28}; }

Q_SIGNALS:
    /// Emitted when the user clicks to dismiss the analyzer.
    void dismissed();

protected:
    void paintEvent(QPaintEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    std::array<float, kBandCount> m_bands{};
};
