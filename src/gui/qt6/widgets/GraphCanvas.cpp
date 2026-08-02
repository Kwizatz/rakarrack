/*
  rakarrack - guitar multi-effects processor
  SPDX-License-Identifier: GPL-2.0-only

  Qt6 GUI - Node canvas for the effect graph.
*/

#include "GraphCanvas.hpp"

#include <QContextMenuEvent>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QGraphicsPathItem>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <QMenu>
#include <QMimeData>
#include <QPainter>
#include <QPainterPath>
#include <QSignalBlocker>
#include <QtMath>

#include <algorithm>

namespace {

// Node geometry. Ports sit on the vertical centre of each side.
constexpr qreal kNodeWidth  = 140.0;
constexpr qreal kNodeHeight = 46.0;
constexpr qreal kPortRadius = 6.0;
constexpr qreal kGridStep   = 24.0;

QPointF outputPortPos(const QPointF& topLeft)
{
    return topLeft + QPointF(kNodeWidth, kNodeHeight * 0.5);
}

QPointF inputPortPos(const QPointF& topLeft)
{
    return topLeft + QPointF(0.0, kNodeHeight * 0.5);
}

/// A wire. Drawn as a horizontal-tangent curve so parallel branches stay
/// readable where several meet at the same port.
QPainterPath wirePath(const QPointF& from, const QPointF& to)
{
    const qreal dx = std::max(40.0, std::abs(to.x() - from.x()) * 0.5);
    QPainterPath path(from);
    path.cubicTo(from + QPointF(dx, 0.0), to - QPointF(dx, 0.0), to);
    return path;
}

} // namespace

// ─── Edge item ─────────────────────────────────────────────────────

class GraphEdgeItem : public QGraphicsPathItem
{
public:
    GraphEdgeItem(int from, int to) : m_from(from), m_to(to)
    {
        setFlag(QGraphicsItem::ItemIsSelectable, true);
        setZValue(-1.0);
    }

    [[nodiscard]] int fromId() const { return m_from; }
    [[nodiscard]] int toId() const { return m_to; }

    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override
    {
        QColor colour = isSelected() ? QColor(120, 190, 255) : QColor(150, 150, 155);
        painter->setPen(QPen(colour, isSelected() ? 2.5 : 1.8));
        painter->setBrush(Qt::NoBrush);
        painter->drawPath(path());
    }

    /// Widen the clickable area, since a curve is hard to hit exactly.
    [[nodiscard]] QPainterPath shape() const override
    {
        QPainterPathStroker stroker;
        stroker.setWidth(10.0);
        return stroker.createStroke(path());
    }

private:
    int m_from;
    int m_to;
};

// ─── Node item ─────────────────────────────────────────────────────

class GraphNodeItem : public QGraphicsItem
{
public:
    GraphNodeItem(GraphCanvas& canvas, int id, QString title, bool endpoint)
        : m_canvas(canvas), m_id(id), m_title(std::move(title)), m_endpoint(endpoint)
    {
        setFlag(QGraphicsItem::ItemIsMovable, true);
        setFlag(QGraphicsItem::ItemIsSelectable, !endpoint);
        setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    }

    [[nodiscard]] int nodeId() const { return m_id; }
    void setBypassed(bool bypassed) { m_bypassed = bypassed; update(); }

    [[nodiscard]] QPointF outputPortScenePos() const { return pos() + outputPortPos({0, 0}); }
    [[nodiscard]] QPointF inputPortScenePos() const { return pos() + inputPortPos({0, 0}); }

    [[nodiscard]] QRectF boundingRect() const override
    {
        // Room for the ports, which sit on the edges.
        return QRectF(-kPortRadius, 0.0, kNodeWidth + kPortRadius * 2, kNodeHeight);
    }

    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override
    {
        const QRectF body(0.0, 0.0, kNodeWidth, kNodeHeight);

        QColor fill = m_endpoint ? QColor(58, 66, 78) : QColor(48, 50, 56);
        if (m_bypassed)
            fill = QColor(40, 40, 44);

        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->setBrush(fill);
        painter->setPen(QPen(isSelected() ? QColor(120, 190, 255) : QColor(90, 92, 100),
                             isSelected() ? 2.0 : 1.2));
        painter->drawRoundedRect(body, 6.0, 6.0);

        painter->setPen(m_bypassed ? QColor(130, 130, 135) : QColor(226, 228, 232));
        QFont f = painter->font();
        f.setPointSizeF(f.pointSizeF() * 0.95);
        painter->setFont(f);
        painter->drawText(body.adjusted(10, 0, -10, 0),
                          Qt::AlignVCenter | Qt::AlignLeft,
                          m_bypassed ? m_title + QStringLiteral("  (off)") : m_title);

        // Ports. The graph input has only an output, and vice versa.
        painter->setPen(Qt::NoPen);
        painter->setBrush(QColor(150, 200, 140));
        if (m_id != kInputNodeId)
            painter->drawEllipse(inputPortPos({0, 0}), kPortRadius, kPortRadius);
        if (m_id != kOutputNodeId)
            painter->drawEllipse(outputPortPos({0, 0}), kPortRadius, kPortRadius);
    }

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override
    {
        const QVariant result = QGraphicsItem::itemChange(change, value);
        if (change == QGraphicsItem::ItemPositionHasChanged)
            m_canvas.nodePositionChanged(m_id, pos());
        return result;
    }

    void mousePressEvent(QGraphicsSceneMouseEvent* event) override
    {
        // Pressing the output port starts a wire rather than a move.
        if (event->button() == Qt::LeftButton && m_id != kOutputNodeId
            && QLineF(event->pos(), outputPortPos({0, 0})).length() <= kPortRadius * 1.8)
        {
            m_wiring = true;
            m_wireEnd = event->scenePos();
            event->accept();
            return;
        }
        QGraphicsItem::mousePressEvent(event);
    }

    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override
    {
        if (m_wiring)
        {
            m_wireEnd = event->scenePos();
            if (scene())
                scene()->update();
            event->accept();
            return;
        }
        QGraphicsItem::mouseMoveEvent(event);
    }

    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override
    {
        if (m_wiring)
        {
            m_wiring = false;
            if (scene())
            {
                // Find whichever node's input port the wire was dropped on.
                for (QGraphicsItem* item : scene()->items(event->scenePos()))
                {
                    auto* target = dynamic_cast<GraphNodeItem*>(item);
                    if (target == nullptr || target == this)
                        continue;
                    if (QLineF(event->scenePos(), target->inputPortScenePos()).length()
                        > kPortRadius * 1.8)
                        continue;
                    m_canvas.requestConnection(m_id, target->nodeId());
                    break;
                }
                scene()->update();
            }
            event->accept();
            return;
        }

        QGraphicsItem::mouseReleaseEvent(event);
        m_canvas.nodeMoved(m_id, pos());
    }

public:
    /// The wire being dragged, if any. Drawn by the canvas foreground so it
    /// sits above everything.
    [[nodiscard]] bool wiring() const { return m_wiring; }
    [[nodiscard]] QPointF wireEnd() const { return m_wireEnd; }

private:
    GraphCanvas& m_canvas;
    int      m_id;
    QString  m_title;
    bool     m_endpoint;
    bool     m_bypassed{false};
    bool     m_wiring{false};
    QPointF  m_wireEnd;
};

// ─── Canvas ────────────────────────────────────────────────────────

GraphCanvas::GraphCanvas(QWidget* parent)
    : QGraphicsView(parent)
{
    m_scene = new QGraphicsScene(this);
    setScene(m_scene);
    setRenderHint(QPainter::Antialiasing, true);
    setDragMode(QGraphicsView::RubberBandDrag);
    setAcceptDrops(true);
    viewport()->setAcceptDrops(true);

    connect(m_scene, &QGraphicsScene::selectionChanged, this, [this]
    {
        int selectedId = -1;
        for (QGraphicsItem* selected : m_scene->selectedItems())
        {
            if (auto* node = dynamic_cast<GraphNodeItem*>(selected))
            {
                selectedId = node->nodeId();
                break;
            }
        }

        if (selectedId == m_selectedNodeId)
            return;
        m_selectedNodeId = selectedId;
        Q_EMIT nodeSelected(selectedId);
    });
}

GraphCanvas::~GraphCanvas() = default;

void GraphCanvas::setNameLookup(std::function<QString(int)> lookup)
{
    m_nameLookup = std::move(lookup);
    rebuildScene();
}

void GraphCanvas::setAvailableTypes(std::vector<int> types)
{
    m_availableTypes = std::move(types);
}

QString GraphCanvas::nameForType(int type) const
{
    if (m_nameLookup)
        return m_nameLookup(type);
    return QStringLiteral("Effect %1").arg(type);
}

void GraphCanvas::setLayout(const GraphLayout& layout)
{
    m_layout = layout;
    m_endpointPositionsInitialized = false;
    if (m_selectedNodeId >= 0
        && std::none_of(m_layout.nodes.begin(), m_layout.nodes.end(),
                        [this](const GraphNodeLayout& node) {
                            return node.id == m_selectedNodeId;
                        }))
    {
        m_selectedNodeId = -1;
        Q_EMIT nodeSelected(-1);
    }
    placeUnpositionedNodes();
    rebuildScene();
}

void GraphCanvas::setNodeBypassed(int nodeId, bool bypassed)
{
    for (GraphNodeLayout& node : m_layout.nodes)
    {
        if (node.id != nodeId || node.bypassed == bypassed)
            continue;
        node.bypassed = bypassed;
        rebuildScene();
        Q_EMIT layoutEdited();
        return;
    }
}

void GraphCanvas::placeUnpositionedNodes()
{
    // A graph converted from the old sixteen-slot chain has no positions, so
    // spread anything still at the origin left to right in declaration order.
    qreal x = 60.0;
    for (GraphNodeLayout& n : m_layout.nodes)
    {
        if (n.x == 0.0f && n.y == 0.0f)
        {
            n.x = static_cast<float>(x);
            n.y = 220.0f;
        }
        x = std::max(x, static_cast<qreal>(n.x)) + kNodeWidth + 60.0;
    }
}

void GraphCanvas::addEndpointItems()
{
    if (!m_endpointPositionsInitialized)
    {
        qreal minX = 1e9;
        qreal maxX = -1e9;
        for (const GraphNodeLayout& n : m_layout.nodes)
        {
            minX = std::min(minX, static_cast<qreal>(n.x));
            maxX = std::max(maxX, static_cast<qreal>(n.x));
        }
        if (m_layout.nodes.empty())
        {
            minX = 200.0;
            maxX = 200.0;
        }

        m_inputNodePosition = {minX - kNodeWidth - 80.0, 220.0};
        m_outputNodePosition = {maxX + kNodeWidth + 80.0, 220.0};
        m_endpointPositionsInitialized = true;
    }

    auto* in = new GraphNodeItem(*this, kInputNodeId, tr("Input"), true);
    in->setPos(m_inputNodePosition);
    m_scene->addItem(in);
    m_nodeItems.push_back(in);

    auto* out = new GraphNodeItem(*this, kOutputNodeId, tr("Output"), true);
    out->setPos(m_outputNodePosition);
    m_scene->addItem(out);
    m_nodeItems.push_back(out);
}

void GraphCanvas::rebuildScene()
{
    const QSignalBlocker blockSelectionSignals(m_scene);
    m_scene->clear();
    m_nodeItems.clear();
    m_edgeItems.clear();

    for (const GraphNodeLayout& n : m_layout.nodes)
    {
        auto* item = new GraphNodeItem(*this, n.id, nameForType(n.type), false);
        item->setPos(n.x, n.y);
        item->setBypassed(n.bypassed);
        m_scene->addItem(item);
        item->setSelected(n.id == m_selectedNodeId);
        m_nodeItems.push_back(item);
    }

    addEndpointItems();

    for (const Connection& c : m_layout.connections)
    {
        GraphNodeItem* from = itemForNode(c.from);
        GraphNodeItem* to   = itemForNode(c.to);
        if (from == nullptr || to == nullptr)
            continue;

        auto* edge = new GraphEdgeItem(c.from, c.to);
        edge->setPath(wirePath(from->outputPortScenePos(), to->inputPortScenePos()));
        m_scene->addItem(edge);
        m_edgeItems.push_back(edge);
    }

    // Bound the scene to what is actually in it, with room to drag outwards.
    // A fixed rect leaves the patch somewhere off in the corner.
    updateSceneRect();
}

GraphNodeItem* GraphCanvas::itemForNode(int nodeId) const
{
    for (GraphNodeItem* item : m_nodeItems)
        if (item->nodeId() == nodeId)
            return item;
    return nullptr;
}

void GraphCanvas::nodePositionChanged(int nodeId, QPointF pos)
{
    if (nodeId == kInputNodeId)
        m_inputNodePosition = pos;
    else if (nodeId == kOutputNodeId)
        m_outputNodePosition = pos;

    updateEdgesForNode(nodeId);
}

void GraphCanvas::updateEdgesForNode(int nodeId)
{
    for (GraphEdgeItem* edge : m_edgeItems)
    {
        if (edge->fromId() != nodeId && edge->toId() != nodeId)
            continue;

        GraphNodeItem* from = itemForNode(edge->fromId());
        GraphNodeItem* to = itemForNode(edge->toId());
        if (from != nullptr && to != nullptr)
            edge->setPath(wirePath(from->outputPortScenePos(),
                                   to->inputPortScenePos()));
    }
}

void GraphCanvas::updateSceneRect()
{
    m_scene->setSceneRect(m_scene->itemsBoundingRect().adjusted(
        -160, -160, 160, 160));
}

void GraphCanvas::nodeMoved(int nodeId, QPointF pos)
{
    if (nodeId == kInputNodeId || nodeId == kOutputNodeId)
    {
        updateSceneRect();
        return;
    }

    for (GraphNodeLayout& n : m_layout.nodes)
    {
        if (n.id != nodeId)
            continue;
        if (n.x == static_cast<float>(pos.x()) && n.y == static_cast<float>(pos.y()))
            return;
        n.x = static_cast<float>(pos.x());
        n.y = static_cast<float>(pos.y());
        updateSceneRect();
        Q_EMIT layoutEdited();
        return;
    }
}

void GraphCanvas::requestConnection(int fromId, int toId)
{
    // The evaluator's rules, asked rather than reimplemented.
    if (!layoutCanConnect(m_layout, fromId, toId))
        return;

    m_layout.connections.push_back({fromId, toId});
    rebuildScene();
    Q_EMIT layoutEdited();
}

void GraphCanvas::addNodeAt(int type, QPointF scenePos)
{
    int nextId = 1;
    for (const GraphNodeLayout& n : m_layout.nodes)
        nextId = std::max(nextId, n.id + 1);

    GraphNodeLayout n;
    n.id   = nextId;
    n.type = type;
    n.mix  = defaultMixModeForType(type);
    n.x    = static_cast<float>(scenePos.x());
    n.y    = static_cast<float>(scenePos.y());
    m_layout.nodes.push_back(n);

    m_selectedNodeId = nextId;
    rebuildScene();
    Q_EMIT layoutEdited();
    Q_EMIT nodeSelected(nextId);
}

void GraphCanvas::removeNode(int nodeId)
{
    const bool removedSelection = (m_selectedNodeId == nodeId);
    std::erase_if(m_layout.nodes,
                  [nodeId](const GraphNodeLayout& n) { return n.id == nodeId; });
    // A node's wires go with it, or the graph would refer to something absent.
    std::erase_if(m_layout.connections, [nodeId](const Connection& c) {
        return c.from == nodeId || c.to == nodeId;
    });

    if (removedSelection)
        m_selectedNodeId = -1;
    rebuildScene();
    Q_EMIT layoutEdited();
    if (removedSelection)
        Q_EMIT nodeSelected(-1);
}

void GraphCanvas::removeSelectedEdges()
{
    bool changed = false;
    for (GraphEdgeItem* edge : m_edgeItems)
    {
        if (!edge->isSelected())
            continue;
        const int from = edge->fromId();
        const int to   = edge->toId();
        std::erase_if(m_layout.connections, [from, to](const Connection& c) {
            return c.from == from && c.to == to;
        });
        changed = true;
    }

    for (GraphNodeItem* item : m_nodeItems)
    {
        if (item->isSelected())
        {
            removeNode(item->nodeId());
            return;
        }
    }

    if (changed)
    {
        rebuildScene();
        Q_EMIT layoutEdited();
    }
}

void GraphCanvas::toggleBypass(int nodeId)
{
    for (GraphNodeLayout& n : m_layout.nodes)
    {
        if (n.id != nodeId)
            continue;
        setNodeBypassed(nodeId, !n.bypassed);
        return;
    }
}

void GraphCanvas::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace)
    {
        removeSelectedEdges();
        event->accept();
        return;
    }
    QGraphicsView::keyPressEvent(event);
}

void GraphCanvas::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasFormat(kEffectTypeMimeType))
        event->acceptProposedAction();
    else
        QGraphicsView::dragEnterEvent(event);
}

void GraphCanvas::dragMoveEvent(QDragMoveEvent* event)
{
    if (event->mimeData()->hasFormat(kEffectTypeMimeType))
        event->acceptProposedAction();
    else
        QGraphicsView::dragMoveEvent(event);
}

void GraphCanvas::dropEvent(QDropEvent* event)
{
    if (!event->mimeData()->hasFormat(kEffectTypeMimeType))
    {
        QGraphicsView::dropEvent(event);
        return;
    }

    bool valid = false;
    const int type = QString::fromLatin1(
        event->mimeData()->data(kEffectTypeMimeType)).toInt(&valid);
    if (!valid
        || std::find(m_availableTypes.begin(), m_availableTypes.end(), type)
               == m_availableTypes.end())
    {
        event->ignore();
        return;
    }

    addNodeAt(type, mapToScene(event->position().toPoint()));
    event->setDropAction(Qt::CopyAction);
    event->accept();
}

void GraphCanvas::contextMenuEvent(QContextMenuEvent* event)
{
    const QPointF scenePos = mapToScene(event->pos());

    QMenu menu(this);

    GraphNodeItem* under = nullptr;
    for (QGraphicsItem* item : m_scene->items(scenePos))
    {
        under = dynamic_cast<GraphNodeItem*>(item);
        if (under != nullptr)
            break;
    }

    if (under != nullptr && under->nodeId() != kInputNodeId
        && under->nodeId() != kOutputNodeId)
    {
        const int id = under->nodeId();
        menu.addAction(tr("Bypass"), this, [this, id] { toggleBypass(id); });
        menu.addAction(tr("Remove"), this, [this, id] { removeNode(id); });
    }
    else
    {
        QMenu* add = menu.addMenu(tr("Add effect"));
        for (int type : m_availableTypes)
        {
            add->addAction(nameForType(type), this,
                           [this, type, scenePos] { addNodeAt(type, scenePos); });
        }
        if (m_availableTypes.empty())
            add->setEnabled(false);
    }

    menu.exec(event->globalPos());
}

void GraphCanvas::drawBackground(QPainter* painter, const QRectF& rect)
{
    painter->fillRect(rect, QColor(30, 31, 34));

    // A grid, so dragging a node reads as movement rather than drift.
    painter->setPen(QPen(QColor(44, 45, 50), 1.0));
    const qreal left = std::floor(rect.left() / kGridStep) * kGridStep;
    const qreal top  = std::floor(rect.top() / kGridStep) * kGridStep;
    for (qreal x = left; x < rect.right(); x += kGridStep)
        painter->drawLine(QPointF(x, rect.top()), QPointF(x, rect.bottom()));
    for (qreal y = top; y < rect.bottom(); y += kGridStep)
        painter->drawLine(QPointF(rect.left(), y), QPointF(rect.right(), y));

    // The wire currently being dragged, if any.
    for (GraphNodeItem* item : m_nodeItems)
    {
        if (!item->wiring())
            continue;
        painter->setPen(QPen(QColor(120, 190, 255), 2.0, Qt::DashLine));
        painter->setBrush(Qt::NoBrush);
        painter->drawPath(wirePath(item->outputPortScenePos(), item->wireEnd()));
    }
}
