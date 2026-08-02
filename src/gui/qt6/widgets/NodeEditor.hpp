/*
  rakarrack - guitar multi-effects processor
  SPDX-License-Identifier: GPL-2.0-only

  Qt6 GUI - Main node editor.
*/

#pragma once

#include "EffectGraph.hpp"

#include <QWidget>

class EffectPanel;
class EngineController;
class GraphCanvas;
class QLabel;
class QLineEdit;
class QListWidget;
class QVBoxLayout;

class NodeEditor final : public QWidget
{
    Q_OBJECT

public:
    explicit NodeEditor(EngineController& engine, QWidget* parent = nullptr);
    ~NodeEditor() override = default;

    /// Reload the canvas after a preset or bank changes the engine routing.
    void syncFromEngine();

private Q_SLOTS:
    void onLayoutEdited();
    void onNodeSelected(int nodeId);

private:
    void setupUi();
    void populateEffectList();
    void applyLayout();
    void updateStatus();

    [[nodiscard]] GraphLayout layoutFromEngine() const;

    void showNodeEditor(int nodeId);
    void clearNodeEditor(const QString& reason);

    EngineController& m_engine;
    GraphCanvas*      m_canvas{nullptr};
    QLineEdit*        m_filter{nullptr};
    QListWidget*      m_effectList{nullptr};
    QLabel*           m_status{nullptr};

    QVBoxLayout* m_paramLayout{nullptr};
    QLabel*      m_paramNote{nullptr};
    EffectPanel* m_paramPanel{nullptr};
    int          m_paramNodeId{-1};
};