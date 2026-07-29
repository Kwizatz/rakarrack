/*
  rakarrack - guitar multi-effects processor
  SPDX-License-Identifier: GPL-2.0-only

  EffectParamsTest.cpp - Does a captured parameter set really reproduce an
                         effect?

  Capturing settings by sweeping getpar() rests on an assumption: that
  getpar() and changepar() cover the same indices in every effect. If some
  effect accepts a parameter it will not report, that setting is silently lost
  on save, and a preset would come back subtly wrong.

  Rather than read 47 pairs of switch statements and hope, this configures each
  effect, copies its settings to a second instance through capture/apply, and
  checks the second instance both reports the same parameters and produces the
  same audio.

  Unlike the other tests this one links the engine, because it needs the real
  effects. It still needs no JACK server and no audio device.
*/

#include "EffectFactory.hpp"
#include "EffectParams.hpp"
#include "Effect.hpp"
#include "dsp_constants.hpp"
#include "EngineController.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <new>
#include <string>
#include <vector>

// Fresh heap memory holds whatever was there before, and the two instances
// compared here are allocated at different addresses. Any effect that still
// reads its own state before writing it would make this test report parameter
// loss that is not there -- and do so intermittently. Filling every allocation
// with a fixed byte removes that variable, so a difference here means a
// difference in the parameters.
namespace {
void* pinnedAlloc(std::size_t n)
{
    void* p = std::malloc(n ? n : 1);
    if (p == nullptr)
        throw std::bad_alloc();
    std::memset(p, 0, n);
    return p;
}
} // namespace

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmismatched-new-delete"
void* operator new(std::size_t n) { return pinnedAlloc(n); }
void* operator new[](std::size_t n) { return pinnedAlloc(n); }
void operator delete(void* p) noexcept { std::free(p); }
void operator delete[](void* p) noexcept { std::free(p); }
void operator delete(void* p, std::size_t) noexcept { std::free(p); }
void operator delete[](void* p, std::size_t) noexcept { std::free(p); }
#pragma GCC diagnostic pop

namespace {

int g_checks = 0;
int g_failures = 0;

void check(bool cond, const char* what)
{
    ++g_checks;
    if (!cond)
    {
        std::printf("%-58s FAILED\n", what);
        ++g_failures;
    }
}

constexpr int kBlock = 128;
constexpr unsigned kSeed = 4242;

std::vector<float> testSignal()
{
    std::vector<float> v(kBlock);
    for (int i = 0; i < kBlock; ++i)
        v[static_cast<std::size_t>(i)] =
            0.4f * std::sin(0.07f * static_cast<float>(i))
          + 0.2f * std::sin(0.31f * static_cast<float>(i));
    return v;
}

EffectFactoryConfig makeConfig(float* aux)
{
    EffectFactoryConfig cfg;
    cfg.auxResampled = aux;
    return cfg;
}

/// Runs one block and returns the output, so two instances can be compared.
void renderBlock(Effect& fx, std::vector<float>& l, std::vector<float>& r)
{
    const std::vector<float> sig = testSignal();
    l = sig;
    r = sig;
    fx.setMaxBlockSize(kBlock);
    fx.out(l.data(), r.data(), kBlock);
}

} // namespace

int main()
{
    // The engine defines these; nothing has set them without RKR running.
    SAMPLE_RATE  = 44100;
    fSAMPLE_RATE = 44100.0f;
    cSAMPLE_RATE = 1.0f / 44100.0f;
    PERIOD       = kBlock;
    fPERIOD      = static_cast<float>(kBlock);

    std::vector<float> aux(kBlock, 0.0f);
    const EffectFactoryConfig cfg = makeConfig(aux.data());

    int paramMismatch = 0;
    int audioMismatch = 0;
    std::string paramNames;
    std::string audioNames;

    for (int type = 0; type < kEffectTypeCount; ++type)
    {
        for (int preset = 0; preset < 3; ++preset)
        {
            // Several effects draw from the global PRNG while being built or
            // configured -- reverb comb lengths, LFO seeds -- so both
            // instances have to start from the same seed to be comparable.
            std::srand(kSeed);
            auto source = createEffect(type, cfg);
            if (!source)
                continue;
            source->setpreset(preset);
            // applyEffectSettings() leaves the effect settled, and several
            // setpreset() implementations do not. Settle both, or this
            // measures that difference rather than the settings.
            source->cleanup();

            const EffectSettings settings = captureEffectSettings(*source);

            std::srand(kSeed);
            auto copy = createEffect(type, cfg);
            applyEffectSettings(*copy, settings);

            // Every setting the source had must survive the trip.
            const EffectSettings readback = captureEffectSettings(*copy);
            if (readback != settings)
            {
                ++paramMismatch;
                paramNames += " " + std::to_string(type);

                // Naming the slot turns "some effect is wrong" into something
                // that can be looked up directly in that effect's changepar().
                if (readback.preset != settings.preset)
                {
                    std::printf("    type %d: preset was %d, came back %d\n",
                                type, settings.preset, readback.preset);
                }
                const std::size_t n = std::max(settings.params.size(), readback.params.size());
                for (std::size_t i = 0; i < n; ++i)
                {
                    const int want = (i < settings.params.size()) ? settings.params[i] : 0;
                    const int got  = (i < readback.params.size()) ? readback.params[i] : 0;
                    if (want != got)
                    {
                        std::printf("    type %d preset %d: slot %zu was %d, came back %d\n",
                                    type, preset, i, want, got);
                        break;
                    }
                }
                break;
            }

            // And the copy must actually sound like the source, which catches
            // anything changepar() applies but getpar() does not report.
            std::vector<float> sl, sr, cl, cr;
            renderBlock(*source, sl, sr);
            renderBlock(*copy, cl, cr);
            if (sl != cl || sr != cr)
            {
                ++audioMismatch;
                audioNames += " " + std::to_string(type);

                double worst = 0.0;
                for (std::size_t i = 0; i < sl.size(); ++i)
                    worst = std::max(worst,
                                     std::fabs(static_cast<double>(sl[i]) - cl[i]));

                std::printf("    type %d preset %d: worst sample difference %g\n",
                            type, preset, worst);
                break;
            }
        }
    }

    if (!paramNames.empty())
        std::printf("  parameter mismatch in types:%s\n", paramNames.c_str());
    if (!audioNames.empty())
        std::printf("  audio mismatch in types:%s\n", audioNames.c_str());

    check(paramMismatch == 0, "every effect's parameters survive capture/apply");
    check(audioMismatch == 0, "every effect sounds the same after capture/apply");

    // ---- restoring twice must be the same as restoring once
    //
    // applyEffectSettings() replays the preset and then the parameters, so a
    // changepar() that is not idempotent drifts a little further each time.
    // That is what separates "the settings are wrong" from "applying them
    // twice is not the same as applying them once".
    {
        int notIdempotent = 0;
        std::string names;

        for (int type = 0; type < kEffectTypeCount; ++type)
        {
            std::srand(kSeed);
            auto probe = createEffect(type, cfg);
            if (!probe)
                continue;
            probe->setpreset(1);
            const EffectSettings settings = captureEffectSettings(*probe);

            std::srand(kSeed);
            auto once = createEffect(type, cfg);
            applyEffectSettings(*once, settings);

            std::srand(kSeed);
            auto twice = createEffect(type, cfg);
            applyEffectSettings(*twice, settings);
            applyEffectSettings(*twice, settings);

            std::vector<float> al, ar, bl, br;
            renderBlock(*once, al, ar);
            renderBlock(*twice, bl, br);
            if (al != bl || ar != br)
            {
                ++notIdempotent;
                names += " " + std::to_string(type);
            }
        }

        if (!names.empty())
            std::printf("  applying settings twice differs in types:%s\n", names.c_str());
        check(notIdempotent == 0, "restoring settings twice matches restoring once");
    }

    // ---- the sweep has to be wide enough for the widest effect
    {
        // EQ addresses bands up to 10 + 5*MAX_EQ_BANDS - 1.
        check(kEffectParamSlots > 10 + 5 * MAX_EQ_BANDS - 1,
              "parameter sweep covers EQ's highest band index");
    }

    // ---- a zero-valued setting must not be silently skipped
    {
        std::srand(kSeed);
        auto fx = createEffect(4, cfg);      // Echo
        check(fx != nullptr, "test effect is created");

        // Drive a parameter to zero, which is exactly the case a sparse
        // "only save what is set" scheme would lose.
        fx->changepar(0, 0);
        const EffectSettings settings = captureEffectSettings(*fx);

        std::srand(kSeed);
        auto other = createEffect(4, cfg);
        other->changepar(0, 100);            // start from something different
        applyEffectSettings(*other, settings);
        check(other->getpar(0) == 0, "a parameter set to zero is restored as zero");
    }

    // ---- trailing zeros are trimmed on capture but restored on apply
    {
        std::srand(kSeed);
        auto fx = createEffect(4, cfg);      // Echo uses indices 0..8
        const EffectSettings settings = captureEffectSettings(*fx);
        check(settings.params.size() < static_cast<std::size_t>(kEffectParamSlots),
              "unused trailing slots are trimmed");

        std::srand(kSeed);
        auto other = createEffect(4, cfg);
        applyEffectSettings(*other, settings);
        check(captureEffectSettings(*other) == settings,
              "a trimmed capture still restores exactly");
    }

    std::printf("\n%d checks, %d failed\n", g_checks, g_failures);
    return g_failures == 0 ? 0 : 1;
}
