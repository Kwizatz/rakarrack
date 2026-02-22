/*
  rakarrack - guitar multi-effects processor
  SPDX-License-Identifier: GPL-2.0-only

  Qt6 GUI — Dynamics panels implementation
*/

#include "DynamicsPanels.hpp"

// ═══════════════════════════════════════════════════════════════════════════
// Compressor (index 1) — params start at index 1
// ═══════════════════════════════════════════════════════════════════════════

static constexpr ParamDesc kCompressorParams[] = {
    { 1, "Threshold",  -60,   0},
    { 2, "Ratio",        1,  20},
    { 3, "Output",     -40,  15},
    { 4, "Attack",       1, 1000},
    { 5, "Release",     10, 2000},
    { 6, "Auto Output",  0,   1, ParamDesc::Toggle},
    { 7, "Knee",         0, 100},
    { 8, "Stereo",       0,   1, ParamDesc::Toggle},
    { 9, "Peak",         0,   1, ParamDesc::Toggle},
};

CompressorPanel::CompressorPanel(EngineController& e, QWidget* p)
    : SliderPanel(e, 1, kCompressorParams, p) {}

// ═══════════════════════════════════════════════════════════════════════════
// Gate (index 16) — params start at index 1
// ═══════════════════════════════════════════════════════════════════════════

static constexpr ParamDesc kGateParams[] = {
    { 1, "Threshold", -70,    20},
    { 2, "Range",     -90,     0},
    { 3, "Attack",      1,  5000},
    { 4, "Decay",       1,  5000},
    { 5, "LPF",        20, 26000},
    { 6, "HPF",        20, 20000},
    { 7, "Hold",        1,   500},
};

GatePanel::GatePanel(EngineController& e, QWidget* p)
    : SliderPanel(e, 16, kGateParams, p) {}

// ═══════════════════════════════════════════════════════════════════════════
// Expander (index 25) — params start at index 1
// ═══════════════════════════════════════════════════════════════════════════

static constexpr ParamDesc kExpanderParams[] = {
    { 1, "Threshold", -80,     0},
    { 2, "Shape",       0,    50},
    { 3, "Attack",     10,  2000},
    { 4, "Decay",      10,   500},
    { 5, "LPF",        20, 26000},
    { 6, "HPF",        20, 20000},
    { 7, "Level",       0,   127},
};

ExpanderPanel::ExpanderPanel(EngineController& e, QWidget* p)
    : SliderPanel(e, 25, kExpanderParams, p) {}

// ═══════════════════════════════════════════════════════════════════════════
// CompBand — Multiband Compressor (index 43)
// ═══════════════════════════════════════════════════════════════════════════

static constexpr ParamDesc kCompBandParams[] = {
    { 0, "Volume",           0, 127},
    { 1, "Low Ratio",        0, 127},
    { 2, "Mid-Low Ratio",    0, 127},
    { 3, "Mid-High Ratio",   0, 127},
    { 4, "High Ratio",       0, 127},
    { 5, "Low Threshold",    0, 127},
    { 6, "Mid-Low Thresh",   0, 127},
    { 7, "Mid-High Thresh",  0, 127},
    { 8, "High Threshold",   0, 127},
    { 9, "Cross 1",          0, 127},
    {10, "Cross 2",          0, 127},
    {11, "Cross 3",          0, 127},
    {12, "Level",            0, 127},
};

CompBandPanel::CompBandPanel(EngineController& e, QWidget* p)
    : SliderPanel(e, 43, kCompBandParams, p) {}
