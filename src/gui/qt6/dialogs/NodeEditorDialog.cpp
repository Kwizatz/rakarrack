/*
  rakarrack - guitar multi-effects processor
  SPDX-License-Identifier: GPL-2.0-only

  Qt6 GUI - Node editor dialog.
*/

#include "NodeEditorDialog.hpp"

#include "EngineController.hpp"
#include "EffectRegistry.hpp"
#include "dsp_constants.hpp"
#include "../widgets/GraphCanvas.hpp"
#include "../panels/EffectPanel.hpp"

#include <QDialogButtonBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>

#include <algorithm>

NodeEditorDialog::NodeEditorDialog(EngineController& engine, QWidget* parent)
    : QDialog(parent), m_engine(engine)
{
    setupUi();

    m_canvas->setNameLookup([this](int type) {
        return QString::fromStdString(m_engine.getEffectTypeName(type));
    });

    std::vector<int> types;
    types.reserve(kEffectTypeCount);
    for (int t = 0; t < kEffectTypeCount; ++t)
        types.push_back(t);
    m_canvas->setAvailableTypes(std::move(types));

    // Open on whatever the engine is actually running. With the graph active
    // that is the graph itself, which may already branch; otherwise the fixed
    // chain converted to a series of nodes.
    GraphLayout opening = m_engine.getGraphLayout();
    if (!m_engine.isGraphPathActive() || opening.nodes.empty())
        opening = layoutFromEngine();
    m_canvas->setLayout(opening);
    updateStatus();

    // Open on the first node rather than an empty pane, so the settings are
    // one click away instead of hidden behind a selection nobody made yet.
    if (m_engine.hasNodeInstances() && !opening.nodes.empty())
        onNodeSelected(opening.nodes.front().id);
    else
        clearNodeEditor(tr("Select a node to edit its settings."));
}

void NodeEditorDialog::setupUi()
{
    setWindowTitle(tr("Node Editor"));
    resize(1240, 640);

    auto* layout = new QVBoxLayout(this);

    auto* split = new QHBoxLayout;
    layout->addLayout(split, 1);

    m_canvas = new GraphCanvas(this);
    split->addWidget(m_canvas, 1);

    // The settings of whichever node is selected. Panels are per effect type,
    // so this is the only place a second Chorus in a patch can be reached --
    // the main window's panels address the type, not the node.
    auto* box = new QGroupBox(tr("Node Settings"), this);
    box->setMinimumWidth(300);
    auto* boxLayout = new QVBoxLayout(box);

    auto* scroll = new QScrollArea(box);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    boxLayout->addWidget(scroll);

    auto* host = new QWidget(scroll);
    m_paramLayout = new QVBoxLayout(host);
    m_paramLayout->setContentsMargins(0, 0, 0, 0);

    m_paramNote = new QLabel(host);
    m_paramNote->setWordWrap(true);
    m_paramLayout->addWidget(m_paramNote);
    m_paramLayout->addStretch(1);

    scroll->setWidget(host);
    split->addWidget(box);

    m_status = new QLabel(this);
    m_status->setWordWrap(true);
    layout->addWidget(m_status);

    auto* buttons = new QDialogButtonBox(
        QDialogButtonBox::Apply | QDialogButtonBox::Close, this);
    layout->addWidget(buttons);

    connect(buttons->button(QDialogButtonBox::Apply), &QPushButton::clicked,
            this, &NodeEditorDialog::onApply);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(m_canvas, &GraphCanvas::layoutEdited,
            this, &NodeEditorDialog::onLayoutEdited);
    connect(m_canvas, &GraphCanvas::nodeSelected,
            this, &NodeEditorDialog::onNodeSelected);
}

// ---------------------------------------------------------------------------
// Per-node settings pane
// ---------------------------------------------------------------------------

void NodeEditorDialog::clearNodeEditor(const QString& reason)
{
    delete m_paramPanel;          // also removes it from the layout
    m_paramPanel = nullptr;
    m_paramNodeId = -1;
    m_paramNote->setText(reason);
    m_paramNote->setVisible(true);
}

void NodeEditorDialog::showNodeEditor(int nodeId)
{
    if (nodeId == m_paramNodeId && m_paramPanel)
        return;

    // Settings live on the running instance, so there is nothing to edit until
    // the patch has been applied and the node exists in the engine.
    const int liveType = m_engine.getNodeType(nodeId);
    if (liveType < 0)
    {
        clearNodeEditor(tr("This node is not running yet. Apply the patch to "
                           "edit its settings."));
        return;
    }

    auto panel = EffectPanel::create(liveType, m_engine, nullptr);
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
    m_paramLayout->insertWidget(0, m_paramPanel);
    m_paramPanel->show();
}

void NodeEditorDialog::onNodeSelected(int nodeId)
{
    if (nodeId < 0)
    {
        clearNodeEditor(tr("Select a node to edit its settings."));
        return;
    }

    if (!m_engine.hasNodeInstances())
    {
        clearNodeEditor(tr("Nodes get their own settings once a patch has been "
                           "applied. Until then the panels in the main window "
                           "edit each effect type."));
        return;
    }

    showNodeEditor(nodeId);
}

GraphLayout NodeEditorDialog::layoutFromEngine() const
{
    GraphLayout layout;

    const auto order = m_engine.getEffectOrder();
    int previous = kInputNodeId;
    int nextId = 1;

    for (int slot : order)
    {
        if (slot == EMPTY_SLOT)
            continue;

        GraphNodeLayout n;
        n.id       = nextId++;
        n.type     = slot;
        n.mix      = defaultMixModeForType(slot);
        n.bypassed = !m_engine.isEffectEnabled(slot);
        layout.nodes.push_back(n);

        layout.connections.push_back({previous, n.id});
        previous = n.id;
    }

    layout.connections.push_back({previous, kOutputNodeId});
    return layout;
}

std::vector<int> NodeEditorDialog::seriesChainTypes() const
{
    // Walk from the input. Anything that branches, rejoins, or leaves a node
    // stranded means the layout says more than an effect order can.
    const GraphLayout& layout = m_canvas->layout();

    const auto edgesFrom = [&layout](int id) {
        std::vector<int> out;
        for (const Connection& c : layout.connections)
            if (c.from == id)
                out.push_back(c.to);
        return out;
    };
    const auto edgesInto = [&layout](int id) {
        int count = 0;
        for (const Connection& c : layout.connections)
            if (c.to == id)
                ++count;
        return count;
    };

    std::vector<int> types;
    int at = kInputNodeId;
    std::size_t visited = 0;

    while (at != kOutputNodeId)
    {
        const std::vector<int> next = edgesFrom(at);
        if (next.size() != 1)
            return {};                 // a split, or a dead end
        at = next.front();

        if (at == kOutputNodeId)
            break;
        if (edgesInto(at) != 1)
            return {};                 // a merge

        const auto found = std::find_if(
            layout.nodes.begin(), layout.nodes.end(),
            [at](const GraphNodeLayout& n) { return n.id == at; });
        if (found == layout.nodes.end())
            return {};

        types.push_back(found->type);
        if (++visited > layout.nodes.size())
            return {};                 // should be impossible: cycles are refused
    }

    if (visited != layout.nodes.size())
        return {};                     // something is not on the path

    return types;
}

void NodeEditorDialog::updateStatus()
{
    const GraphLayout& layout = m_canvas->layout();
    const std::vector<int> chain = seriesChainTypes();
    const bool branching = chain.empty() && !layout.nodes.empty();

    if (m_engine.isGraphPathActive())
    {
        m_status->setText(
            branching
                ? tr("%1 effects, routed as a graph. Right-click to add, drag "
                     "from a green port to connect, Delete to remove.")
                      .arg(layout.nodes.size())
                : tr("%1 effects in series. Right-click to add, drag from a "
                     "green port to connect, Delete to remove.")
                      .arg(chain.size()));
        return;
    }

    if (branching)
    {
        m_status->setText(
            tr("This patch branches, which the %1-slot chain cannot hold. "
               "Turn on Settings > Audio > Route through the node graph to "
               "run it, or straighten it into a single path to apply it.")
                .arg(kMaxEffectSlots));
        return;
    }

    if (chain.size() > static_cast<std::size_t>(kMaxEffectSlots))
    {
        m_status->setText(tr("%1 effects, but the chain holds %2.")
                              .arg(chain.size())
                              .arg(kMaxEffectSlots));
        return;
    }

    m_status->setText(tr("%1 effects in series. Right-click to add, "
                         "drag from a green port to connect, Delete to remove.")
                          .arg(chain.size()));
}

void NodeEditorDialog::onLayoutEdited()
{
    updateStatus();
}

void NodeEditorDialog::onApply()
{
    const GraphLayout& layout = m_canvas->layout();

    // With the graph running the audio, the patch goes across as it is --
    // splits, merges and all.
    if (m_engine.isGraphPathActive())
    {
        if (!m_engine.setGraphLayout(layout))
        {
            m_status->setText(tr("That patch could not be applied: it uses an "
                                 "effect this build does not have."));
            return;
        }

        // The nodes are running now, so the settings pane has something to
        // edit. Rebuild it against the instance that was just created.
        const int shown = m_paramNodeId;
        clearNodeEditor(tr("Select a node to edit its settings."));
        if (shown >= 0)
            showNodeEditor(shown);

        Q_EMIT applied();
        return;
    }

    // Otherwise only what the fixed chain can hold.
    const std::vector<int> chain = seriesChainTypes();
    if (chain.empty() && !layout.nodes.empty())
        return;                        // updateStatus() has already explained why
    if (chain.size() > static_cast<std::size_t>(kMaxEffectSlots))
        return;

    std::array<int, kMaxEffectSlots> order{};
    order.fill(EMPTY_SLOT);
    for (std::size_t i = 0; i < chain.size(); ++i)
        order[i] = chain[i];

    m_engine.setEffectOrder(order);

    for (const GraphNodeLayout& n : layout.nodes)
        m_engine.setEffectEnabled(n.type, !n.bypassed);

    Q_EMIT applied();
}
