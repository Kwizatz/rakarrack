/*
  rakarrack - guitar multi-effects processor
  SPDX-License-Identifier: GPL-2.0-only

  Qt6 GUI — Pitch panels implementation
*/

#include "PitchPanels.hpp"

// ═══════════════════════════════════════════════════════════════════════════
// Harmonizer (index 14)
// Interval 0-24 where 12=unison, Note/Type/Select as choices
// ═══════════════════════════════════════════════════════════════════════════

static constexpr ParamDesc kHarmonizerParams[] = {
    { 0, "Volume",       0, 127},
    { 1, "Pan",          0, 127},
    { 2, "Gain",         0, 127},
    { 3, "Interval",     0,  24, ParamDesc::Choice},  // 12=unison
    { 4, "Filter Freq",  0, 127},
    { 5, "Select",       0,   1, ParamDesc::Toggle},
    { 6, "Note",         0,  11, ParamDesc::Choice},   // 12 notes
    { 7, "Type",         0,   2, ParamDesc::Choice},
    { 8, "Filter Gain",  0, 127},
    { 9, "Filter Q",     0, 127},
    {10, "MIDI",         0,   1, ParamDesc::Toggle},
};

HarmonizerPanel::HarmonizerPanel(EngineController& e, QWidget* p)
    : SliderPanel(e, 14, kHarmonizerParams, p) {}

// ═══════════════════════════════════════════════════════════════════════════
// Ring (index 21) — ring modulator with waveform mix
// ═══════════════════════════════════════════════════════════════════════════

static constexpr ParamDesc kRingParams[] = {
    { 0, "Volume",     0, 127},
    { 1, "Pan",        0, 127},
    { 2, "LR Cross",   0, 127},
    { 3, "Level",      0, 127},
    { 4, "Input",      0, 127},
    { 5, "Freq",       0, 127},
    { 6, "Stereo",     0,   1, ParamDesc::Toggle},
    { 7, "Depth",      0, 127},
    { 8, "Sine",       0, 127},
    { 9, "Triangle",   0, 127},
    {10, "Saw",        0, 127},
    {11, "Square",     0, 127},
    {12, "Auto Freq",  0,   1, ParamDesc::Toggle},
};

RingPanel::RingPanel(EngineController& e, QWidget* p)
    : SliderPanel(e, 21, kRingParams, p) {}

// ═══════════════════════════════════════════════════════════════════════════
// Shifter (index 38) — pitch shifter with whammy mode
// ═══════════════════════════════════════════════════════════════════════════

static constexpr ParamDesc kShifterParams[] = {
    { 0, "Volume",      0, 127},
    { 1, "Pan",         0, 127},
    { 2, "Gain",        0, 127},
    { 3, "Interval",    0,  24, ParamDesc::Choice},
    { 4, "Filter Freq", 0, 127},
    { 5, "Mode",        0,   5, ParamDesc::Choice},
    { 6, "Note",        0,  11, ParamDesc::Choice},
    { 7, "Type",        0,   2, ParamDesc::Choice},
    { 8, "Filter Gain", 0, 127},
    { 9, "Filter Q",    0, 127},
    {10, "Attack",      0, 127},
    {11, "Decay",       0, 127},
    {12, "Threshold",   0, 127},
    {13, "Whammy",      0,   1, ParamDesc::Toggle},
};

ShifterPanel::ShifterPanel(EngineController& e, QWidget* p)
    : SliderPanel(e, 38, kShifterParams, p) {}

// ═══════════════════════════════════════════════════════════════════════════
// StereoHarm (index 42) — dual independent pitch shifters (L/R)
// ═══════════════════════════════════════════════════════════════════════════

static constexpr ParamDesc kStereoHarmParams[] = {
    { 0, "Volume",       0, 127},
    { 1, "Gain L",       0, 127},
    { 2, "Interval L",   0,  24, ParamDesc::Choice},
    { 3, "Chrome L",     0, 127},
    { 4, "Gain R",       0, 127},
    { 5, "Interval R",   0,  24, ParamDesc::Choice},
    { 6, "Chrome R",     0, 127},
    { 7, "Select",       0,   1, ParamDesc::Toggle},
    { 8, "Note",         0,  11, ParamDesc::Choice},
    { 9, "Type",         0,   2, ParamDesc::Choice},
    {10, "MIDI",         0,   1, ParamDesc::Toggle},
    {11, "LR Cross",     0, 127},
};

StereoHarmPanel::StereoHarmPanel(EngineController& e, QWidget* p)
    : SliderPanel(e, 42, kStereoHarmParams, p) {}
