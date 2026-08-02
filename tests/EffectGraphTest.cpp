/*
  rakarrack - guitar multi-effects processor
  SPDX-License-Identifier: GPL-2.0-only

  EffectGraphTest.cpp - Routing, mix staging and legacy-equivalence checks
                        for EffectGraph.

  This links against EffectGraph.cpp alone rather than the engine library. That
  is deliberate: the graph is meant to depend on nothing but Effect, so if this
  ever stops linking without the engine, the graph has grown a dependency it
  should not have. The handful of globals below are the entire surface it
  touches, supplied here so the test needs no JACK server, no audio device and
  no preset files.
*/

#include "EffectGraph.hpp"
#include "Effect.hpp"

#include <cstdio>
#include <memory>
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

/// Adds a fixed amount to every sample, so signal flow can be traced
/// numerically: the value arriving at the output tells you exactly which nodes
/// ran, in which order, and how their results were combined.
class AddConst : public Effect
{
public:
    explicit AddConst(float v) : m_v(v) {}

    void out(float* l, float* r, int n) override
    {
        for (int i = 0; i < n; ++i)
        {
            l[i] += m_v;
            r[i] += m_v;
        }
    }

    void out(float* l, float* r) override { out(l, r, PERIOD); }

private:
    float m_v;
};

/// RKR::Vol_Efx(), transcribed. The graph is checked against this rather than
/// against hand-computed constants so the expected values stay tied to the
/// behaviour being preserved.
float legacyVolEfx(int type, float volume, float dry, float wet)
{
    float v1 = 0.0f;
    float v2 = 0.0f;
    if (volume < 0.5f)
    {
        v1 = 1.0f;
        v2 = volume * 2.0f;
    }
    else
    {
        v1 = (1.0f - volume) * 2.0f;
        v2 = 1.0f;
    }
    if (type == 8 || type == 15)
        v2 *= v2;
    return dry * v2 + wet * v1;
}

} // namespace

int main()
{
    constexpr int N = 8;
    std::vector<float> inL(N, 1.0f), inR(N, 1.0f), outL(N, 0.0f), outR(N, 0.0f);

    // ---- direct endpoint connection is a transparent stereo pass-through
    {
      EffectGraph g;
      g.setMaxBlockSize(N);
      check(g.connect(kInputNodeId, kOutputNodeId),
          "direct: connect input->output");

      inL[3] = 0.25f;
      inR[3] = -0.75f;
      g.process(inL.data(), inR.data(), outL.data(), outR.data(), N);
      check(outL == inL && outR == inR,
          "direct: stereo input is copied to output");

      g.process(inL.data(), inR.data(), inL.data(), inR.data(), N);
      check(inL[3] == 0.25f && inR[3] == -0.75f,
          "direct: in-place pass-through preserves both channels");
    }

    // ---- series: in -> (+10) -> (+100) -> out  =>  1 + 10 + 100 = 111
    {
        EffectGraph g;
        g.setMaxBlockSize(N);
        const int a = g.addNode(0, std::make_unique<AddConst>(10.0f));
        const int b = g.addNode(0, std::make_unique<AddConst>(100.0f));
        check(g.connect(kInputNodeId, a), "series: connect input->a");
        check(g.connect(a, b), "series: connect a->b");
        check(g.connect(b, kOutputNodeId), "series: connect b->output");
        g.process(inL.data(), inR.data(), outL.data(), outR.data(), N);
        check(outL[0] == 111.0f, "series: signal passes through both in order");
    }

    // ---- parallel: in -> (+10) -> out and in -> (+100) -> out  =>  11 + 101 = 112
    {
        EffectGraph g;
        g.setMaxBlockSize(N);
        const int a = g.addNode(0, std::make_unique<AddConst>(10.0f));
        const int b = g.addNode(0, std::make_unique<AddConst>(100.0f));
        g.connect(kInputNodeId, a);
        g.connect(kInputNodeId, b);
        g.connect(a, kOutputNodeId);
        g.connect(b, kOutputNodeId);
        g.process(inL.data(), inR.data(), outL.data(), outR.data(), N);
        check(outL[0] == 112.0f, "parallel: split then merge sums both branches");
    }

    // ---- merge into one node: (in + in) -> (+10) => 2 + 10 = 12
    {
        EffectGraph g;
        g.setMaxBlockSize(N);
        const int a = g.addNode(0, std::make_unique<AddConst>(0.0f));
        const int b = g.addNode(0, std::make_unique<AddConst>(0.0f));
        const int m = g.addNode(0, std::make_unique<AddConst>(10.0f));
        g.connect(kInputNodeId, a);
        g.connect(kInputNodeId, b);
        g.connect(a, m);
        g.connect(b, m);
        g.connect(m, kOutputNodeId);
        g.process(inL.data(), inR.data(), outL.data(), outR.data(), N);
        check(outL[0] == 12.0f, "merge: two edges into one node are summed");
    }

    // ---- ordering is independent of insertion order
    {
        EffectGraph g;
        g.setMaxBlockSize(N);
        // Create the *second* stage first, so declaration order != signal order.
        const int second = g.addNode(0, std::make_unique<AddConst>(100.0f));
        const int first  = g.addNode(0, std::make_unique<AddConst>(10.0f));
        g.connect(kInputNodeId, first);
        g.connect(first, second);
        g.connect(second, kOutputNodeId);
        g.process(inL.data(), inR.data(), outL.data(), outR.data(), N);
        check(outL[0] == 111.0f, "topo sort: evaluation follows edges, not insertion");
    }

    // ---- cycles rejected
    {
        EffectGraph g;
        g.setMaxBlockSize(N);
        const int a = g.addNode(0, std::make_unique<AddConst>(1.0f));
        const int b = g.addNode(0, std::make_unique<AddConst>(1.0f));
        g.connect(a, b);
        check(!g.connect(b, a), "cycle: back edge is rejected");
        check(!g.connect(a, a), "cycle: self connection is rejected");
        check(g.wouldCreateCycle(b, a), "cycle: wouldCreateCycle agrees");
    }

    // ---- endpoint direction enforced
    {
        EffectGraph g;
        g.setMaxBlockSize(N);
        const int a = g.addNode(0, std::make_unique<AddConst>(1.0f));
        check(!g.connect(a, kInputNodeId), "endpoints: nothing may feed the input");
        check(!g.connect(kOutputNodeId, a), "endpoints: nothing may leave the output");
    }

    // ---- removing a node drops its edges
    {
        EffectGraph g;
        g.setMaxBlockSize(N);
        const int a = g.addNode(0, std::make_unique<AddConst>(10.0f));
        g.connect(kInputNodeId, a);
        g.connect(a, kOutputNodeId);
        check(g.connections().size() == 2, "remove: two edges before removal");
        check(g.removeNode(a), "remove: node removed");
        check(g.connections().empty(), "remove: dangling edges cleaned up");
        g.process(inL.data(), inR.data(), outL.data(), outR.data(), N);
        check(outL[0] == 0.0f, "remove: empty graph outputs silence");
    }

    // ---- bypass passes signal through untouched
    {
        EffectGraph g;
        g.setMaxBlockSize(N);
        const int a = g.addNode(0, std::make_unique<AddConst>(10.0f));
        g.connect(kInputNodeId, a);
        g.connect(a, kOutputNodeId);
        g.setNodeBypassed(a, true);
        g.process(inL.data(), inR.data(), outL.data(), outR.data(), N);
        check(outL[0] == 1.0f, "bypass: node passes its input through unchanged");
    }

    // ---- legacy series chain helper reproduces the old rack order
    {
        EffectGraph g;
        g.setMaxBlockSize(N);
        std::vector<std::unique_ptr<Effect>> fx;
        fx.push_back(std::make_unique<AddConst>(10.0f));
        fx.push_back(std::make_unique<AddConst>(100.0f));
        g.buildSeriesChain({0, 1}, std::move(fx));
        g.process(inL.data(), inR.data(), outL.data(), outR.data(), N);
        check(outL[0] == 111.0f, "buildSeriesChain: legacy order reproduced");
        check(g.isFullyConnected(), "buildSeriesChain: all nodes connected");
    }

    // ---- mix modes reproduce the legacy Vol_Efx / Vol2_Efx / Vol3_Efx staging
    {
        check(defaultMixModeForType(0) == MixMode::Replace, "mix: type 0 is Replace");
        check(defaultMixModeForType(12) == MixMode::Gain2x, "mix: type 12 is Gain2x");
        check(defaultMixModeForType(5) == MixMode::WetDry, "mix: type 5 is WetDry");
        check(defaultMixModeForType(44) == MixMode::Replace, "mix: type 44 is Replace");

        // Gain2x doubles.
        {
            EffectGraph g;
            g.setMaxBlockSize(N);
            const int a = g.addNode(12, std::make_unique<AddConst>(0.0f));
            g.connect(kInputNodeId, a);
            g.connect(a, kOutputNodeId);
            g.process(inL.data(), inR.data(), outL.data(), outR.data(), N);
            check(outL[0] == 2.0f, "mix: Gain2x doubles the node output");
        }

        // WetDry crossfade matches the legacy formula, including the
        // dry-coefficient squaring on Reverb (8) and MusicDelay (15).
        for (int type : {5, 8, 15})
        {
            for (float vol : {0.0f, 0.25f, 0.5f, 0.75f, 1.0f})
            {
                EffectGraph g;
                g.setMaxBlockSize(N);
                auto fx = std::make_unique<AddConst>(10.0f);
                fx->outvolume = vol;
                const int a = g.addNode(type, std::move(fx));
                g.connect(kInputNodeId, a);
                g.connect(a, kOutputNodeId);
                g.process(inL.data(), inR.data(), outL.data(), outR.data(), N);

                const float expected = legacyVolEfx(type, vol, 1.0f, 11.0f);
                char label[96];
                std::snprintf(label, sizeof(label),
                              "mix: WetDry type %d vol %.2f matches Vol_Efx", type, vol);
                check(outL[0] == expected, label);
            }
        }
    }

    // ---- a series chain matches a direct simulation of the legacy Alg() loop
    {
        struct Step { int type; float add; float outvolume; bool active; };
        const std::vector<Step> steps = {
            {  0, 3.0f, 0.30f, true  },  // Replace
            {  5, 7.0f, 0.30f, true  },  // WetDry, volume < 0.5
            { 12, 1.0f, 0.90f, true  },  // Gain2x
            {  8, 5.0f, 0.75f, true  },  // WetDry, reverb squares the dry coeff
            { 21, 2.0f, 0.40f, false },  // inactive: must be skipped entirely
            { 15, 4.0f, 0.60f, true  },  // WetDry, musicdelay squares it too
        };

        // Legacy: one shared bus, with smp holding the previous stage's output.
        // Control_Gain() seeds smp with the chain input before the loop starts,
        // and Vol2_Efx() refreshes it after every active effect.
        float efxout = 1.0f;
        float smp    = efxout;
        for (const Step& s : steps)
        {
            if (!s.active) continue;
            efxout += s.add;
            switch (defaultMixModeForType(s.type))
            {
            case MixMode::Replace:
                break;
            case MixMode::Gain2x:
                efxout *= 2.0f;
                break;
            case MixMode::WetDry:
                efxout = legacyVolEfx(s.type, s.outvolume, smp, efxout);
                break;
            }
            smp = efxout;
        }

        // Graph: same chain, with the inactive step present but bypassed.
        EffectGraph g;
        g.setMaxBlockSize(N);
        int prev = kInputNodeId;
        for (const Step& s : steps)
        {
            auto fx = std::make_unique<AddConst>(s.add);
            fx->outvolume = s.outvolume;
            const int id = g.addNode(s.type, std::move(fx));
            g.setNodeBypassed(id, !s.active);
            g.connect(prev, id);
            prev = id;
        }
        g.connect(prev, kOutputNodeId);
        g.process(inL.data(), inR.data(), outL.data(), outR.data(), N);

        check(outL[0] == efxout, "legacy equivalence: matches simulated Alg() loop");
    }

    // ---- borrowed nodes run the caller's effect and leave ownership alone
    {
        AddConst owned(10.0f);
        EffectGraph g;
        g.setMaxBlockSize(N);
        const int a = g.addBorrowedNode(0, &owned);
        g.connect(kInputNodeId, a);
        g.connect(a, kOutputNodeId);
        g.process(inL.data(), inR.data(), outL.data(), outR.data(), N);
        check(outL[0] == 11.0f, "borrowed: graph runs an effect it does not own");
    }

    // ---- processing in place is safe, which is how Alg() calls it
    {
        EffectGraph g;
        g.setMaxBlockSize(N);
        const int a = g.addNode(0, std::make_unique<AddConst>(10.0f));
        const int b = g.addNode(0, std::make_unique<AddConst>(100.0f));
        g.connect(kInputNodeId, a);
        g.connect(a, b);
        g.connect(b, kOutputNodeId);

        std::vector<float> busL(N, 1.0f), busR(N, 1.0f);
        g.process(busL.data(), busR.data(), busL.data(), busR.data(), N);
        check(busL[0] == 111.0f, "in place: input and output may be the same buffer");
    }

    // ---- layoutCanConnect applies the same rules without any effects
    //
    // The node editor asks this before allowing a wire, so it has to agree
    // with what connect() would accept. Two sets of rules that could drift
    // apart would mean an editor that draws a patch the evaluator rejects.
    {
        GraphLayout layout;
        layout.nodes.push_back({1, 0, false, MixMode::Replace, 0.0f, 0.0f, {}});
        layout.nodes.push_back({2, 0, false, MixMode::Replace, 0.0f, 0.0f, {}});
        layout.nodes.push_back({3, 0, false, MixMode::Replace, 0.0f, 0.0f, {}});
        layout.connections.push_back({kInputNodeId, 1});
        layout.connections.push_back({1, 2});

        check(layoutCanConnect(layout, 2, 3), "layout: a new forward edge is allowed");
        check(layoutCanConnect(layout, 1, 3), "layout: a split is allowed");
        check(layoutCanConnect(layout, 2, kOutputNodeId), "layout: reaching the output is allowed");

        check(!layoutCanConnect(layout, 1, 2), "layout: a duplicate edge is refused");
        check(!layoutCanConnect(layout, 2, 1), "layout: a back edge is refused");
        check(!layoutCanConnect(layout, 1, 1), "layout: a self connection is refused");
        check(!layoutCanConnect(layout, 1, kInputNodeId), "layout: nothing may feed the input");
        check(!layoutCanConnect(layout, kOutputNodeId, 1), "layout: nothing may leave the output");
        check(!layoutCanConnect(layout, 1, 99), "layout: an unknown node is refused");

        // A longer cycle, which a naive one-step check would miss.
        layout.connections.push_back({2, 3});
        check(!layoutCanConnect(layout, 3, 1), "layout: an indirect cycle is refused");

        // And it must agree with the graph itself.
        EffectGraph g;
        const int a = g.addNode(0, std::make_unique<AddConst>(0.0f));
        const int b = g.addNode(0, std::make_unique<AddConst>(0.0f));
        g.connect(a, b);
        const GraphLayout mirrored = g.layout();
        check(layoutCanConnect(mirrored, b, a) == g.connect(b, a),
              "layout: agrees with EffectGraph::connect on a back edge");
    }

    std::printf("\n%d checks, %d failed\n", g_checks, g_failures);
    return g_failures == 0 ? 0 : 1;
}
