/*
  rakarrack - guitar multi-effects processor
  SPDX-License-Identifier: GPL-2.0-only

  Qt6 GUI — Miscellaneous effect panels:
    ExciterPanel   (index 22)
    ShufflePanel   (index 26)
    MBVvolPanel    (index 28)
    LooperPanel    (index 30)
    VocoderPanel   (index 35)
    SustainerPanel (index 36)
    SequencePanel  (index 37)
*/

#pragma once

#include "SliderPanel.hpp"

class ExciterPanel : public SliderPanel
{
public:
    explicit ExciterPanel(EngineController& e, QWidget* p = nullptr);
};

class ShufflePanel : public SliderPanel
{
public:
    explicit ShufflePanel(EngineController& e, QWidget* p = nullptr);
};

class MBVvolPanel : public SliderPanel
{
public:
    explicit MBVvolPanel(EngineController& e, QWidget* p = nullptr);
};

class LooperPanel : public SliderPanel
{
public:
    explicit LooperPanel(EngineController& e, QWidget* p = nullptr);
};

class VocoderPanel : public SliderPanel
{
public:
    explicit VocoderPanel(EngineController& e, QWidget* p = nullptr);
};

class SustainerPanel : public SliderPanel
{
public:
    explicit SustainerPanel(EngineController& e, QWidget* p = nullptr);
};

class SequencePanel : public SliderPanel
{
public:
    explicit SequencePanel(EngineController& e, QWidget* p = nullptr);
};
