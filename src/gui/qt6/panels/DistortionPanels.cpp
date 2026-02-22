/*
  rakarrack - guitar multi-effects processor
  SPDX-License-Identifier: GPL-2.0-only

  Qt6 GUI — Distortion panels implementation
*/

#include "DistortionPanels.hpp"

// ═══════════════════════════════════════════════════════════════════════════
// Distorsion (index 2) / Overdrive (index 3) — same Distorsion class
// ═══════════════════════════════════════════════════════════════════════════

static constexpr ParamDesc kDistorsionParams[] = {
    { 0, "Volume",      0, 127},
    { 1, "Pan",         0, 127},
    { 2, "LR Cross",    0, 127},
    { 3, "Drive",       0, 127},
    { 4, "Level",       0, 127},
    { 5, "Type",        0,  29, ParamDesc::Choice},  // 30 waveshape types
    { 6, "Negate",      0,   1, ParamDesc::Toggle},
    { 7, "LPF",         0, 127},
    { 8, "HPF",         0, 127},
    { 9, "Stereo",      0,   1, ParamDesc::Toggle},
    {10, "Prefilter",   0,   1, ParamDesc::Toggle},
    {12, "Sub Octave",  0, 127},
};

DistorsionPanel::DistorsionPanel(EngineController& e, int effectIndex,
                                 QWidget* p)
    : SliderPanel(e, effectIndex, kDistorsionParams, p) {}

// ═══════════════════════════════════════════════════════════════════════════
// NewDist (index 17) — Distorsion variant with resonance
// ═══════════════════════════════════════════════════════════════════════════

static constexpr ParamDesc kNewDistParams[] = {
    { 0, "Volume",      0, 127},
    { 1, "Pan",         0, 127},
    { 2, "LR Cross",    0, 127},
    { 3, "Drive",       0, 127},
    { 4, "Level",       0, 127},
    { 5, "Type",        0,  29, ParamDesc::Choice},
    { 6, "Negate",      0,   1, ParamDesc::Toggle},
    { 7, "LPF",         0, 127},
    { 8, "HPF",         0, 127},
    { 9, "Stereo",      0,   1, ParamDesc::Toggle},
    {10, "Prefilter",   0,   1, ParamDesc::Toggle},
    {12, "Sub Octave",  0, 127},
    {13, "Mid",         0, 127},
    {14, "HPF2",        0, 127},
};

NewDistPanel::NewDistPanel(EngineController& e, QWidget* p)
    : SliderPanel(e, 17, kNewDistParams, p) {}

// ═══════════════════════════════════════════════════════════════════════════
// Valve (index 19)
// ═══════════════════════════════════════════════════════════════════════════

static constexpr ParamDesc kValveParams[] = {
    { 0, "Volume",      0, 127},
    { 1, "Pan",         0, 127},
    { 2, "LR Cross",    0, 127},
    { 3, "Drive",       0, 127},
    { 4, "Level",       0, 127},
    { 5, "Type",        0,  29, ParamDesc::Choice},
    { 6, "Negate",      0,   1, ParamDesc::Toggle},
    { 7, "LPF",         0, 127},
    { 8, "HPF",         0, 127},
    { 9, "Stereo",      0,   1, ParamDesc::Toggle},
    {10, "Prefilter",   0,   1, ParamDesc::Toggle},
    {11, "Presence",    0, 127},
    {12, "Sub Octave",  0, 127},
};

ValvePanel::ValvePanel(EngineController& e, QWidget* p)
    : SliderPanel(e, 19, kValveParams, p) {}

// ═══════════════════════════════════════════════════════════════════════════
// MBDist — Multiband Distortion (index 23)
// ═══════════════════════════════════════════════════════════════════════════

static constexpr ParamDesc kMBDistParams[] = {
    { 0, "Volume",       0, 127},
    { 1, "Pan",          0, 127},
    { 2, "LR Cross",     0, 127},
    { 3, "Drive",        0, 127},
    { 4, "Level",        0, 127},
    { 5, "Type Low",     0,  29, ParamDesc::Choice},
    { 6, "Type Mid",     0,  29, ParamDesc::Choice},
    { 7, "Type High",    0,  29, ParamDesc::Choice},
    { 8, "Vol Low",      0, 127},
    { 9, "Vol Mid",      0, 127},
    {10, "Vol High",     0, 127},
    {11, "Cross 1",      0, 127},
    {12, "Cross 2",      0, 127},
    {13, "Negate",       0,   1, ParamDesc::Toggle},
    {14, "Stereo",       0,   1, ParamDesc::Toggle},
};

MBDistPanel::MBDistPanel(EngineController& e, QWidget* p)
    : SliderPanel(e, 23, kMBDistParams, p) {}

// ═══════════════════════════════════════════════════════════════════════════
// StompBox (index 39)
// ═══════════════════════════════════════════════════════════════════════════

static constexpr const char* kStompModes[] = {
    "Odie", "Grunge", "Rat", "FatCat",
    "Dist+", "DeathMetal", "MetalZone", "ClassicFuzz",
};

static constexpr ParamDesc kStompBoxParams[] = {
    { 0, "Volume",  0, 127},
    { 1, "High",    0, 127},
    { 2, "Mid",     0, 127},
    { 3, "Low",     0, 127},
    { 4, "Gain",    0, 127},
    { 5, "Mode",    0,   7, ParamDesc::Choice, kStompModes},
};

StompBoxPanel::StompBoxPanel(EngineController& e, QWidget* p)
    : SliderPanel(e, 39, kStompBoxParams, p) {}
