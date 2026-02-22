/*
  rakarrack - guitar multi-effects processor
  SPDX-License-Identifier: GPL-2.0-only

  Qt6 GUI — Modulation effect panels:
    ChorusPanel      (index 5, also used for Flanger index 7)
    PhaserPanel      (index 6)
    AlienwahPanel    (index 11)
    PanPanel         (index 13)
    APhaserPanel     (index 18)
    DualFlangePanel  (index 20)
    SynthfilterPanel (index 27)
    OpticaltremPanel (index 44)
    VibePanel        (index 45)
    InfinityPanel    (index 46)
*/

#pragma once

#include "SliderPanel.hpp"

class ChorusPanel : public SliderPanel
{
public:
    ChorusPanel(EngineController& e, int effectIndex, QWidget* p = nullptr);
};

class PhaserPanel : public SliderPanel
{
public:
    explicit PhaserPanel(EngineController& e, QWidget* p = nullptr);
};

class AlienwahPanel : public SliderPanel
{
public:
    explicit AlienwahPanel(EngineController& e, QWidget* p = nullptr);
};

class PanPanel : public SliderPanel
{
public:
    explicit PanPanel(EngineController& e, QWidget* p = nullptr);
};

class APhaserPanel : public SliderPanel
{
public:
    explicit APhaserPanel(EngineController& e, QWidget* p = nullptr);
};

class DualFlangePanel : public SliderPanel
{
public:
    explicit DualFlangePanel(EngineController& e, QWidget* p = nullptr);
};

class SynthfilterPanel : public SliderPanel
{
public:
    explicit SynthfilterPanel(EngineController& e, QWidget* p = nullptr);
};

class OpticaltremPanel : public SliderPanel
{
public:
    explicit OpticaltremPanel(EngineController& e, QWidget* p = nullptr);
};

class VibePanel : public SliderPanel
{
public:
    explicit VibePanel(EngineController& e, QWidget* p = nullptr);
};

class InfinityPanel : public SliderPanel
{
public:
    explicit InfinityPanel(EngineController& e, QWidget* p = nullptr);
};
