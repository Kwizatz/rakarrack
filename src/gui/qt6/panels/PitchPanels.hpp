/*
  rakarrack - guitar multi-effects processor
  SPDX-License-Identifier: GPL-2.0-only

  Qt6 GUI — Pitch effect panels:
    HarmonizerPanel  (index 14)
    RingPanel        (index 21)
    ShifterPanel     (index 38)
    StereoHarmPanel  (index 42)
*/

#pragma once

#include "SliderPanel.hpp"

class HarmonizerPanel : public SliderPanel
{
public:
    explicit HarmonizerPanel(EngineController& e, QWidget* p = nullptr);
};

class RingPanel : public SliderPanel
{
public:
    explicit RingPanel(EngineController& e, QWidget* p = nullptr);
};

class ShifterPanel : public SliderPanel
{
public:
    explicit ShifterPanel(EngineController& e, QWidget* p = nullptr);
};

class StereoHarmPanel : public SliderPanel
{
public:
    explicit StereoHarmPanel(EngineController& e, QWidget* p = nullptr);
};
