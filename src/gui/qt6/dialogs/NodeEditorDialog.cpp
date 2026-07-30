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

#include <QDialogButtonBox>
#include <QLabel>
#include <QPushButton>
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

    m_canvas->setLayout(layoutFromEngine());
    updateStatus();
}

void NodeEditorDialog::setupUi()
{
    setWindowTitle(tr("Node Editor"));
    resize(1000, 620);

    auto* layout = new QVBoxLayout(this);

    m_canvas = new GraphCanvas(this);
    layout->addWidget(m_canvas, 1);

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

    if (chain.empty() && !layout.nodes.empty())
    {
        m_status->setText(
            tr("This patch branches, so it cannot be applied to the effect "
               "chain yet — the chain is a plain series of up to %1 slots. "
               "Straighten it into a single path to apply it.")
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
    const std::vector<int> chain = seriesChainTypes();
    if (chain.empty() && !m_canvas->layout().nodes.empty())
        return;                        // updateStatus() has already explained why
    if (chain.size() > static_cast<std::size_t>(kMaxEffectSlots))
        return;

    std::array<int, kMaxEffectSlots> order{};
    order.fill(EMPTY_SLOT);
    for (std::size_t i = 0; i < chain.size(); ++i)
        order[i] = chain[i];

    m_engine.setEffectOrder(order);

    for (const GraphNodeLayout& n : m_canvas->layout().nodes)
        m_engine.setEffectEnabled(n.type, !n.bypassed);

    Q_EMIT applied();
}
