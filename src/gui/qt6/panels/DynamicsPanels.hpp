/*
  rakarrack - guitar multi-effects processor
  SPDX-License-Identifier: GPL-2.0-only

  Qt6 GUI — Dynamics effect panels:
    CompressorPanel  (index 1)
    GatePanel        (index 16)
    ExpanderPanel    (index 25)
    CompBandPanel    (index 43)
*/

#pragma once

#include "SliderPanel.hpp"

class CompressorPanel : public SliderPanel
{
public:
    explicit CompressorPanel(EngineController& e, QWidget* p = nullptr);
};

class GatePanel : public SliderPanel
{
public:
    explicit GatePanel(EngineController& e, QWidget* p = nullptr);
};

class ExpanderPanel : public SliderPanel
{
public:
    explicit ExpanderPanel(EngineController& e, QWidget* p = nullptr);
};

class CompBandPanel : public SliderPanel
{
public:
    explicit CompBandPanel(EngineController& e, QWidget* p = nullptr);
};
