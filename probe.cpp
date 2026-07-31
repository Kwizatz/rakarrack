// Throwaway: FX off is silent, FX on is noisy, and it scales with input gain.
// So the chain is amplifying whatever noise floor arrives. By how much?
#include "global.hpp"

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <string>
#include <vector>

static std::vector<char> path(const std::string& s)
{
    std::vector<char> v(s.begin(), s.end());
    v.push_back('\0');
    return v;
}

/// Feed a steady low-level noise and report the output level in dB.
static double run(int preset, double inputDb, int inputGain)
{
    srand(12345);
    auto rkr = std::make_unique<RKR>(44100u, 256u);
    auto bank = path("data/Default.rkrb");
    rkr->loadbank(bank.data());
    rkr->Bank_to_Preset(preset);
    rkr->Bypass = 1;
    rkr->booster = 1.0f;
    rkr->Input_Gain = static_cast<float>(inputGain) / 127.0f;
    rkr->calculavol(1);

    const float amp = static_cast<float>(std::pow(10.0, inputDb / 20.0));
    std::vector<float> dryL(256), dryR(256);

    double peak = 0.0;
    for (int b = 0; b < 400; ++b)
    {
        for (int i = 0; i < 256; ++i)
        {
            const float n = amp * (2.0f * (static_cast<float>(rand()) / RAND_MAX) - 1.0f);
            dryL[i] = n;
            dryR[i] = n;
        }
        std::memcpy(rkr->efxoutl.data(), dryL.data(), sizeof(float) * 256);
        std::memcpy(rkr->efxoutr.data(), dryR.data(), sizeof(float) * 256);
        rkr->Alg(rkr->efxoutl.data(), rkr->efxoutr.data(), dryL.data(), dryR.data(), nullptr);

        if (b > 100)                       // let it settle
            for (int i = 0; i < 256; ++i)
                if (std::isfinite(rkr->efxoutl[i]))
                    peak = std::max(peak, std::abs(static_cast<double>(rkr->efxoutl[i])));
    }
    return peak > 0 ? 20.0 * std::log10(peak) : -999.0;
}

int main()
{
    const double floorDb = -70.0;   // a typical consumer line-in noise floor
    std::printf("a %.0f dB noise floor through Default preset 0\n\n", floorDb);
    std::printf("%14s %12s %12s\n", "input gain", "output", "vs FX off");
    for (int g : {0, 16, 32, 48, 64, 80, 96, 112, 127})
    {
        const double out = run(0, floorDb, g);
        std::printf("%14d %9.1f dB %9.1f dB\n", g, out, out - floorDb);
    }
    std::printf("\nFX off passes the input at unity, so it sits at %.0f dB.\n", floorDb);
    return 0;
}
