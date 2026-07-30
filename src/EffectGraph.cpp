/*
  rakarrack - guitar multi-effects processor
  SPDX-License-Identifier: GPL-2.0-only

  EffectGraph.cpp - Node graph for arbitrary effect routing.
*/

#include "EffectGraph.hpp"
#include "Effect.hpp"

#include <algorithm>
#include <cstring>

EffectGraph::EffectGraph() = default;
EffectGraph::~EffectGraph() = default;

// ─── Layout-level validation ───────────────────────────────────────

bool layoutCanConnect(const GraphLayout& layout, int from, int to)
{
    // Signal only ever leaves the input and only ever enters the output.
    if (to == kInputNodeId || from == kOutputNodeId)
        return false;
    if (from == to)
        return false;

    const auto known = [&layout](int id) {
        if (id == kInputNodeId || id == kOutputNodeId)
            return true;
        for (const GraphNodeLayout& n : layout.nodes)
            if (n.id == id)
                return true;
        return false;
    };
    if (!known(from) || !known(to))
        return false;

    for (const Connection& c : layout.connections)
        if (c.from == from && c.to == to)
            return false;

    // A cycle exists if `to` already reaches `from`. Walk forwards from `to`;
    // the endpoints are not nodes, so nothing leaves the output and the search
    // terminates there.
    std::vector<int> pending{to};
    std::vector<int> seen;
    while (!pending.empty())
    {
        const int at = pending.back();
        pending.pop_back();
        if (at == from)
            return false;
        if (std::find(seen.begin(), seen.end(), at) != seen.end())
            continue;
        seen.push_back(at);

        for (const Connection& c : layout.connections)
            if (c.from == at)
                pending.push_back(c.to);
    }

    return true;
}

// ─── Mix modes ─────────────────────────────────────────────────────

MixMode defaultMixModeForType(int type)
{
    // Transcribed from the legacy switch in RKR::Alg(). Everything not listed
    // here crossfaded against the pre-effect signal via Vol_Efx().
    switch (type)
    {
    // Vol2_Efx(): output used unchanged.
    case  0: case  1: case  9: case 16: case 20: case 22:
    case 25: case 33: case 34: case 36: case 39: case 44:
        return MixMode::Replace;

    // Vol3_Efx(): Cabinet is the only effect that doubled its output.
    case 12:
        return MixMode::Gain2x;

    default:
        return MixMode::WetDry;
    }
}

namespace {

/// Reproduces RKR::Vol_Efx()'s crossfade for one node.
void applyWetDry(int type, float outvolume,
                 const float* dryL, const float* dryR,
                 float* wetL, float* wetR, std::size_t frames)
{
    float v1 = 0.0f;
    float v2 = 0.0f;
    if (outvolume < 0.5f)
    {
        v1 = 1.0f;
        v2 = outvolume * 2.0f;
    }
    else
    {
        v1 = (1.0f - outvolume) * 2.0f;
        v2 = 1.0f;
    }

    // Reverb (8) and MusicDelay (15) square the dry coefficient. Preserved
    // from the legacy Vol_Efx() so those two sound unchanged.
    if (type == 8 || type == 15)
        v2 *= v2;

    for (std::size_t i = 0; i < frames; ++i)
    {
        wetL[i] = dryL[i] * v2 + wetL[i] * v1;
        wetR[i] = dryR[i] * v2 + wetR[i] * v1;
    }
}

} // namespace

// ─── Topology ──────────────────────────────────────────────────────

int EffectGraph::indexOf(int id) const
{
    for (std::size_t i = 0; i < m_nodes.size(); ++i)
        if (m_nodes[i].id == id)
            return static_cast<int>(i);
    return -1;
}

EffectNode* EffectGraph::findNode(int id)
{
    const int idx = indexOf(id);
    return (idx < 0) ? nullptr : &m_nodes[static_cast<std::size_t>(idx)];
}

const EffectNode* EffectGraph::findNode(int id) const
{
    const int idx = indexOf(id);
    return (idx < 0) ? nullptr : &m_nodes[static_cast<std::size_t>(idx)];
}

int EffectGraph::addNode(int type, std::unique_ptr<Effect> effect, float x, float y)
{
    // Register it as a borrowed node first, then hand the ownership over, so
    // there is only one copy of the node setup logic.
    Effect* raw = effect.get();
    const int id = addBorrowedNode(type, raw, x, y);
    m_nodes.back().owned = std::move(effect);
    return id;
}

int EffectGraph::addBorrowedNode(int type, Effect* effect, float x, float y)
{
    EffectNode node;
    node.id       = m_nextId++;
    node.type     = type;
    node.bypassed = false;
    node.mix      = defaultMixModeForType(type);
    node.x        = x;
    node.y        = y;
    node.effect   = effect;

    if (node.effect && m_maxBlockSize > 0)
        node.effect->setMaxBlockSize(m_maxBlockSize);

    m_nodes.push_back(std::move(node));

    m_bufL.emplace_back(static_cast<std::size_t>(std::max(m_maxBlockSize, 0)), 0.0f);
    m_bufR.emplace_back(static_cast<std::size_t>(std::max(m_maxBlockSize, 0)), 0.0f);

    rebuildOrder();
    return m_nodes.back().id;
}

bool EffectGraph::removeNode(int id)
{
    const int idx = indexOf(id);
    if (idx < 0)
        return false;

    std::erase_if(m_connections, [id](const Connection& c) {
        return c.from == id || c.to == id;
    });

    m_nodes.erase(m_nodes.begin() + idx);
    m_bufL.erase(m_bufL.begin() + idx);
    m_bufR.erase(m_bufR.begin() + idx);

    rebuildOrder();
    return true;
}

bool EffectGraph::connect(int from, int to)
{
    if (from == to)
        return false;
    // The endpoints are one-way: nothing feeds the input, nothing leaves the output.
    if (to == kInputNodeId || from == kOutputNodeId)
        return false;
    if (from != kInputNodeId && indexOf(from) < 0)
        return false;
    if (to != kOutputNodeId && indexOf(to) < 0)
        return false;

    const Connection edge{from, to};
    if (std::find(m_connections.begin(), m_connections.end(), edge) != m_connections.end())
        return false;

    if (wouldCreateCycle(from, to))
        return false;

    m_connections.push_back(edge);
    rebuildOrder();
    return true;
}

bool EffectGraph::disconnect(int from, int to)
{
    const Connection edge{from, to};
    const auto it = std::find(m_connections.begin(), m_connections.end(), edge);
    if (it == m_connections.end())
        return false;

    m_connections.erase(it);
    rebuildOrder();
    return true;
}

bool EffectGraph::wouldCreateCycle(int from, int to) const
{
    // Adding from->to closes a loop exactly when `to` can already reach `from`.
    if (from == to)
        return true;
    if (to == kOutputNodeId || from == kInputNodeId)
        return false;   // endpoints can never be part of a loop

    std::vector<int> stack{to};
    std::vector<int> seen;

    while (!stack.empty())
    {
        const int current = stack.back();
        stack.pop_back();

        if (current == from)
            return true;
        if (std::find(seen.begin(), seen.end(), current) != seen.end())
            continue;
        seen.push_back(current);

        for (const Connection& c : m_connections)
            if (c.from == current && c.to != kOutputNodeId)
                stack.push_back(c.to);
    }

    return false;
}

void EffectGraph::clear()
{
    m_connections.clear();
    m_nodes.clear();
    m_bufL.clear();
    m_bufR.clear();
    m_order.clear();
}

void EffectGraph::buildSeriesChain(const std::vector<int>& types,
                                   std::vector<std::unique_ptr<Effect>> effects)
{
    clear();

    int previous = kInputNodeId;
    for (std::size_t i = 0; i < types.size() && i < effects.size(); ++i)
    {
        // Lay the chain out left to right so it reads like the old rack.
        const int id = addNode(types[i], std::move(effects[i]),
                               static_cast<float>(i) * 180.0f, 0.0f);
        connect(previous, id);
        previous = id;
    }

    connect(previous, kOutputNodeId);
}

void EffectGraph::setNodeBypassed(int id, bool bypassed)
{
    if (EffectNode* node = findNode(id))
        node->bypassed = bypassed;
}

void EffectGraph::setNodeMixMode(int id, MixMode mix)
{
    if (EffectNode* node = findNode(id))
        node->mix = mix;
}

GraphLayout EffectGraph::layout() const
{
    GraphLayout out;
    out.nodes.reserve(m_nodes.size());

    for (const EffectNode& node : m_nodes)
    {
        GraphNodeLayout n;
        n.id       = node.id;
        n.type     = node.type;
        n.bypassed = node.bypassed;
        n.mix      = node.mix;
        n.x        = node.x;
        n.y        = node.y;
        if (node.effect)
            n.settings = captureEffectSettings(*node.effect);
        out.nodes.push_back(std::move(n));
    }

    out.connections = m_connections;
    return out;
}

bool EffectGraph::build(const GraphLayout& layout,
                        const std::function<std::unique_ptr<Effect>(int)>& make)
{
    clear();

    for (const GraphNodeLayout& n : layout.nodes)
    {
        std::unique_ptr<Effect> effect = make ? make(n.type) : nullptr;
        if (!effect)
        {
            // An unknown effect type would silently change the signal path.
            clear();
            return false;
        }

        EffectNode node;
        node.id       = n.id;
        node.type     = n.type;
        node.bypassed = n.bypassed;
        node.mix      = n.mix;
        node.x        = n.x;
        node.y        = n.y;
        node.owned    = std::move(effect);
        node.effect   = node.owned.get();

        applyEffectSettings(*node.effect, n.settings);

        if (m_maxBlockSize > 0)
            node.effect->setMaxBlockSize(m_maxBlockSize);

        m_nodes.push_back(std::move(node));
        m_bufL.emplace_back(static_cast<std::size_t>(std::max(m_maxBlockSize, 0)), 0.0f);
        m_bufR.emplace_back(static_cast<std::size_t>(std::max(m_maxBlockSize, 0)), 0.0f);

        // Keep handing out ids above anything the file used.
        if (n.id >= m_nextId)
            m_nextId = n.id + 1;
    }

    // connect() re-checks endpoints, duplicates and cycles, so a hand-edited
    // file cannot produce a graph the evaluator would not accept.
    for (const Connection& c : layout.connections)
    {
        if (!connect(c.from, c.to))
        {
            clear();
            return false;
        }
    }

    rebuildOrder();
    return true;
}

bool EffectGraph::isFullyConnected() const
{
    for (const EffectNode& node : m_nodes)
    {
        const bool hasIn = std::any_of(m_connections.begin(), m_connections.end(),
                                       [&](const Connection& c) { return c.to == node.id; });
        const bool hasOut = std::any_of(m_connections.begin(), m_connections.end(),
                                        [&](const Connection& c) { return c.from == node.id; });
        if (!hasIn || !hasOut)
            return false;
    }
    return true;
}

// ─── Ordering ──────────────────────────────────────────────────────

void EffectGraph::rebuildOrder()
{
    m_order.clear();
    m_order.reserve(m_nodes.size());

    // Kahn's algorithm. Only edges between real nodes constrain the order;
    // edges from the input endpoint never block a node from being ready.
    std::vector<int> indegree(m_nodes.size(), 0);
    for (const Connection& c : m_connections)
    {
        if (c.from == kInputNodeId || c.to == kOutputNodeId)
            continue;
        const int idx = indexOf(c.to);
        if (idx >= 0)
            ++indegree[static_cast<std::size_t>(idx)];
    }

    std::vector<int> ready;
    for (std::size_t i = 0; i < m_nodes.size(); ++i)
        if (indegree[i] == 0)
            ready.push_back(m_nodes[i].id);

    while (!ready.empty())
    {
        const int id = ready.front();
        ready.erase(ready.begin());
        m_order.push_back(id);

        for (const Connection& c : m_connections)
        {
            if (c.from != id || c.to == kOutputNodeId)
                continue;
            const int idx = indexOf(c.to);
            if (idx >= 0 && --indegree[static_cast<std::size_t>(idx)] == 0)
                ready.push_back(m_nodes[static_cast<std::size_t>(idx)].id);
        }
    }

    // connect() rejects cycles, so every node must have been scheduled.
    // If that ever fails, fall back to declaration order rather than dropping
    // nodes silently.
    if (m_order.size() != m_nodes.size())
    {
        m_order.clear();
        for (const EffectNode& n : m_nodes)
            m_order.push_back(n.id);
    }
}

// ─── Audio ─────────────────────────────────────────────────────────

void EffectGraph::setMaxBlockSize(int maxBlockSize)
{
    m_maxBlockSize = maxBlockSize;

    const auto n = static_cast<std::size_t>(std::max(maxBlockSize, 0));
    for (auto& b : m_bufL) b.assign(n, 0.0f);
    for (auto& b : m_bufR) b.assign(n, 0.0f);
    m_outAccumL.assign(n, 0.0f);
    m_outAccumR.assign(n, 0.0f);
    m_dryL.assign(n, 0.0f);
    m_dryR.assign(n, 0.0f);

    for (EffectNode& node : m_nodes)
        if (node.effect)
            node.effect->setMaxBlockSize(maxBlockSize);
}

void EffectGraph::process(const float* inL, const float* inR,
                          float* outL, float* outR, int nframes)
{
    const auto frames = static_cast<std::size_t>(nframes);

    for (const int id : m_order)
    {
        const int idx = indexOf(id);
        if (idx < 0)
            continue;

        auto& bufL = m_bufL[static_cast<std::size_t>(idx)];
        auto& bufR = m_bufR[static_cast<std::size_t>(idx)];

        // Gather: the first incoming edge copies, the rest sum on top of it.
        bool seeded = false;
        for (const Connection& c : m_connections)
        {
            if (c.to != id)
                continue;

            const float* srcL = nullptr;
            const float* srcR = nullptr;
            if (c.from == kInputNodeId)
            {
                srcL = inL;
                srcR = inR;
            }
            else
            {
                const int srcIdx = indexOf(c.from);
                if (srcIdx < 0)
                    continue;
                srcL = m_bufL[static_cast<std::size_t>(srcIdx)].data();
                srcR = m_bufR[static_cast<std::size_t>(srcIdx)].data();
            }

            if (!seeded)
            {
                std::memcpy(bufL.data(), srcL, frames * sizeof(float));
                std::memcpy(bufR.data(), srcR, frames * sizeof(float));
                seeded = true;
            }
            else
            {
                for (std::size_t i = 0; i < frames; ++i)
                {
                    bufL[i] += srcL[i];
                    bufR[i] += srcR[i];
                }
            }
        }

        // An unconnected input means silence into this node.
        if (!seeded)
        {
            std::memset(bufL.data(), 0, frames * sizeof(float));
            std::memset(bufR.data(), 0, frames * sizeof(float));
        }

        // Effects transform in place; a bypassed node just passes its input on.
        EffectNode& node = m_nodes[static_cast<std::size_t>(idx)];
        if (!node.bypassed && node.effect)
        {
            // A crossfading node needs its input kept aside, since out() is
            // destructive.
            if (node.mix == MixMode::WetDry)
            {
                std::memcpy(m_dryL.data(), bufL.data(), frames * sizeof(float));
                std::memcpy(m_dryR.data(), bufR.data(), frames * sizeof(float));
            }

            node.effect->out(bufL.data(), bufR.data(), nframes);

            switch (node.mix)
            {
            case MixMode::Replace:
                break;
            case MixMode::Gain2x:
                for (std::size_t i = 0; i < frames; ++i)
                {
                    bufL[i] *= 2.0f;
                    bufR[i] *= 2.0f;
                }
                break;
            case MixMode::WetDry:
                applyWetDry(node.type, node.effect->outvolume,
                            m_dryL.data(), m_dryR.data(),
                            bufL.data(), bufR.data(), frames);
                break;
            }
        }
    }

    // Mix everything feeding the output endpoint.
    bool seeded = false;
    for (const Connection& c : m_connections)
    {
        if (c.to != kOutputNodeId)
            continue;

        const float* srcL = nullptr;
        const float* srcR = nullptr;
        if (c.from == kInputNodeId)
        {
            srcL = inL;
            srcR = inR;
        }
        else
        {
            const int srcIdx = indexOf(c.from);
            if (srcIdx < 0)
                continue;
            srcL = m_bufL[static_cast<std::size_t>(srcIdx)].data();
            srcR = m_bufR[static_cast<std::size_t>(srcIdx)].data();
        }

        if (!seeded)
        {
            std::memcpy(m_outAccumL.data(), srcL, frames * sizeof(float));
            std::memcpy(m_outAccumR.data(), srcR, frames * sizeof(float));
            seeded = true;
        }
        else
        {
            for (std::size_t i = 0; i < frames; ++i)
            {
                m_outAccumL[i] += srcL[i];
                m_outAccumR[i] += srcR[i];
            }
        }
    }

    if (seeded)
    {
        std::memcpy(outL, m_outAccumL.data(), frames * sizeof(float));
        std::memcpy(outR, m_outAccumR.data(), frames * sizeof(float));
    }
    else
    {
        std::memset(outL, 0, frames * sizeof(float));
        std::memset(outR, 0, frames * sizeof(float));
    }
}
