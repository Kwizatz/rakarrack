/*
  rakarrack - guitar multi-effects processor
  SPDX-License-Identifier: GPL-2.0-only

  EffectGraph.hpp - Node graph for arbitrary effect routing.

  Replaces the legacy model (a flat efx_order[16] run in place on one shared
  stereo bus) with a directed acyclic graph of effect instances, so pedals can
  be duplicated, split into parallel paths and mixed back together.

  Design notes:
    - Nodes are addressed by a stable id, so connections survive removals.
    - Two reserved ids act as the graph's endpoints: kInputNodeId is the signal
      coming in, kOutputNodeId is what leaves.
    - Cycles are rejected at connect() time, which keeps process() a plain
      single pass over a precomputed order.
    - A node with several incoming edges sums them (a merge). A node with
      several outgoing edges feeds each of them the same signal (a split).
    - process() never allocates: buffers are sized by setMaxBlockSize() and the
      processing order is recomputed only when the topology changes.
*/

#pragma once

#include <memory>
#include <string>
#include <vector>

class Effect;

/// The signal entering the graph.
inline constexpr int kInputNodeId = -1;
/// The signal leaving the graph.
inline constexpr int kOutputNodeId = -2;

/// One effect instance placed on the board.
struct EffectNode
{
    int  id{0};
    int  type{0};                     ///< Effect type index (0..46)
    bool bypassed{false};

    /// Canvas position. Carried here so it round-trips with the preset;
    /// the engine itself never reads it.
    float x{0.0f};
    float y{0.0f};

    std::unique_ptr<Effect> effect;
};

/// A directed edge. `from`/`to` are node ids, or the reserved endpoint ids.
struct Connection
{
    int from{kInputNodeId};
    int to{kOutputNodeId};

    friend bool operator==(const Connection&, const Connection&) = default;
};

class EffectGraph
{
public:
    EffectGraph();
    ~EffectGraph();

    EffectGraph(const EffectGraph&) = delete;
    EffectGraph& operator=(const EffectGraph&) = delete;

    // ─── Topology (GUI/loader thread) ──────────────────────────────

    /// Take ownership of `effect` as a new node of the given type.
    /// Returns the new node id.
    int addNode(int type, std::unique_ptr<Effect> effect, float x = 0.0f, float y = 0.0f);

    /// Remove a node and every connection touching it. Returns false if absent.
    bool removeNode(int id);

    /// Connect `from` -> `to`. Returns false if either endpoint is unknown, the
    /// edge already exists, or it would introduce a cycle.
    bool connect(int from, int to);

    /// Remove a connection. Returns false if it was not present.
    bool disconnect(int from, int to);

    /// True if adding `from` -> `to` would make the graph cyclic.
    [[nodiscard]] bool wouldCreateCycle(int from, int to) const;

    /// Drop every node and connection.
    void clear();

    /// Replace the graph with a plain series chain of the given effect types,
    /// which is how a legacy efx_order[] preset maps onto the graph.
    /// `effects` must line up with `types`.
    void buildSeriesChain(const std::vector<int>& types,
                          std::vector<std::unique_ptr<Effect>> effects);

    // ─── Queries ───────────────────────────────────────────────────

    [[nodiscard]] const std::vector<EffectNode>& nodes() const { return m_nodes; }
    [[nodiscard]] const std::vector<Connection>& connections() const { return m_connections; }

    [[nodiscard]] EffectNode* findNode(int id);
    [[nodiscard]] const EffectNode* findNode(int id) const;

    /// True if every node lies on a path from the input to the output.
    /// Orphaned nodes are legal but silent, so this is advisory for the GUI.
    [[nodiscard]] bool isFullyConnected() const;

    void setNodeBypassed(int id, bool bypassed);

    // ─── Audio ─────────────────────────────────────────────────────

    /// Size the per-node buffers. Must be called before process(), and again
    /// whenever the host block size grows.
    void setMaxBlockSize(int maxBlockSize);

    /// Run one block through the graph. Input and output may not overlap.
    /// Allocation-free provided setMaxBlockSize() covered `nframes`.
    void process(const float* inL, const float* inR,
                 float* outL, float* outR, int nframes);

private:
    /// Recompute m_order (Kahn's algorithm). Called on any topology change.
    void rebuildOrder();

    /// Index into m_nodes for a node id, or -1.
    [[nodiscard]] int indexOf(int id) const;

    std::vector<EffectNode> m_nodes;
    std::vector<Connection> m_connections;

    /// Node ids in a valid evaluation order (endpoints excluded).
    std::vector<int> m_order;

    /// Per-node accumulated input, parallel to m_nodes.
    std::vector<std::vector<float>> m_bufL;
    std::vector<std::vector<float>> m_bufR;

    /// Accumulator for the output node.
    std::vector<float> m_outAccumL;
    std::vector<float> m_outAccumR;

    int m_nextId{1};
    int m_maxBlockSize{0};
};
