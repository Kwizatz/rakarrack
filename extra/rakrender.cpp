/*
  rakarrack - guitar multi-effects processor
  SPDX-License-Identifier: GPL-2.0-only

  rakrender.cpp - Offline renderer: runs a WAV file through the effect engine
                  without JACK, and can A/B the legacy chain against the node
                  graph.

  Rendering offline makes the two signal paths directly comparable. Both are
  supposed to produce identical audio, and since they perform the same
  arithmetic in the same order that should hold bit for bit -- so any
  difference at all is a real difference, not rounding.
*/

#include "global.hpp"
#include "AllEffects.hpp"
#include "EffectRegistry.hpp"
#include "EngineController.hpp"

#include <sndfile.h>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <new>
#include <string>
#include <vector>

// ─── Heap fill ─────────────────────────────────────────────────────
//
// Fresh heap memory normally holds whatever was there before, which differs
// from one process to the next. Any effect that reads its own state before
// writing it therefore produces different audio on every run, and comparing
// two renders proves nothing.
//
// Filling every allocation with a known byte makes that leftover deterministic.
// Two runs with the same fill must then match; two runs with *different* fills
// only differ if something is reading memory it never wrote. That difference is
// the detector, and --only-effect narrows it to a single effect.
namespace {
int g_heapFill = -1;    // -1 leaves malloc's memory alone

void* fillingAlloc(std::size_t n)
{
    void* p = std::malloc(n ? n : 1);
    if (p == nullptr)
        throw std::bad_alloc();
    if (g_heapFill >= 0)
        std::memset(p, g_heapFill, n);
    return p;
}
} // namespace

// Replacing global new/delete with malloc/free is exactly the pairing GCC
// warns about here, and exactly what this needs to do.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmismatched-new-delete"

void* operator new(std::size_t n) { return fillingAlloc(n); }
void* operator new[](std::size_t n) { return fillingAlloc(n); }
void operator delete(void* p) noexcept { std::free(p); }
void operator delete[](void* p) noexcept { std::free(p); }
void operator delete(void* p, std::size_t) noexcept { std::free(p); }
void operator delete[](void* p, std::size_t) noexcept { std::free(p); }

#pragma GCC diagnostic pop


namespace {

struct Options
{
    std::string inPath;
    std::string outPath;
    std::string bankPath;
    std::string saveBankPath;
    int          preset{0};
    int          period{256};
    int          onlyEffect{-1};
    int          heapFill{-1};
    bool         graph{false};
    bool         ab{false};
    unsigned int seed{12345};
    double       tail{0.0};
};

struct Audio
{
    std::vector<float> l;
    std::vector<float> r;
    int sampleRate{0};
};

void usage()
{
    std::printf(
        "Usage:\n"
        "  rakrender --in FILE --out FILE [options]   render once\n"
        "  rakrender --in FILE --ab [options]         render both paths, compare\n"
        "  rakrender --bank FILE --save-bank FILE     rewrite a bank as JSON\n"
        "\n"
        "Options:\n"
        "  --bank FILE     bank to load (e.g. data/Default.rkrb)\n"
        "  --preset N      preset within the bank (default 0)\n"
        "  --period N      block size in frames (default 256)\n"
        "  --tail SECONDS  extra silence to render, for delay and reverb tails\n"
        "  --graph         use the node graph path instead of the legacy chain\n"
        "  --seed N        PRNG seed (default 12345)\n"
        "\n"
        "Diagnostics:\n"
        "  --only-effect N  run just effect type N, ignoring the preset's chain\n"
        "  --heap-fill N    fill every allocation with byte N, so leftover heap\n"
        "                   contents are the same on every run. Renders that\n"
        "                   differ between two fills are reading uninitialised\n"
        "                   memory.\n"
        "\n"
        "Effects randomise parameters at construction through the global PRNG,\n"
        "so --ab reseeds before each run to keep the two engines identical.\n");
}

bool readWav(const std::string& path, Audio& audio)
{
    SF_INFO info{};
    SNDFILE* file = sf_open(path.c_str(), SFM_READ, &info);
    if (file == nullptr)
    {
        std::fprintf(stderr, "cannot open %s: %s\n", path.c_str(), sf_strerror(nullptr));
        return false;
    }

    std::vector<float> interleaved(static_cast<std::size_t>(info.frames) * info.channels);
    const sf_count_t read = sf_readf_float(file, interleaved.data(), info.frames);
    sf_close(file);

    if (read != info.frames)
    {
        std::fprintf(stderr, "short read on %s\n", path.c_str());
        return false;
    }

    audio.sampleRate = info.samplerate;
    audio.l.resize(static_cast<std::size_t>(info.frames));
    audio.r.resize(static_cast<std::size_t>(info.frames));

    for (std::size_t i = 0; i < static_cast<std::size_t>(info.frames); ++i)
    {
        audio.l[i] = interleaved[i * info.channels];
        // Mono input feeds both sides, which is what a guitar input does.
        audio.r[i] = (info.channels > 1) ? interleaved[i * info.channels + 1]
                                         : audio.l[i];
    }
    return true;
}

bool writeWav(const std::string& path, const Audio& audio)
{
    SF_INFO info{};
    info.samplerate = audio.sampleRate;
    info.channels   = 2;
    info.format     = SF_FORMAT_WAV | SF_FORMAT_FLOAT;

    SNDFILE* file = sf_open(path.c_str(), SFM_WRITE, &info);
    if (file == nullptr)
    {
        std::fprintf(stderr, "cannot write %s: %s\n", path.c_str(), sf_strerror(nullptr));
        return false;
    }

    // libsndfile adds a PEAK chunk to float files, and that chunk carries the
    // wall-clock time the peaks were measured. Two renders of the same audio
    // then differ by a few bytes of header, which makes byte comparison of
    // output files useless. Nothing here needs the chunk.
    sf_command(file, SFC_SET_ADD_PEAK_CHUNK, nullptr, SF_FALSE);

    std::vector<float> interleaved(audio.l.size() * 2);
    for (std::size_t i = 0; i < audio.l.size(); ++i)
    {
        interleaved[i * 2]     = audio.l[i];
        interleaved[i * 2 + 1] = audio.r[i];
    }

    sf_writef_float(file, interleaved.data(), static_cast<sf_count_t>(audio.l.size()));
    sf_close(file);
    return true;
}

/// Print the chain actually in use, so a render can be seen to be non-trivial.
void reportChain(RKR& rkr)
{
    EngineController controller(rkr);

    std::printf("  chain:");
    bool any = false;
    for (int i = 0; i < MAX_EFFECT_SLOTS; ++i)
    {
        const int type = rkr.efx_order[i];
        if (type == EMPTY_SLOT)
            continue;

        const int* active = bypassByIndex(rkr, type);
        if (active == nullptr || *active == 0)
            continue;

        std::printf(" %s%s(%d)", any ? "-> " : "",
                    controller.getEffectTypeName(type).c_str(), type);
        any = true;
    }
    std::printf("%s\n", any ? "" : " (no effects active)");
}

/// Render `in` through a freshly constructed engine into `out`, which must
/// already be sized to the number of frames wanted.
void render(const Options& opt, const Audio& in, Audio& out, bool useGraph, bool verbose)
{
    // Reverb comb lengths, LFO seeds and formant tables are all drawn from the
    // global PRNG while effects are being built. Two engines only match if they
    // start from the same seed.
    std::srand(opt.seed);

    // RKR carries the whole preset bank by value, close to a megabyte, which
    // does not belong on a 1 MB stack.
    auto rkr = std::make_unique<RKR>(static_cast<unsigned int>(in.sampleRate),
                                     static_cast<unsigned int>(opt.period));

    if (!opt.bankPath.empty())
    {
        std::vector<char> mutablePath(opt.bankPath.begin(), opt.bankPath.end());
        mutablePath.push_back('\0');
        if (rkr->loadbank(mutablePath.data()) == 0)
            std::fprintf(stderr, "warning: could not load bank %s\n", opt.bankPath.c_str());
        else
            rkr->Bank_to_Preset(opt.preset);
    }

    // Actualizar_Audio() clears the master switch while it applies a preset.
    rkr->Bypass = 1;
    rkr->use_effect_graph = useGraph;

    if (opt.onlyEffect >= 0)
    {
        // Replace the preset's chain with a single effect, so a difference in
        // the output can be attributed to that effect alone.
        for (int i = 0; i < MAX_EFFECT_SLOTS; ++i)
            rkr->efx_order[i] = EMPTY_SLOT;
        rkr->efx_order[0] = opt.onlyEffect;

        for (int type = 0; type < kEffectTypeCount; ++type)
            if (int* active = bypassByIndex(*rkr, type))
                *active = (type == opt.onlyEffect) ? 1 : 0;
    }

    if (verbose)
        reportChain(*rkr);

    const std::size_t period = static_cast<std::size_t>(opt.period);
    std::vector<float> dryL(period, 0.0f);
    std::vector<float> dryR(period, 0.0f);

    for (std::size_t pos = 0; pos < out.l.size(); pos += period)
    {
        const std::size_t n = std::min(period, out.l.size() - pos);

        // Past the end of the input the engine keeps running on silence, which
        // is how delay and reverb tails get captured.
        for (std::size_t i = 0; i < period; ++i)
        {
            const std::size_t src = pos + i;
            const bool inRange = (src < in.l.size()) && (i < n);
            dryL[i] = inRange ? in.l[src] : 0.0f;
            dryR[i] = inRange ? in.r[src] : 0.0f;
        }

        std::memcpy(rkr->efxoutl.data(), dryL.data(), period * sizeof(float));
        std::memcpy(rkr->efxoutr.data(), dryR.data(), period * sizeof(float));

        // Matches the JACK callback: the bus carries the input in, the result
        // back out, and the untouched input is passed alongside for the
        // global dry/wet control.
        rkr->Alg(rkr->efxoutl.data(), rkr->efxoutr.data(), dryL.data(), dryR.data(), nullptr);

        std::memcpy(out.l.data() + pos, rkr->efxoutl.data(), n * sizeof(float));
        std::memcpy(out.r.data() + pos, rkr->efxoutr.data(), n * sizeof(float));
    }
}

/// Returns true when the two renders match exactly.
bool compare(const Audio& a, const Audio& b)
{
    double maxDiff = 0.0;
    std::size_t firstDiff = 0;
    bool found = false;

    for (std::size_t i = 0; i < a.l.size(); ++i)
    {
        const double dl = std::fabs(static_cast<double>(a.l[i]) - b.l[i]);
        const double dr = std::fabs(static_cast<double>(a.r[i]) - b.r[i]);
        const double d  = std::max(dl, dr);

        if (d > 0.0 && !found)
        {
            firstDiff = i;
            found = true;
        }
        maxDiff = std::max(maxDiff, d);
    }

    std::printf("\n%zu frames compared\n", a.l.size());
    if (!found)
    {
        std::printf("IDENTICAL - the two paths agree bit for bit\n");
        return true;
    }

    std::printf("DIFFERENT\n");
    std::printf("  first difference at frame %zu (%.3f s)\n",
                firstDiff, static_cast<double>(firstDiff) / a.sampleRate);
    std::printf("  maximum absolute difference %g\n", maxDiff);
    return false;
}

bool parseArgs(int argc, char** argv, Options& opt)
{
    for (int i = 1; i < argc; ++i)
    {
        const std::string arg = argv[i];
        const bool hasValue = (i + 1 < argc);

        if (arg == "--in" && hasValue)            opt.inPath   = argv[++i];
        else if (arg == "--out" && hasValue)      opt.outPath  = argv[++i];
        else if (arg == "--bank" && hasValue)     opt.bankPath = argv[++i];
        else if (arg == "--save-bank" && hasValue) opt.saveBankPath = argv[++i];
        else if (arg == "--preset" && hasValue)   opt.preset   = std::atoi(argv[++i]);
        else if (arg == "--period" && hasValue)   opt.period   = std::atoi(argv[++i]);
        else if (arg == "--only-effect" && hasValue) opt.onlyEffect = std::atoi(argv[++i]);
        else if (arg == "--heap-fill" && hasValue)   opt.heapFill   = std::atoi(argv[++i]);
        else if (arg == "--tail" && hasValue)     opt.tail     = std::atof(argv[++i]);
        else if (arg == "--seed" && hasValue)     opt.seed     = static_cast<unsigned>(std::atoi(argv[++i]));
        else if (arg == "--graph")                opt.graph    = true;
        else if (arg == "--ab")                   opt.ab       = true;
        else if (arg == "--help" || arg == "-h")  { usage(); std::exit(0); }
        else
        {
            std::fprintf(stderr, "unknown or incomplete option: %s\n", arg.c_str());
            return false;
        }
    }

    if (!opt.saveBankPath.empty())
    {
        // Conversion is a standalone mode; it renders nothing.
        if (opt.bankPath.empty())
        {
            std::fprintf(stderr, "--save-bank needs --bank to convert from\n");
            return false;
        }
        return true;
    }

    if (opt.inPath.empty())
    {
        std::fprintf(stderr, "--in is required\n");
        return false;
    }
    if (!opt.ab && opt.outPath.empty())
    {
        std::fprintf(stderr, "--out is required unless --ab is given\n");
        return false;
    }    if (opt.period <= 0)
    {
        std::fprintf(stderr, "--period must be positive\n");
        return false;
    }
    return true;
}

} // namespace

int main(int argc, char** argv)
{
    Options opt;
    if (argc < 2)
    {
        usage();
        return 1;
    }
    if (!parseArgs(argc, argv, opt))
        return 1;

    // Before anything the engine allocates.
    g_heapFill = opt.heapFill;

    if (!opt.saveBankPath.empty())
    {
        // Loading accepts either format and saving always writes JSON, so a
        // load/save pair is the conversion.
        auto rkr = std::make_unique<RKR>(44100u, static_cast<unsigned int>(opt.period));

        std::vector<char> from(opt.bankPath.begin(), opt.bankPath.end());
        from.push_back('\0');
        if (rkr->loadbank(from.data()) == 0)
        {
            std::fprintf(stderr, "could not load bank %s\n", opt.bankPath.c_str());
            return 1;
        }

        std::vector<char> to(opt.saveBankPath.begin(), opt.saveBankPath.end());
        to.push_back('\0');
        if (rkr->savebank(to.data()) == 0)
        {
            std::fprintf(stderr, "could not write bank %s\n", opt.saveBankPath.c_str());
            return 1;
        }

        std::printf("converted %s -> %s\n", opt.bankPath.c_str(), opt.saveBankPath.c_str());
        return 0;
    }

    Audio in;
    if (!readWav(opt.inPath, in))
        return 1;

    const std::size_t tailFrames =
        static_cast<std::size_t>(opt.tail * in.sampleRate);
    const std::size_t frames = in.l.size() + tailFrames;

    std::printf("input   %s (%d Hz, %zu frames)\n",
                opt.inPath.c_str(), in.sampleRate, in.l.size());
    if (!opt.bankPath.empty())
        std::printf("bank    %s, preset %d\n", opt.bankPath.c_str(), opt.preset);
    std::printf("period  %d frames, tail %.2f s, seed %u\n",
                opt.period, opt.tail, opt.seed);

    if (opt.ab)
    {
        Audio legacy;
        legacy.sampleRate = in.sampleRate;
        legacy.l.assign(frames, 0.0f);
        legacy.r.assign(frames, 0.0f);

        Audio graph;
        graph.sampleRate = in.sampleRate;
        graph.l.assign(frames, 0.0f);
        graph.r.assign(frames, 0.0f);

        std::printf("\nlegacy chain:\n");
        render(opt, in, legacy, false, true);

        std::printf("node graph:\n");
        render(opt, in, graph, true, true);

        return compare(legacy, graph) ? 0 : 1;
    }

    Audio out;
    out.sampleRate = in.sampleRate;
    out.l.assign(frames, 0.0f);
    out.r.assign(frames, 0.0f);

    std::printf("\n%s path:\n", opt.graph ? "node graph" : "legacy chain");
    render(opt, in, out, opt.graph, true);

    if (!writeWav(opt.outPath, out))
        return 1;

    std::printf("wrote   %s (%zu frames)\n", opt.outPath.c_str(), out.l.size());
    return 0;
}
