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

    /// Edit one graph node instead of the engine's per-type instance.
    ///
    /// A patch may hold several nodes of the same type, each with its own
    /// settings, so a panel showing "Chorus" is ambiguous until it is told
    /// which one it means. Pass kNoNode to go back to addressing the type,
    /// which is what the main window's panels do.
    static constexpr int kNoNode = -1;
    void setTargetNode(int nodeId);
    [[nodiscard]] int targetNode() const { return m_nodeId; }

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

    // Parameter access that follows the panel's target. Subclasses use these
    // rather than talking to the engine directly, so retargeting one panel to
    // a node does not need every control rewired.
    void setParam(int paramId, int value);
    [[nodiscard]] int getParam(int paramId) const;
    void setPreset(int preset);

    // Named apart from QWidget::setEnabled/isEnabled deliberately: these are
    // the effect's on/off state, and shadowing the widget's would be a trap.
    void setEffectActive(bool active);
    [[nodiscard]] bool isEffectActive() const;

    EngineController& m_engine;
    int m_effectIndex;
    int m_nodeId{kNoNode};

private:
    void setupHeader();
    void updateOnButtonAppearance(bool active);

    QVBoxLayout* m_mainLayout  = nullptr;
    QVBoxLayout* m_bodyLayout  = nullptr;
    QPushButton* m_onButton    = nullptr;
    QComboBox*   m_presetCombo = nullptr;
};
