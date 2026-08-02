/*
  rakarrack - guitar multi-effects processor
  SPDX-License-Identifier: GPL-2.0-only

  Qt6 GUI - Main node editor.
*/

#include "NodeEditor.hpp"

#include "GraphCanvas.hpp"
#include "../panels/EffectPanel.hpp"
#include "EngineController.hpp"
#include "EffectRegistry.hpp"
#include "dsp_constants.hpp"

#include <QAbstractItemView>
#include <QDrag>
#include <QFrame>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMimeData>
#include <QScrollArea>
#include <QSplitter>
#include <QVBoxLayout>

#include <algorithm>
#include <array>
#include <utility>
#include <vector>

namespace {

class EffectListWidget final : public QListWidget
{
public:
    using QListWidget::QListWidget;

protected:
    void startDrag(Qt::DropActions) override
    {
        QListWidgetItem* item = currentItem();
        if (item == nullptr)
            return;

        auto* mime = new QMimeData;
        mime->setData(kEffectTypeMimeType,
                      QByteArray::number(item->data(Qt::UserRole).toInt()));

        QDrag drag(this);
        drag.setMimeData(mime);
        drag.exec(Qt::CopyAction);
    }
};

} // namespace

NodeEditor::NodeEditor(EngineController& engine, QWidget* parent)
    : QWidget(parent), m_engine(engine)
{
    // The main editor is the routing authority. The legacy chain remains an
    // engine fallback, but there is no second signal-path mode in this UI.
    m_engine.setGraphPathActive(true);

    setupUi();

    m_canvas->setNameLookup([this](int type) {
        return QString::fromStdString(m_engine.getEffectTypeName(type));
    });

    std::vector<int> types;
    types.reserve(kEffectTypeCount);
    for (int type = 0; type < kEffectTypeCount; ++type)
        types.push_back(type);
    m_canvas->setAvailableTypes(std::move(types));

    populateEffectList();
    syncFromEngine();
}

void NodeEditor::setupUi()
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    auto* splitter = new QSplitter(Qt::Horizontal, this);
    splitter->setChildrenCollapsible(false);
    layout->addWidget(splitter);

    m_canvas = new GraphCanvas(splitter);
    splitter->addWidget(m_canvas);

    auto* sidebar = new QWidget(splitter);
    sidebar->setMinimumWidth(240);
    auto* sidebarLayout = new QVBoxLayout(sidebar);
    sidebarLayout->setContentsMargins(8, 4, 4, 4);
    sidebarLayout->setSpacing(6);

    auto* effectsTitle = new QLabel(tr("Effects"), sidebar);
    QFont effectsFont = effectsTitle->font();
    effectsFont.setBold(true);
    effectsTitle->setFont(effectsFont);
    sidebarLayout->addWidget(effectsTitle);

    m_filter = new QLineEdit(sidebar);
    m_filter->setPlaceholderText(tr("Filter effects"));
    m_filter->setClearButtonEnabled(true);
    sidebarLayout->addWidget(m_filter);

    m_effectList = new EffectListWidget(sidebar);
    m_effectList->setDragEnabled(true);
    m_effectList->setDragDropMode(QAbstractItemView::DragOnly);
    m_effectList->setDefaultDropAction(Qt::CopyAction);
    m_effectList->setSelectionMode(QAbstractItemView::SingleSelection);
    m_effectList->setUniformItemSizes(true);
    sidebarLayout->addWidget(m_effectList, 2);

    auto* settingsTitle = new QLabel(tr("Node Settings"), sidebar);
    QFont settingsFont = settingsTitle->font();
    settingsFont.setBold(true);
    settingsTitle->setFont(settingsFont);
    sidebarLayout->addWidget(settingsTitle);

    auto* scroll = new QScrollArea(sidebar);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);

    auto* paramHost = new QWidget(scroll);
    m_paramLayout = new QVBoxLayout(paramHost);
    m_paramLayout->setContentsMargins(0, 0, 0, 0);

    m_paramNote = new QLabel(paramHost);
    m_paramNote->setWordWrap(true);
    m_paramLayout->addWidget(m_paramNote);
    m_paramLayout->addStretch(1);
    scroll->setWidget(paramHost);
    sidebarLayout->addWidget(scroll, 3);

    m_status = new QLabel(sidebar);
    m_status->setWordWrap(true);
    sidebarLayout->addWidget(m_status);

    splitter->addWidget(sidebar);
    splitter->setStretchFactor(0, 3);
    splitter->setStretchFactor(1, 1);
    splitter->setSizes({900, 300});

    connect(m_filter, &QLineEdit::textChanged, this,
            [this](const QString& text)
            {
                for (int row = 0; row < m_effectList->count(); ++row)
                {
                    QListWidgetItem* item = m_effectList->item(row);
                    item->setHidden(!item->text().contains(
                        text, Qt::CaseInsensitive));
                }
            });
    connect(m_canvas, &GraphCanvas::layoutEdited,
            this, &NodeEditor::onLayoutEdited);
    connect(m_canvas, &GraphCanvas::nodeSelected,
            this, &NodeEditor::onNodeSelected);
}

void NodeEditor::populateEffectList()
{
    std::vector<std::pair<QString, int>> effects;
    effects.reserve(kEffectTypeCount);
    for (int type = 0; type < kEffectTypeCount; ++type)
    {
        effects.emplace_back(
            QString::fromStdString(m_engine.getEffectTypeName(type)), type);
    }

    std::sort(effects.begin(), effects.end(), [](const auto& left, const auto& right) {
        return QString::localeAwareCompare(left.first, right.first) < 0;
    });

    for (const auto& [name, type] : effects)
    {
        auto* item = new QListWidgetItem(name, m_effectList);
        item->setData(Qt::UserRole, type);
        item->setFlags(item->flags() | Qt::ItemIsDragEnabled);
    }
}

GraphLayout NodeEditor::layoutFromEngine() const
{
    GraphLayout layout;
    std::array<bool, kEffectTypeCount> seen{};
    int previous = kInputNodeId;
    int nextId = 1;

    for (int type : m_engine.getEffectOrder())
    {
        if (type == EMPTY_SLOT)
            continue;
        if (type < 0 || type >= kEffectTypeCount || seen[type])
            continue;
        seen[type] = true;

        GraphNodeLayout node;
        node.id       = nextId++;
        node.type     = type;
        node.mix      = defaultMixModeForType(type);
        node.bypassed = !m_engine.isEffectEnabled(type);
        node.settings = m_engine.getEffectSettings(type);
        layout.nodes.push_back(std::move(node));

        layout.connections.push_back({previous, layout.nodes.back().id});
        previous = layout.nodes.back().id;
    }

    layout.connections.push_back({previous, kOutputNodeId});
    return layout;
}

void NodeEditor::syncFromEngine()
{
    const bool hasOwnedGraph = m_engine.hasNodeInstances();
    const GraphLayout layout = hasOwnedGraph
        ? m_engine.getGraphLayout()
        : layoutFromEngine();

    clearNodeEditor(tr("No node selected."));
    m_canvas->setLayout(layout);

    if (hasOwnedGraph)
        updateStatus();
    else
        applyLayout();
}

void NodeEditor::onLayoutEdited()
{
    applyLayout();
}

void NodeEditor::applyLayout()
{
    if (!m_engine.setGraphLayout(m_canvas->layout()))
    {
        m_status->setText(tr("The patch could not be applied."));
        return;
    }
    updateStatus();
}

void NodeEditor::updateStatus()
{
    const GraphLayout& layout = m_canvas->layout();
    const bool direct = std::find(layout.connections.begin(),
                                  layout.connections.end(),
                                  Connection{kInputNodeId, kOutputNodeId})
                        != layout.connections.end();

    if (layout.nodes.empty())
    {
        m_status->setText(direct
            ? tr("Direct input to output - live")
            : tr("Output disconnected - silence"));
        return;
    }

    m_status->setText(tr("%n effect(s) - live", nullptr,
                         static_cast<int>(layout.nodes.size())));
}

void NodeEditor::clearNodeEditor(const QString& reason)
{
    delete m_paramPanel;
    m_paramPanel = nullptr;
    m_paramNodeId = -1;
    m_paramNote->setText(reason);
    m_paramNote->setVisible(true);
}

void NodeEditor::showNodeEditor(int nodeId)
{
    if (nodeId == m_paramNodeId && m_paramPanel != nullptr)
        return;

    const int type = m_engine.getNodeType(nodeId);
    if (type < 0)
    {
        clearNodeEditor(tr("The selected node is not running."));
        return;
    }

    auto panel = EffectPanel::create(type, m_engine, nullptr);
    if (!panel)
    {
        clearNodeEditor(tr("No controls for this effect."));
        return;
    }

    clearNodeEditor(QString());
    m_paramNote->setVisible(false);

    m_paramPanel = panel.release();
    m_paramNodeId = nodeId;
    m_paramPanel->setTargetNode(nodeId);
    connect(m_paramPanel, &EffectPanel::bypassChanged, this,
            [this, nodeId](int, bool active)
            {
                if (m_paramNodeId == nodeId)
                    m_canvas->setNodeBypassed(nodeId, !active);
            });
    m_paramLayout->insertWidget(0, m_paramPanel);
    m_paramPanel->show();
}

void NodeEditor::onNodeSelected(int nodeId)
{
    if (nodeId < 0)
    {
        clearNodeEditor(tr("No node selected."));
        return;
    }
    showNodeEditor(nodeId);
}