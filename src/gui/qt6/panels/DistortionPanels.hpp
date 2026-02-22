/*
  rakarrack - guitar multi-effects processor
  SPDX-License-Identifier: GPL-2.0-only

  Qt6 GUI — Distortion family panels:
    DistorsionPanel  (index 2, also used for Overdrive index 3)
    NewDistPanel     (index 17)
    ValvePanel       (index 19)
    MBDistPanel      (index 23)
    StompBoxPanel    (index 39)
*/

#pragma once

#include "SliderPanel.hpp"

class DistorsionPanel : public SliderPanel
{
public:
    DistorsionPanel(EngineController& e, int effectIndex, QWidget* p = nullptr);
};

class NewDistPanel : public SliderPanel
{
public:
    explicit NewDistPanel(EngineController& e, QWidget* p = nullptr);
};

class ValvePanel : public SliderPanel
{
public:
    explicit ValvePanel(EngineController& e, QWidget* p = nullptr);
};

class MBDistPanel : public SliderPanel
{
public:
    explicit MBDistPanel(EngineController& e, QWidget* p = nullptr);
};

class StompBoxPanel : public SliderPanel
{
public:
    explicit StompBoxPanel(EngineController& e, QWidget* p = nullptr);
};
