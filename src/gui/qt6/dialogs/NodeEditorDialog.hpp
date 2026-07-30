/*
  rakarrack - guitar multi-effects processor
  SPDX-License-Identifier: GPL-2.0-only

  Qt6 GUI - Node editor dialog.

  Hosts the graph canvas. Editing here produces a GraphLayout, which is a
  superset of what the effect order can express: it can hold splits, merges,
  and more than one instance of an effect.

  While the engine still runs the sixteen-slot chain, this opens on that chain
  converted to a graph, and applying writes back whatever part of the result
  the chain can represent -- a plain series path. Anything else is kept in the
  layout but cannot be applied yet, and the dialog says so rather than
  silently dropping it.
*/

#pragma once

#include "EffectGraph.hpp"

#include <QDialog>

class EngineController;
class GraphCanvas;
class QLabel;

class NodeEditorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NodeEditorDialog(EngineController& engine, QWidget* parent = nullptr);
    ~NodeEditorDialog() override = default;

Q_SIGNALS:
    /// The patch was written to the engine. Not QDialog::accepted, because
    /// Apply deliberately leaves the dialog open.
    void applied();

private Q_SLOTS:
    void onLayoutEdited();
    void onApply();

private:
    void setupUi();

    /// Build a series-chain layout from the engine's current effect order.
    [[nodiscard]] GraphLayout layoutFromEngine() const;

    /// The series path from input to output, or empty if the layout branches.
    [[nodiscard]] std::vector<int> seriesChainTypes() const;

    void updateStatus();

    EngineController& m_engine;
    GraphCanvas*      m_canvas{nullptr};
    QLabel*           m_status{nullptr};
};
