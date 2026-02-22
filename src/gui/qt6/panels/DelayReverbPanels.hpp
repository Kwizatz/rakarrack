/*
  rakarrack - guitar multi-effects processor
  SPDX-License-Identifier: GPL-2.0-only

  Qt6 GUI — Delay & Reverb effect panels:
    EchoPanel        (index 4)
    ReverbPanel      (index 8)
    MusicDelayPanel  (index 15)
    ArpiePanel       (index 24)
    ConvolotronPanel (index 29)
    RBEchoPanel      (index 32)
    ReverbtronPanel  (index 40)
    EchotronPanel    (index 41)
*/

#pragma once

#include "SliderPanel.hpp"

class EchoPanel : public SliderPanel
{
public:
    explicit EchoPanel(EngineController& e, QWidget* p = nullptr);
};

class ReverbPanel : public SliderPanel
{
public:
    explicit ReverbPanel(EngineController& e, QWidget* p = nullptr);
};

class MusicDelayPanel : public SliderPanel
{
public:
    explicit MusicDelayPanel(EngineController& e, QWidget* p = nullptr);
};

class ArpiePanel : public SliderPanel
{
public:
    explicit ArpiePanel(EngineController& e, QWidget* p = nullptr);
};

class ConvolotronPanel : public SliderPanel
{
public:
    explicit ConvolotronPanel(EngineController& e, QWidget* p = nullptr);
};

class RBEchoPanel : public SliderPanel
{
public:
    explicit RBEchoPanel(EngineController& e, QWidget* p = nullptr);
};

class ReverbtronPanel : public SliderPanel
{
public:
    explicit ReverbtronPanel(EngineController& e, QWidget* p = nullptr);
};

class EchotronPanel : public SliderPanel
{
public:
    explicit EchotronPanel(EngineController& e, QWidget* p = nullptr);
};
