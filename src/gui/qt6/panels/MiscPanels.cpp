/*
  rakarrack - guitar multi-effects processor
  SPDX-License-Identifier: GPL-2.0-only

  Qt6 GUI — Misc panels implementation
*/

#include "MiscPanels.hpp"

// ═══════════════════════════════════════════════════════════════════════════
// Exciter / HarmonicEnhancer (index 22) — 10 harmonic level sliders
// ═══════════════════════════════════════════════════════════════════════════

static constexpr ParamDesc kExciterParams[] = {
    { 0, "Volume",      0, 127},
    { 1, "Harm 1",      0, 127},
    { 2, "Harm 2",      0, 127},
    { 3, "Harm 3",      0, 127},
    { 4, "Harm 4",      0, 127},
    { 5, "Harm 5",      0, 127},
    { 6, "Harm 6",      0, 127},
    { 7, "Harm 7",      0, 127},
    { 8, "Harm 8",      0, 127},
    { 9, "Harm 9",      0, 127},
    {10, "Harm 10",     0, 127},
    {11, "LPF",         0, 127},
    {12, "HPF",         0, 127},
};

ExciterPanel::ExciterPanel(EngineController& e, QWidget* p)
    : SliderPanel(e, 22, kExciterParams, p) {}

// ═══════════════════════════════════════════════════════════════════════════
// Shuffle (index 26) — 4-band gain with crossover frequencies
// ═══════════════════════════════════════════════════════════════════════════

static constexpr ParamDesc kShuffleParams[] = {
    { 0, "Volume",      0, 127},
    { 1, "Low",         0, 127},
    { 2, "Mid-Low",     0, 127},
    { 3, "Mid-High",    0, 127},
    { 4, "High",        0, 127},
    { 5, "Cross 1",     0, 127},
    { 6, "Cross 2",     0, 127},
    { 7, "Cross 3",     0, 127},
    { 8, "Q",           0, 127},
    { 9, "Gain",        0, 127},
};

ShufflePanel::ShufflePanel(EngineController& e, QWidget* p)
    : SliderPanel(e, 26, kShuffleParams, p) {}

// ═══════════════════════════════════════════════════════════════════════════
// MBVvol — Multiband Volume (index 28) — dual LFO sections
// ═══════════════════════════════════════════════════════════════════════════

static constexpr ParamDesc kMBVvolParams[] = {
    { 0, "Volume",         0, 127},
    { 1, "LFO1 Freq",      0, 127},
    { 2, "LFO1 Rand",      0, 127},
    { 3, "LFO1 Type",      0,  11, ParamDesc::Choice, kLfoTypeNames},
    { 4, "LFO1 Stereo",    0, 127},
    { 5, "Cross 1",        0, 127},
    { 6, "Cross 2",        0, 127},
    { 7, "Vol Low",        0, 127},
    { 8, "Vol Mid",        0, 127},
    { 9, "Vol High",       0, 127},
    {10, "Combi Mode",     0, 127},
    {11, "LFO2 Freq",      0, 127},
    {12, "LFO2 Rand",      0, 127},
    {13, "LFO2 Type",      0,  11, ParamDesc::Choice, kLfoTypeNames},
    {14, "LFO2 Stereo",    0, 127},
};

MBVvolPanel::MBVvolPanel(EngineController& e, QWidget* p)
    : SliderPanel(e, 28, kMBVvolParams, p) {}

// ═══════════════════════════════════════════════════════════════════════════
// Looper (index 30) — transport controls + 2-track looper
// ═══════════════════════════════════════════════════════════════════════════

static constexpr const char* kBarChoices[] = {
    "2", "3", "4", "5", "6", "7", "9", "11",
};

static constexpr const char* kMeterSources[] = {
    "Bar", "1", "Free",
};

static constexpr ParamDesc kLooperParams[] = {
    { 0, "Volume",       0, 127},
    { 1, "Play",         0,   1, ParamDesc::Toggle},
    { 2, "Stop",         0,   1, ParamDesc::Toggle},
    { 3, "Record",       0,   1, ParamDesc::Toggle},
    { 4, "Clear",        0,   1, ParamDesc::Toggle},
    { 5, "Reverse",      0,   1, ParamDesc::Toggle},
    { 6, "Fade Trk1",    0, 127},
    { 7, "Track 1",      0,   1, ParamDesc::Toggle},
    { 8, "Track 2",      0,   1, ParamDesc::Toggle},
    { 9, "Autoplay",     0,   1, ParamDesc::Toggle},
    {10, "Fade Trk2",    0, 127},
    {11, "Rec Trk1",     0,   1, ParamDesc::Toggle},
    {12, "Rec Trk2",     0,   1, ParamDesc::Toggle},
    {13, "Link",         0,   1, ParamDesc::Toggle},
    {14, "Tempo",       20, 300},
    {15, "Bar",          0,   7, ParamDesc::Choice, kBarChoices},
    {16, "Metronome",    0,   1, ParamDesc::Toggle},
    {17, "Meter Src",    0,   2, ParamDesc::Choice, kMeterSources},
};

LooperPanel::LooperPanel(EngineController& e, QWidget* p)
    : SliderPanel(e, 30, kLooperParams, p) {}

// ═══════════════════════════════════════════════════════════════════════════
// Vocoder (index 35) — requires aux input
// ═══════════════════════════════════════════════════════════════════════════

static constexpr ParamDesc kVocoderParams[] = {
    { 0, "Volume",       0, 127},
    { 1, "Pan",          0, 127},
    { 2, "Input",        0, 127},
    { 3, "Q",            0, 127},
    { 4, "Ring",         0, 127},
    { 5, "Level",        0, 127},
    { 6, "Muf",          0, 127},
};

VocoderPanel::VocoderPanel(EngineController& e, QWidget* p)
    : SliderPanel(e, 35, kVocoderParams, p) {}

// ═══════════════════════════════════════════════════════════════════════════
// Sustainer (index 36) — simplest effect (2 params)
// ═══════════════════════════════════════════════════════════════════════════

static constexpr ParamDesc kSustainerParams[] = {
    { 0, "Volume",   0, 127},
    { 1, "Sustain",  0, 127},
};

SustainerPanel::SustainerPanel(EngineController& e, QWidget* p)
    : SliderPanel(e, 36, kSustainerParams, p) {}

// ═══════════════════════════════════════════════════════════════════════════
// Sequence (index 37) — 8-step sequencer
// ═══════════════════════════════════════════════════════════════════════════

static constexpr const char* kSequenceModes[] = {
    "Linear",      "Up/Down",     "Stepper",   "Shifter",
    "Tremor",      "Arpeggiator", "Chorus",    "TrigStepper",
    "Delay",
};

static constexpr const char* kSequenceRanges[] = {
    "1", "2", "3", "4", "5", "6", "7", "8",
};

static constexpr ParamDesc kSequenceParams[] = {
    { 0, "Step 1",       0, 127},
    { 1, "Step 2",       0, 127},
    { 2, "Step 3",       0, 127},
    { 3, "Step 4",       0, 127},
    { 4, "Step 5",       0, 127},
    { 5, "Step 6",       0, 127},
    { 6, "Step 7",       0, 127},
    { 7, "Step 8",       0, 127},
    { 8, "Volume",       0, 127},
    { 9, "Tempo",       20, 300},
    {10, "Q",            0, 127},
    {11, "Amplitude",    0,   1, ParamDesc::Toggle},
    {12, "Stereo Diff",  0,   7, ParamDesc::Choice},
    {13, "Mode",         0,   8, ParamDesc::Choice, kSequenceModes},
    {14, "Range",        1,   8, ParamDesc::Choice, kSequenceRanges},
};

SequencePanel::SequencePanel(EngineController& e, QWidget* p)
    : SliderPanel(e, 37, kSequenceParams, p) {}
