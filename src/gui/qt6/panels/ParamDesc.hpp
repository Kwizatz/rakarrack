/*
  rakarrack - guitar multi-effects processor
  SPDX-License-Identifier: GPL-2.0-only

  Qt6 GUI — Parameter descriptor for auto-layout effect panels.

  Each effect defines a static array of ParamDesc entries describing its
  parameters.  SliderPanel reads these to build a grid of labelled controls.
*/

#pragma once

#include <cstdint>

/// Describes one effect parameter for auto-layout in SliderPanel.
struct ParamDesc
{
    int  id;                ///< Parameter index for changepar / getpar
    const char* label;      ///< Display name shown next to the control

    int  minVal = 0;        ///< Minimum integer value
    int  maxVal = 127;      ///< Maximum integer value

    /// Kind of widget to create.
    enum Control : uint8_t { Slider, Toggle, Choice };
    Control control = Slider;

    /// For Choice controls: pointer to an array of (maxVal-minVal+1) C-string
    /// labels.  If nullptr the combo box items are numbered minVal..maxVal.
    const char* const* choices = nullptr;
};

// ─── Shared choice-name arrays used by many effects ────────────────────────

/// LFO waveform types (12 entries, indices 0–11).
inline constexpr const char* kLfoTypeNames[] = {
    "Sine",       "Triangle",  "Ramp +",     "Ramp -",
    "ZigZag",     "Mod Sq",    "Mod Saw",    "Lorenz",
    "Lorenz XY",  "S && H",    "Tri-top",    "Tri-bottom",
};

/// EQ / filter band types (10 entries, indices 0–9).
inline constexpr const char* kEQFilterTypeNames[] = {
    "Off", "LP1", "HP1", "LP2", "HP2",
    "BP",  "Notch", "Peak", "LowShelf", "HighShelf",
};
