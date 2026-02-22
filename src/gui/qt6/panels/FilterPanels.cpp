/*
  rakarrack - guitar multi-effects processor
  SPDX-License-Identifier: GPL-2.0-only

  Qt6 GUI — Filter panels implementation
*/

#include "FilterPanels.hpp"

// ═══════════════════════════════════════════════════════════════════════════
// WahWah / DynamicFilter (index 10) — LFO at P2-P5
// ═══════════════════════════════════════════════════════════════════════════

static constexpr ParamDesc kWahWahParams[] = {
    { 0, "Volume",       0, 127},
    { 1, "Pan",          0, 127},
    { 2, "LFO Freq",     0, 127},
    { 3, "LFO Rand",     0, 127},
    { 4, "LFO Type",     0,  11, ParamDesc::Choice, kLfoTypeNames},
    { 5, "LFO Stereo",   0, 127},
    { 6, "Depth",        0, 127},
    { 7, "Amp Sens",     0, 127},
    { 8, "Amp Smooth",   0,   1, ParamDesc::Toggle},
    { 9, "Amp Inv",      0,   1, ParamDesc::Toggle},
};

WahWahPanel::WahWahPanel(EngineController& e, QWidget* p)
    : SliderPanel(e, 10, kWahWahParams, p) {}

// ═══════════════════════════════════════════════════════════════════════════
// RyanWah (index 31) — LFO at P2-P5, LP/BP/HP mix
// ═══════════════════════════════════════════════════════════════════════════

static constexpr ParamDesc kRyanWahParams[] = {
    { 0, "Volume",       0, 127},
    { 1, "Pan",          0, 127},
    { 2, "LFO Freq",     0, 127},
    { 3, "LFO Rand",     0, 127},
    { 4, "LFO Type",     0,  11, ParamDesc::Choice, kLfoTypeNames},
    { 5, "LFO Stereo",   0, 127},
    { 6, "Depth",        0, 127},
    { 7, "Amp Sens",     0, 127},
    { 8, "Smooth",       0,   1, ParamDesc::Toggle},
    { 9, "LP Mix",       0, 127},
    {10, "BP Mix",       0, 127},
    {11, "HP Mix",       0, 127},
    {12, "Freq 1",       0, 127},
    {13, "Q 1",          0, 127},
    {14, "Stages",       0, 127},
    {15, "Range",        0, 127},
    {16, "Env Sens",     0, 127},
    {17, "Attack",       0, 127},
    {18, "Release",      0, 127},
};

RyanWahPanel::RyanWahPanel(EngineController& e, QWidget* p)
    : SliderPanel(e, 31, kRyanWahParams, p) {}

// ═══════════════════════════════════════════════════════════════════════════
// CoilCrafter (index 33)
// ═══════════════════════════════════════════════════════════════════════════

static constexpr ParamDesc kCoilCrafterParams[] = {
    { 0, "Volume",     0, 127},
    { 1, "Tone",       0, 127},
    { 2, "Freq 1",     0, 127},
    { 3, "Q 1",        0, 127},
    { 4, "Freq 2",     0, 127},
    { 5, "Q 2",        0, 127},
    { 6, "NF Freq",    0, 127},
    { 7, "NF Gain",    0, 127},
    { 8, "NF Q",       0, 127},
};

CoilCrafterPanel::CoilCrafterPanel(EngineController& e, QWidget* p)
    : SliderPanel(e, 33, kCoilCrafterParams, p) {}

// ═══════════════════════════════════════════════════════════════════════════
// ShelfBoost (index 34)
// ═══════════════════════════════════════════════════════════════════════════

static constexpr ParamDesc kShelfBoostParams[] = {
    { 0, "Volume",     0, 127},
    { 1, "Gain",       0, 127},
    { 2, "Frequency",  0, 127},
    { 3, "Slope",      0, 127},
    { 4, "Q",          0, 127},
};

ShelfBoostPanel::ShelfBoostPanel(EngineController& e, QWidget* p)
    : SliderPanel(e, 34, kShelfBoostParams, p) {}
