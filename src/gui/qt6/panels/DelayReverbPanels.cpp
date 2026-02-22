/*
  rakarrack - guitar multi-effects processor
  SPDX-License-Identifier: GPL-2.0-only

  Qt6 GUI — Delay & Reverb panels implementation
*/

#include "DelayReverbPanels.hpp"

// ═══════════════════════════════════════════════════════════════════════════
// Echo (index 4)
// ═══════════════════════════════════════════════════════════════════════════

static constexpr ParamDesc kEchoParams[] = {
    { 0, "Volume",     0, 127},
    { 1, "Pan",        0, 127},
    { 2, "Delay",      0, 127},
    { 3, "LR Delay",   0, 127},
    { 4, "LR Cross",   0, 127},
    { 5, "Feedback",   0, 127},
    { 6, "Damp",       0, 127},
    { 7, "Direct",     0,   1, ParamDesc::Toggle},
};

EchoPanel::EchoPanel(EngineController& e, QWidget* p)
    : SliderPanel(e, 4, kEchoParams, p) {}

// ═══════════════════════════════════════════════════════════════════════════
// Reverb (index 8)
// ═══════════════════════════════════════════════════════════════════════════

static constexpr const char* kReverbTypes[] = {
    "Random", "Freeverb", "Bandwidth",
};

static constexpr ParamDesc kReverbParams[] = {
    { 0, "Volume",       0, 127},
    { 1, "Pan",          0, 127},
    { 2, "Time",         0, 127},
    { 3, "Initial Delay",0, 127},
    { 4, "Init Dly Fb",  0, 127},
    { 5, "unused1",      0, 127},
    { 6, "unused2",      0, 127},
    { 7, "LPF",          0, 127},
    { 8, "HPF",          0, 127},
    { 9, "Damp",         0, 127},
    {10, "Type",         0,   2, ParamDesc::Choice, kReverbTypes},
    {11, "Room Size",    0, 127},
};

ReverbPanel::ReverbPanel(EngineController& e, QWidget* p)
    : SliderPanel(e, 8, kReverbParams, p) {}

// ═══════════════════════════════════════════════════════════════════════════
// MusicDelay (index 15) — dual delay channels, tempo-synced
// ═══════════════════════════════════════════════════════════════════════════

static constexpr ParamDesc kMusicDelayParams[] = {
    { 0, "Volume",     0, 127},
    { 1, "Pan 1",      0, 127},
    { 2, "Delay 1",    0, 127},
    { 3, "Pan 2",      0, 127},
    { 4, "Delay 2",    0, 127},
    { 5, "LR Cross",   0, 127},
    { 6, "Feedback 1", 0, 127},
    { 7, "Feedback 2", 0, 127},
    { 8, "Damp",       0, 127},
    { 9, "Tempo",      0, 127},
    {10, "Direct",     0,   1, ParamDesc::Toggle},
    {11, "Gain 1",     0, 127},
    {12, "Gain 2",     0, 127},
};

MusicDelayPanel::MusicDelayPanel(EngineController& e, QWidget* p)
    : SliderPanel(e, 15, kMusicDelayParams, p) {}

// ═══════════════════════════════════════════════════════════════════════════
// Arpie (index 24) — tempo-synced arpeggio delay
// ═══════════════════════════════════════════════════════════════════════════

static constexpr ParamDesc kArpieParams[] = {
    { 0, "Volume",     0, 127},
    { 1, "Pan",        0, 127},
    { 2, "Delay",      0, 127},
    { 3, "LR Delay",   0, 127},
    { 4, "LR Cross",   0, 127},
    { 5, "Feedback",   0, 127},
    { 6, "Damp",       0, 127},
    { 7, "Pattern",    0, 127},
    { 8, "Subdivision",0, 127},
    { 9, "Harm",       0, 127},
};

ArpiePanel::ArpiePanel(EngineController& e, QWidget* p)
    : SliderPanel(e, 24, kArpieParams, p) {}

// ═══════════════════════════════════════════════════════════════════════════
// Convolotron (index 29) — convolution with file selector
// ═══════════════════════════════════════════════════════════════════════════

static constexpr ParamDesc kConvolotronParams[] = {
    { 0, "Volume",      0, 127},
    { 1, "Pan",         0, 127},
    { 2, "Safe",        0,   1, ParamDesc::Toggle},
    { 3, "Length",      0, 127},
    { 4, "User",        0,   1, ParamDesc::Toggle},
    { 5, "Damp",        0, 127},
    { 6, "Feedback",    0, 127},
    { 7, "Level",       0, 127},
    { 8, "File",        0,  10, ParamDesc::Choice},  // built-in file index
    { 9, "LPF",         0, 127},
    {10, "HPF",         0, 127},
    {11, "Stretch",     0, 127},
};

ConvolotronPanel::ConvolotronPanel(EngineController& e, QWidget* p)
    : SliderPanel(e, 29, kConvolotronParams, p) {}

// ═══════════════════════════════════════════════════════════════════════════
// RBEcho (index 32) — tempo-synced echo
// ═══════════════════════════════════════════════════════════════════════════

static constexpr ParamDesc kRBEchoParams[] = {
    { 0, "Volume",      0, 127},
    { 1, "Pan",         0, 127},
    { 2, "Delay",       0, 127},
    { 3, "LR Delay",    0, 127},
    { 4, "LR Cross",    0, 127},
    { 5, "Feedback",    0, 127},
    { 6, "Damp",        0, 127},
    { 7, "Tempo",       0, 127},
    { 8, "Subdivision", 0, 127},
    { 9, "Angle",       0, 127},
    {10, "Direct",      0,   1, ParamDesc::Toggle},
};

RBEchoPanel::RBEchoPanel(EngineController& e, QWidget* p)
    : SliderPanel(e, 32, kRBEchoParams, p) {}

// ═══════════════════════════════════════════════════════════════════════════
// Reverbtron (index 40) — convolution reverb with file selector
// ═══════════════════════════════════════════════════════════════════════════

static constexpr ParamDesc kReverbtronParams[] = {
    { 0, "Volume",       0, 127},
    { 1, "Fade",         0, 127},
    { 2, "Safe",         0,   1, ParamDesc::Toggle},
    { 3, "Length",       0, 127},
    { 4, "User",         0,   1, ParamDesc::Toggle},
    { 5, "Init Delay",   0, 127},
    { 6, "Hi Damp",      0, 127},
    { 7, "Level",        0, 127},
    { 8, "File",         0,  10, ParamDesc::Choice},
    { 9, "Stretch",      0, 127},
    {10, "Feedback",     0, 127},
    {11, "Panning",      0, 127},
    {12, "Extra Stereo", 0,   1, ParamDesc::Toggle},
    {13, "Reverse",      0,   1, ParamDesc::Toggle},
    {14, "LPF",          0, 127},
    {15, "Diffusion",    0, 127},
};

ReverbtronPanel::ReverbtronPanel(EngineController& e, QWidget* p)
    : SliderPanel(e, 40, kReverbtronParams, p) {}

// ═══════════════════════════════════════════════════════════════════════════
// Echotron (index 41) — patterned delay with file + LFO
// ═══════════════════════════════════════════════════════════════════════════

static constexpr ParamDesc kEchotronParams[] = {
    { 0, "Volume",       0, 127},
    { 1, "Depth",        0, 127},
    { 2, "Width",        0, 127},
    { 3, "Length",       0, 127},
    { 4, "User",         0,   1, ParamDesc::Toggle},
    { 5, "Tempo",        0, 127},
    { 6, "Hi Damp",      0, 127},
    { 7, "LR Cross",     0, 127},
    { 8, "File",         0,  10, ParamDesc::Choice},
    { 9, "LFO Stereo",   0, 127},
    {10, "Feedback",     0, 127},
    {11, "Panning",      0, 127},
    {12, "Mod Delay",    0,   1, ParamDesc::Toggle},
    {13, "Mod Filters",  0,   1, ParamDesc::Toggle},
    {14, "LFO Type",     0,  11, ParamDesc::Choice, kLfoTypeNames},
    {15, "Filters",      0,   1, ParamDesc::Toggle},
};

EchotronPanel::EchotronPanel(EngineController& e, QWidget* p)
    : SliderPanel(e, 41, kEchotronParams, p) {}
