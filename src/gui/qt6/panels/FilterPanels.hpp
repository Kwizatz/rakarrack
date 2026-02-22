/*
  rakarrack - guitar multi-effects processor
  SPDX-License-Identifier: GPL-2.0-only

  Qt6 GUI — Filter effect panels:
    WahWahPanel      (index 10, DynamicFilter)
    RyanWahPanel     (index 31)
    CoilCrafterPanel (index 33)
    ShelfBoostPanel  (index 34)
*/

#pragma once

#include "SliderPanel.hpp"

class WahWahPanel : public SliderPanel
{
public:
    explicit WahWahPanel(EngineController& e, QWidget* p = nullptr);
};

class RyanWahPanel : public SliderPanel
{
public:
    explicit RyanWahPanel(EngineController& e, QWidget* p = nullptr);
};

class CoilCrafterPanel : public SliderPanel
{
public:
    explicit CoilCrafterPanel(EngineController& e, QWidget* p = nullptr);
};

class ShelfBoostPanel : public SliderPanel
{
public:
    explicit ShelfBoostPanel(EngineController& e, QWidget* p = nullptr);
};
