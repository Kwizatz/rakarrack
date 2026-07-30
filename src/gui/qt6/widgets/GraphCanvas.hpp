/*
  rakarrack - guitar multi-effects processor
  SPDX-License-Identifier: GPL-2.0-only

  Qt6 GUI - Node canvas for the effect graph.

  The order dialog can only describe what the old rack could hold: a list of
  slots, in order, one instance of each effect. This edits the graph itself,
  so a signal can split, branches can rejoin, and the same effect can appear
  more than once.

  Works on a GraphLayout rather than a live EffectGraph, so it needs no audio
  effects to display or edit a patch, and it asks layoutCanConnect() rather
  than reimplementing the rules the evaluator enforces.
*/

#pragma once

#include "EffectGraph.hpp"

#include <QGraphicsView>
#include <QString>

#include <functional>

class QGraphicsScene;
class GraphNodeItem;
class GraphEdgeItem;

class GraphCanvas : public QGraphicsView
{
    Q_OBJECT

public:
    explicit GraphCanvas(QWidget* parent = nullptr);
    ~GraphCanvas() override;

    /// How to label an effect type. Supplied by the caller so this widget does
    /// not depend on the engine; EngineController::getEffectTypeName() is the
    /// name everything else uses.
    void setNameLookup(std::function<QString(int type)> lookup);

    /// The types offered when adding a node, in the order shown.
    void setAvailableTypes(std::vector<int> types);

    void setLayout(const GraphLayout& layout);
    [[nodiscard]] const GraphLayout& layout() const { return m_layout; }

Q_SIGNALS:
    /// Emitted whenever the user changes the patch. Moving a node counts, so
    /// that canvas positions are saved with it.
    void layoutEdited();

    /// Emitted when the selected node changes, so a host can show its panel.
    /// -1 when nothing is selected.
    void nodeSelected(int nodeId);

protected:
    void contextMenuEvent(QContextMenuEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void drawBackground(QPainter* painter, const QRectF& rect) override;

private:
    friend class GraphNodeItem;

    /// Rebuild the scene from m_layout.
    void rebuildScene();

    /// Endpoint items are drawn as nodes but are not in m_layout.nodes.
    void addEndpointItems();

    void addNodeAt(int type, QPointF scenePos);
    void removeNode(int nodeId);
    void removeSelectedEdges();
    void toggleBypass(int nodeId);

    /// Called by a node item once the user finishes dragging it.
    void nodeMoved(int nodeId, QPointF pos);

    /// Called by a node item when a wire is dropped on a port.
    void requestConnection(int fromId, int toId);

    [[nodiscard]] GraphNodeItem* itemForNode(int nodeId) const;
    [[nodiscard]] QString nameForType(int type) const;

    /// Lay out any node that has no stored position, so a patch loaded from a
    /// file without positions is still readable.
    void placeUnpositionedNodes();

    QGraphicsScene* m_scene{nullptr};
    GraphLayout     m_layout;

    std::function<QString(int)> m_nameLookup;
    std::vector<int>            m_availableTypes;

    std::vector<GraphNodeItem*> m_nodeItems;
    std::vector<GraphEdgeItem*> m_edgeItems;
};
