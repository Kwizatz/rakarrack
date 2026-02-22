/*
  rakarrack - guitar multi-effects processor
  SPDX-License-Identifier: GPL-2.0-only

  Qt6 GUI — SliderPanel: auto-layout effect panel from ParamDesc array.

  Most of the 47 effect panels are thin subclasses that only provide a
  static ParamDesc table.  SliderPanel reads the table at construction
  time and builds a 2-column grid of labelled MidiSliders, QCheckBoxes,
  and QComboBoxes.  syncFromEngine / syncToEngine are wired automatically.
*/

#pragma once

#include "EffectPanel.hpp"
#include "ParamDesc.hpp"

#include <span>
#include <vector>

class MidiSlider;
class QCheckBox;
class QComboBox;

/// Effect panel that auto-generates its body from a ParamDesc table.
class SliderPanel : public EffectPanel
{
public:
    /// @param engine       Thread-safe engine interface.
    /// @param effectIndex  Effect type ID (0-46).
    /// @param params       Static array of parameter descriptors.
    /// @param parent       Parent widget.
    SliderPanel(EngineController& engine, int effectIndex,
                std::span<const ParamDesc> params,
                QWidget* parent = nullptr);

    void syncFromEngine() override;
    void syncToEngine() override;

protected:
    /// One entry per control created from the ParamDesc table.
    struct Binding
    {
        int                paramId;
        int                minVal;   ///< For Choice offset
        ParamDesc::Control type;
        QWidget*           widget;
    };

    std::vector<Binding> m_bindings;
};
