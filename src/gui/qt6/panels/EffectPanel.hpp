/*
  rakarrack - guitar multi-effects processor
  SPDX-License-Identifier: GPL-2.0-only

  Qt6 GUI — EffectPanel base class

  Every effect panel (47 total) inherits from this.  Provides a standard
  layout: header row (On toggle + Preset combo) and a body area filled by
  subclasses with MidiSliders and other controls.

  The factory method create() returns the appropriate subclass for a given
  effect type ID (0-46).
*/

#pragma once

#include <QWidget>

#include <memory>

class EngineController;
class QComboBox;
class QPushButton;
class QVBoxLayout;
class QHBoxLayout;

class EffectPanel : public QWidget
{
    Q_OBJECT

public:
    /// @param engine     Thread-safe engine interface
    /// @param effectIndex  Effect type ID (0-46)
    /// @param parent
    explicit EffectPanel(EngineController& engine, int effectIndex,
                         QWidget* parent = nullptr);
    ~EffectPanel() override = default;

    [[nodiscard]] int effectIndex() const { return m_effectIndex; }

    /// Pull all parameter values from the engine and update controls.
    virtual void syncFromEngine();
    /// Push all control values to the engine.
    virtual void syncToEngine();

    /// Factory:  Returns a panel for the given effect type.
    /// Falls back to a generic placeholder for unimplemented effects.
    static std::unique_ptr<EffectPanel> create(int effectType,
                                                EngineController& engine,
                                                QWidget* parent = nullptr);

Q_SIGNALS:
    /// Emitted when the user toggles the effect on/off.
    void bypassChanged(int effectIndex, bool active);

protected:
    /// Called by subclasses to populate the body area.
    /// Returns the QVBoxLayout below the header for subclasses to fill.
    QVBoxLayout* bodyLayout();

    EngineController& m_engine;
    int m_effectIndex;

private:
    void setupHeader();

    QVBoxLayout* m_mainLayout  = nullptr;
    QVBoxLayout* m_bodyLayout  = nullptr;
    QPushButton* m_onButton    = nullptr;
    QComboBox*   m_presetCombo = nullptr;
};
