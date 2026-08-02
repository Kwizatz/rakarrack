/*
  rakarrack - guitar multi-effects processor
  SPDX-License-Identifier: GPL-2.0-only

  GraphCanvasTest.cpp - Node movement and live edge tracking.
*/

#include "gui/qt6/widgets/GraphCanvas.hpp"

#include <QApplication>
#include <QGraphicsItem>
#include <QGraphicsPathItem>
#include <QGraphicsScene>
#include <QLineF>

#include <algorithm>
#include <cstdio>
#include <vector>

namespace {

int g_checks = 0;
int g_failures = 0;

void check(bool condition, const char* description)
{
    ++g_checks;
    std::printf("%-58s %s\n", description, condition ? "ok" : "FAILED");
    if (!condition)
        ++g_failures;
}

std::vector<QGraphicsPathItem*> edgeItems(QGraphicsScene& scene)
{
    std::vector<QGraphicsPathItem*> edges;
    for (QGraphicsItem* item : scene.items())
        if (auto* edge = dynamic_cast<QGraphicsPathItem*>(item))
            edges.push_back(edge);
    return edges;
}

std::vector<QGraphicsItem*> endpointItems(QGraphicsScene& scene)
{
    std::vector<QGraphicsItem*> endpoints;
    for (QGraphicsItem* item : scene.items())
    {
        if (dynamic_cast<QGraphicsPathItem*>(item) == nullptr
            && !item->flags().testFlag(QGraphicsItem::ItemIsSelectable))
            endpoints.push_back(item);
    }
    return endpoints;
}

} // namespace

int main(int argc, char* argv[])
{
    qputenv("QT_QPA_PLATFORM", "offscreen");
    QApplication app(argc, argv);

    GraphLayout layout;
    GraphNodeLayout node;
    node.id = 1;
    node.type = 0;
    node.x = 100.0f;
    node.y = 100.0f;
    layout.nodes.push_back(node);
    layout.connections = {
        {kInputNodeId, node.id},
        {node.id, kOutputNodeId},
    };

    GraphCanvas canvas;
    canvas.setLayout(layout);
    QGraphicsScene* scene = canvas.scene();

    QGraphicsItem* effect = nullptr;
    for (QGraphicsItem* item : scene->items())
    {
        if (item->flags().testFlag(QGraphicsItem::ItemIsSelectable))
        {
            effect = item;
            break;
        }
    }

    std::vector<QGraphicsItem*> endpoints = endpointItems(*scene);
    std::vector<QGraphicsPathItem*> edges = edgeItems(*scene);
    check(effect != nullptr, "canvas contains the effect node");
    check(endpoints.size() == 2, "canvas contains input and output endpoints");
    check(edges.size() == 2, "canvas contains both connected wires");
    check(std::all_of(endpoints.begin(), endpoints.end(), [](QGraphicsItem* endpoint) {
              return endpoint->flags().testFlag(QGraphicsItem::ItemIsMovable);
          }),
          "input and output endpoints are movable");

    std::vector<QPainterPath> beforeEffectMove;
    for (QGraphicsPathItem* edge : edges)
        beforeEffectMove.push_back(edge->path());

    effect->setPos(effect->pos() + QPointF(80.0, 50.0));
    bool allEffectEdgesMoved = true;
    for (std::size_t i = 0; i < edges.size(); ++i)
        allEffectEdgesMoved &= (edges[i]->path() != beforeEffectMove[i]);
    check(allEffectEdgesMoved,
          "both connected wires update during an effect move");

    QGraphicsItem* input = *std::min_element(
        endpoints.begin(), endpoints.end(), [](QGraphicsItem* left, QGraphicsItem* right) {
            return left->pos().x() < right->pos().x();
        });
    const QPointF movedInputPosition = input->pos() + QPointF(-40.0, -60.0);

    std::vector<QPainterPath> beforeInputMove;
    for (QGraphicsPathItem* edge : edges)
        beforeInputMove.push_back(edge->path());
    input->setPos(movedInputPosition);

    const bool anInputEdgeMoved = std::any_of(
        edges.begin(), edges.end(), [&, index = std::size_t{0}](QGraphicsPathItem* edge) mutable {
            const bool changed = edge->path() != beforeInputMove[index];
            ++index;
            return changed;
        });
    check(anInputEdgeMoved, "the input wire updates during an endpoint move");

    // Rebuilding for an unrelated visual change must not snap a moved endpoint
    // back to its automatic position.
    canvas.setNodeBypassed(node.id, true);
    endpoints = endpointItems(*scene);
    const bool inputPositionPreserved = std::any_of(
        endpoints.begin(), endpoints.end(), [&](QGraphicsItem* endpoint) {
            return QLineF(endpoint->pos(), movedInputPosition).length() < 0.01;
        });
    check(inputPositionPreserved,
          "a moved endpoint keeps its position across a scene rebuild");

    std::printf("\n%d checks, %d failed\n", g_checks, g_failures);
    return g_failures == 0 ? 0 : 1;
}