/*
  rakarrack - guitar multi-effects processor
  SPDX-License-Identifier: GPL-2.0-only

  Qt6 GUI — Modulation panels implementation
*/

#include "ModulationPanels.hpp"

// ═══════════════════════════════════════════════════════════════════════════
// Chorus (index 5) / Flanger (index 7) — same Chorus class
// LFO at P2-P5
// ═══════════════════════════════════════════════════════════════════════════

static constexpr ParamDesc kChorusParams[] = {
    { 0, "Volume",       0, 127},
    { 1, "Pan",          0, 127},
    { 2, "LFO Freq",     0, 127},
    { 3, "LFO Rand",     0, 127},
    { 4, "LFO Type",     0,  11, ParamDesc::Choice, kLfoTypeNames},
    { 5, "LFO Stereo",   0, 127},
    { 6, "Depth",        0, 127},
    { 7, "Delay",        0, 127},
    { 8, "Feedback",     0, 127},
    { 9, "LR Cross",     0, 127},
    {10, "Subtract",     0,   1, ParamDesc::Toggle},
    {11, "Intense",      0, 127},
};

ChorusPanel::ChorusPanel(EngineController& e, int effectIndex, QWidget* p)
    : SliderPanel(e, effectIndex, kChorusParams, p) {}

// ═══════════════════════════════════════════════════════════════════════════
// Phaser (index 6) — LFO at P2-P5
// ═══════════════════════════════════════════════════════════════════════════

static constexpr ParamDesc kPhaserParams[] = {
    { 0, "Volume",       0, 127},
    { 1, "Pan",          0, 127},
    { 2, "LFO Freq",     0, 127},
    { 3, "LFO Rand",     0, 127},
    { 4, "LFO Type",     0,  11, ParamDesc::Choice, kLfoTypeNames},
    { 5, "LFO Stereo",   0, 127},
    { 6, "Depth",        0, 127},
    { 7, "Feedback",     0, 127},
    { 8, "Stages",       0, 127},
    { 9, "LR Cross",     0, 127},
    {10, "Subtract",     0,   1, ParamDesc::Toggle},
    {11, "Phase",        0, 127},
};

PhaserPanel::PhaserPanel(EngineController& e, QWidget* p)
    : SliderPanel(e, 6, kPhaserParams, p) {}

// ═══════════════════════════════════════════════════════════════════════════
// Alienwah (index 11) — LFO at P2-P5
// ═══════════════════════════════════════════════════════════════════════════

static constexpr ParamDesc kAlienwahParams[] = {
    { 0, "Volume",       0, 127},
    { 1, "Pan",          0, 127},
    { 2, "LFO Freq",     0, 127},
    { 3, "LFO Rand",     0, 127},
    { 4, "LFO Type",     0,  11, ParamDesc::Choice, kLfoTypeNames},
    { 5, "LFO Stereo",   0, 127},
    { 6, "Depth",        0, 127},
    { 7, "Feedback",     0, 127},
    { 8, "Delay",        0, 127},
    { 9, "LR Cross",     0, 127},
    {10, "Phase",        0, 127},
};

AlienwahPanel::AlienwahPanel(EngineController& e, QWidget* p)
    : SliderPanel(e, 11, kAlienwahParams, p) {}

// ═══════════════════════════════════════════════════════════════════════════
// Pan (index 13) — LFO at P2-P5
// ═══════════════════════════════════════════════════════════════════════════

static constexpr ParamDesc kPanParams[] = {
    { 0, "Volume",       0, 127},
    { 1, "Pan",          0, 127},
    { 2, "LFO Freq",     0, 127},
    { 3, "LFO Rand",     0, 127},
    { 4, "LFO Type",     0,  11, ParamDesc::Choice, kLfoTypeNames},
    { 5, "LFO Stereo",   0, 127},
    { 6, "Extra Stereo", 0, 127},
};

PanPanel::PanPanel(EngineController& e, QWidget* p)
    : SliderPanel(e, 13, kPanParams, p) {}

// ═══════════════════════════════════════════════════════════════════════════
// APhaser — Analog Phaser (index 18) — LFO at P2-P5
// ═══════════════════════════════════════════════════════════════════════════

static constexpr ParamDesc kAPhaserParams[] = {
    { 0, "Volume",       0, 127},
    { 1, "Pan",          0, 127},
    { 2, "LFO Freq",     0, 127},
    { 3, "LFO Rand",     0, 127},
    { 4, "LFO Type",     0,  11, ParamDesc::Choice, kLfoTypeNames},
    { 5, "LFO Stereo",   0, 127},
    { 6, "Depth",        0, 127},
    { 7, "Feedback",     0, 127},
    { 8, "Stages",       0, 127},
    { 9, "LR Cross",     0, 127},
    {10, "Subtract",     0,   1, ParamDesc::Toggle},
    {11, "Phase",        0, 127},
    {12, "Hyper",        0,   1, ParamDesc::Toggle},
};

APhaserPanel::APhaserPanel(EngineController& e, QWidget* p)
    : SliderPanel(e, 18, kAPhaserParams, p) {}

// ═══════════════════════════════════════════════════════════════════════════
// Dual Flange (index 20) — non-standard LFO at P10-P13
// ═══════════════════════════════════════════════════════════════════════════

static constexpr ParamDesc kDualFlangeParams[] = {
    { 0, "Volume",       0, 127},
    { 1, "Pan",          0, 127},
    { 2, "LR Cross",     0, 127},
    { 3, "Depth",        0, 127},
    { 4, "Width",        0, 127},
    { 5, "Offset",       0, 127},
    { 6, "Feedback",     0, 127},
    { 7, "LPF",          0, 127},
    { 8, "Subtract",     0,   1, ParamDesc::Toggle},
    { 9, "Zero",         0, 127},
    {10, "LFO Freq",     0, 127},
    {11, "LFO Rand",     0, 127},
    {12, "LFO Type",     0,  11, ParamDesc::Choice, kLfoTypeNames},
    {13, "LFO Stereo",   0, 127},
    {14, "Intense",      0, 127},
};

DualFlangePanel::DualFlangePanel(EngineController& e, QWidget* p)
    : SliderPanel(e, 20, kDualFlangeParams, p) {}

// ═══════════════════════════════════════════════════════════════════════════
// Synthfilter (index 27) — LFO at P2-P5, envelope follower
// ═══════════════════════════════════════════════════════════════════════════

static constexpr ParamDesc kSynthfilterParams[] = {
    { 0, "Volume",       0, 127},
    { 1, "Distortion",   0, 127},
    { 2, "LFO Freq",     0, 127},
    { 3, "LFO Rand",     0, 127},
    { 4, "LFO Type",     0,  11, ParamDesc::Choice, kLfoTypeNames},
    { 5, "LFO Stereo",   0, 127},
    { 6, "Width",        0, 127},
    { 7, "Feedback",     0, 127},
    { 8, "LPF Stages",   0, 127},
    { 9, "HPF Stages",   0, 127},
    {10, "Subtract",     0,   1, ParamDesc::Toggle},
    {11, "Depth",        0, 127},
    {12, "Env Sens",     0, 127},
    {13, "Attack",       0, 127},
    {14, "Release",      0, 127},
    {15, "Offset",       0, 127},
};

SynthfilterPanel::SynthfilterPanel(EngineController& e, QWidget* p)
    : SliderPanel(e, 27, kSynthfilterParams, p) {}

// ═══════════════════════════════════════════════════════════════════════════
// Opticaltrem (index 44) — LFO at P1-P4
// ═══════════════════════════════════════════════════════════════════════════

static constexpr ParamDesc kOpticaltremParams[] = {
    { 0, "Depth",        0, 127},
    { 1, "LFO Freq",     0, 127},
    { 2, "LFO Rand",     0, 127},
    { 3, "LFO Type",     0,  11, ParamDesc::Choice, kLfoTypeNames},
    { 4, "LFO Stereo",   0, 127},
    { 5, "Pan",          0, 127},
    { 6, "Invert",       0,   1, ParamDesc::Toggle},
};

OpticaltremPanel::OpticaltremPanel(EngineController& e, QWidget* p)
    : SliderPanel(e, 44, kOpticaltremParams, p) {}

// ═══════════════════════════════════════════════════════════════════════════
// Vibe (index 45) — LFO at P1-P4
// ═══════════════════════════════════════════════════════════════════════════

static constexpr ParamDesc kVibeParams[] = {
    { 0, "Width",        0, 127},
    { 1, "LFO Freq",     0, 127},
    { 2, "LFO Rand",     0, 127},
    { 3, "LFO Type",     0,  11, ParamDesc::Choice, kLfoTypeNames},
    { 4, "LFO Stereo",   0, 127},
    { 5, "Pan",          0, 127},
    { 6, "Volume",       0, 127},
    { 7, "Feedback",     0, 127},
    { 8, "Depth",        0, 127},
    { 9, "LR Cross",     0, 127},
    {10, "Stereo",       0,   1, ParamDesc::Toggle},
};

VibePanel::VibePanel(EngineController& e, QWidget* p)
    : SliderPanel(e, 45, kVibeParams, p) {}

// ═══════════════════════════════════════════════════════════════════════════
// Infinity (index 46) — barber-pole phaser with 8 filter bands
// ═══════════════════════════════════════════════════════════════════════════

static constexpr ParamDesc kInfinityParams[] = {
    { 0, "Volume",       0, 127},
    { 1, "Band 1",       0, 127},
    { 2, "Band 2",       0, 127},
    { 3, "Band 3",       0, 127},
    { 4, "Band 4",       0, 127},
    { 5, "Band 5",       0, 127},
    { 6, "Band 6",       0, 127},
    { 7, "Band 7",       0, 127},
    { 8, "Band 8",       0, 127},
    { 9, "Q",            0, 127},
    {10, "Start Freq",   0, 127},
    {11, "End Freq",     0, 127},
    {12, "Rate",         0, 127},
    {13, "Stereo Diff",  0, 127},
    {14, "Subdivision",  0, 127},
    {15, "Auto Pan",     0, 127},
    {16, "Reverse",      0,   1, ParamDesc::Toggle},
    {17, "Stages",       0, 127},
};

InfinityPanel::InfinityPanel(EngineController& e, QWidget* p)
    : SliderPanel(e, 46, kInfinityParams, p) {}
