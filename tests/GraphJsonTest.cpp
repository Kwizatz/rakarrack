/*
  rakarrack - guitar multi-effects processor
  SPDX-License-Identifier: GPL-2.0-only

  GraphJsonTest.cpp - Round-trip checks for the JSON graph format.

  A graph that loads but wires itself up differently is the dangerous failure
  here, so these check the rebuilt graph actually carries the same signal, not
  just that the same text comes back out.
*/

#include "GraphJson.hpp"
#include "Effect.hpp"

#include <cstdio>
#include <memory>
#include <string>
#include <vector>

// Declared by dsp_constants.hpp; defined by the engine at runtime.
int PERIOD = 256;
unsigned int SAMPLE_RATE = 48000;
float fPERIOD = 256.0f;
float fSAMPLE_RATE = 48000.0f;
float cSAMPLE_RATE = 1.0f / 48000.0f;
int error_num = 0;
int preset = 0;

namespace {

int g_checks = 0;
int g_failures = 0;

void check(bool cond, const char* what)
{
    ++g_checks;
    std::printf("%-58s %s\n", what, cond ? "ok" : "FAILED");
    if (!cond)
        ++g_failures;
}

class AddConst : public Effect
{
public:
    explicit AddConst(float v) : m_v(v) {}
    void out(float* l, float* r, int n) override
    {
        for (int i = 0; i < n; ++i) { l[i] += m_v; r[i] += m_v; }
    }
    void out(float* l, float* r) override { out(l, r, PERIOD); }

    // A settable parameter, so per-node settings can be exercised: two nodes
    // of the same type have to be able to hold different values.
    void changepar(int npar, int value) override
    {
        if (npar == 0) m_offset = value;
    }
    int getpar(int npar) override { return (npar == 0) ? m_offset : 0; }

    [[nodiscard]] int offset() const { return m_offset; }

private:
    float m_v;
    int m_offset{0};
};

/// Effect type index stands in for the amount added, so the value arriving at
/// the output says which nodes ran and in what order.
std::unique_ptr<Effect> makeEffect(int type)
{
    if (type < 0 || type >= 47)
        return nullptr;
    return std::make_unique<AddConst>(static_cast<float>(type));
}

} // namespace

int main()
{
    constexpr int N = 8;

    // ---- a graph survives save and load, and still carries the same signal
    {
        EffectGraph original;
        original.setMaxBlockSize(N);
        const int a = original.addNode(0, makeEffect(0), 10.0f, 20.0f);
        const int b = original.addNode(5, makeEffect(5), 30.0f, 40.0f);
        const int c = original.addNode(8, makeEffect(8), 50.0f, 60.0f);
        original.connect(kInputNodeId, a);
        original.connect(a, b);
        original.connect(a, c);          // a split, which efx_order cannot express
        original.connect(b, kOutputNodeId);
        original.connect(c, kOutputNodeId);
        original.setNodeBypassed(c, true);

        const std::string text = graphToJson(original.layout());
        check(!text.empty(), "graph serialises to non-empty text");

        GraphLayout loaded;
        std::string error;
        check(graphFromJson(text, loaded, error), "graph parses back without error");
        check(loaded == original.layout(), "layout survives the round trip unchanged");

        EffectGraph rebuilt;
        rebuilt.setMaxBlockSize(N);
        check(rebuilt.build(loaded, makeEffect), "graph rebuilds from the layout");
        check(rebuilt.layout() == original.layout(), "rebuilt graph has the same layout");

        // Positions are presentation, but they still have to round-trip.
        const GraphNodeLayout* first = nullptr;
        for (const GraphNodeLayout& n : rebuilt.layout().nodes)
            if (n.id == a) first = &n;
        check(first != nullptr && first->x == 10.0f && first->y == 20.0f,
              "canvas positions survive the round trip");

        std::vector<float> inL(N, 1.0f), inR(N, 1.0f);
        std::vector<float> o1L(N, 0.0f), o1R(N, 0.0f), o2L(N, 0.0f), o2R(N, 0.0f);
        original.process(inL.data(), inR.data(), o1L.data(), o1R.data(), N);
        rebuilt.process(inL.data(), inR.data(), o2L.data(), o2R.data(), N);
        check(o1L[0] == o2L[0] && o1R[0] == o2R[0],
              "rebuilt graph produces the same audio");
        // in -> a(+0) -> b(+5) -> out, plus in -> a -> c(bypassed) -> out
        check(o1L[0] == 7.0f, "the split and the bypassed branch both still apply");
    }

    // ---- mix modes are written by name, not by enum value
    {
        EffectGraph g;
        const int a = g.addNode(12, makeEffect(12));    // Cabinet defaults to Gain2x
        g.connect(kInputNodeId, a);
        g.connect(a, kOutputNodeId);

        const std::string text = graphToJson(g.layout());
        check(text.find("gain2x") != std::string::npos, "mix mode is written by name");
        check(text.find("\"input\"") != std::string::npos, "endpoints are written by name");

        GraphLayout loaded;
        std::string error;
        check(graphFromJson(text, loaded, error), "named mix mode parses");
        check(loaded.nodes.at(0).mix == MixMode::Gain2x, "named mix mode round trips");
    }

    // ---- a node with no explicit mix falls back to its type's default
    {
        GraphLayout loaded;
        std::string error;
        const std::string text =
            R"({"format":"rakarrack-graph","version":1,
                "nodes":[{"id":1,"type":12}],"connections":[]})";
        check(graphFromJson(text, loaded, error), "node without a mix parses");
        check(loaded.nodes.at(0).mix == defaultMixModeForType(12),
              "missing mix falls back to the type default");
    }

    // ---- bad input is refused, and refused whole
    {
        GraphLayout layout;
        layout.nodes.push_back({7, 3, false, MixMode::Replace, 0.0f, 0.0f, {}});
        const GraphLayout before = layout;
        std::string error;

        check(!graphFromJson("{ not json", layout, error), "malformed JSON is rejected");
        check(!error.empty(), "rejection explains itself");
        check(layout == before, "a rejected file leaves the layout untouched");

        error.clear();
        check(!graphFromJson(R"({"format":"something-else","nodes":[]})", layout, error),
              "a foreign format is rejected");
        error.clear();
        check(!graphFromJson(R"({"format":"rakarrack-graph","version":99,"nodes":[]})",
                             layout, error),
              "a newer format version is rejected");
        error.clear();
        check(!graphFromJson(R"({"format":"rakarrack-graph"})", layout, error),
              "a graph with no node list is rejected");
        error.clear();
        check(!graphFromJson(R"({"nodes":[{"id":1,"type":0},{"id":1,"type":1}]})",
                             layout, error),
              "duplicate node ids are rejected");
        error.clear();
        check(!graphFromJson(R"({"nodes":[{"id":-1,"type":0}]})", layout, error),
              "a node claiming a reserved id is rejected");
        error.clear();
        check(!graphFromJson(R"({"nodes":[{"id":1,"type":0}],
                                  "connections":[{"from":"nowhere","to":"output"}]})",
                             layout, error),
              "an unknown endpoint name is rejected");
        error.clear();
        check(!graphFromJson(R"({"nodes":[{"id":1,"type":0,"mix":"sideways"}]})",
                             layout, error),
              "an unknown mix mode is rejected");
        check(layout == before, "still untouched after every rejection");
    }

    // ---- building refuses layouts the evaluator would not accept
    {
        GraphLayout cyclic;
        cyclic.nodes.push_back({1, 0, false, MixMode::Replace, 0.0f, 0.0f, {}});
        cyclic.nodes.push_back({2, 0, false, MixMode::Replace, 0.0f, 0.0f, {}});
        cyclic.connections.push_back({1, 2});
        cyclic.connections.push_back({2, 1});

        EffectGraph g;
        check(!g.build(cyclic, makeEffect), "a cyclic layout is refused");
        check(g.nodes().empty(), "a refused build leaves the graph empty");

        GraphLayout unknown;
        unknown.nodes.push_back({1, 999, false, MixMode::Replace, 0.0f, 0.0f, {}});
        check(!g.build(unknown, makeEffect), "an unknown effect type is refused");
        check(g.nodes().empty(), "a refused build leaves nothing behind");
    }

    // ---- ids from a file do not collide with ids handed out afterwards
    {
        GraphLayout layout;
        layout.nodes.push_back({42, 0, false, MixMode::Replace, 0.0f, 0.0f, {}});

        EffectGraph g;
        check(g.build(layout, makeEffect), "layout with a high id builds");
        const int added = g.addNode(1, makeEffect(1));
        check(added > 42, "new nodes get ids above anything the file used");
    }

    // ---- two nodes of the same type keep their own settings
    //
    // This is the whole point of per-node settings: the legacy preset stored
    // parameters against the effect type, so a second Chorus was impossible.
    {
        EffectGraph g;
        g.setMaxBlockSize(N);
        const int first  = g.addNode(5, makeEffect(5));
        const int second = g.addNode(5, makeEffect(5));   // same type
        g.connect(kInputNodeId, first);
        g.connect(first, second);
        g.connect(second, kOutputNodeId);

        g.findNode(first)->effect->changepar(0, 11);
        g.findNode(second)->effect->changepar(0, 22);

        const std::string text = graphToJson(g.layout());
        GraphLayout loaded;
        std::string error;
        check(graphFromJson(text, loaded, error), "two same-type nodes parse");

        EffectGraph rebuilt;
        rebuilt.setMaxBlockSize(N);
        check(rebuilt.build(loaded, makeEffect), "two same-type nodes rebuild");
        check(rebuilt.findNode(first)->effect->getpar(0) == 11
              && rebuilt.findNode(second)->effect->getpar(0) == 22,
              "each node keeps its own parameter value");
    }

    std::printf("\n%d checks, %d failed\n", g_checks, g_failures);
    return g_failures == 0 ? 0 : 1;
}
