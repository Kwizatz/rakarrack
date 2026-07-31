/*
  rakarrack - guitar multi-effects processor
  SPDX-License-Identifier: GPL-2.0-only

  BankJsonTest.cpp - Round-trip checks for the JSON bank format.

  The format it replaces was a raw struct dump, so the thing worth proving is
  that nothing is lost on the way through: a bank written and read back has to
  be identical field for field, including the parts that are easy to forget --
  empty strings, negative parameters, and the MIDI tables that are omitted
  from the file when they are empty.
*/

#include "BankJson.hpp"

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

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

template <std::size_t N>
void setText(std::array<char, N>& field, const char* text)
{
    field.fill('\0');
    const std::size_t n = std::min(std::strlen(text), N - 1);
    if (n > 0)
        std::memcpy(field.data(), text, n);
}

/// Fills a preset with values chosen to be distinguishable from neighbouring
/// fields, so a serialiser that crosses two of them over gets caught.
Preset_Bank_Struct makePreset(int seed)
{
    Preset_Bank_Struct p{};

    setText(p.Preset_Name, ("preset " + std::to_string(seed)).c_str());
    setText(p.Author, "an author");
    setText(p.Classe, "a class");
    setText(p.Type, "ABC");
    setText(p.ConvoFiname, "/path/to/impulse.wav");
    setText(p.RevFiname, "/path/to/reverb.rvb");
    setText(p.EchoFiname, "/path/to/echo.dly");

    p.Input_Gain    = 0.25f + static_cast<float>(seed) * 0.01f;
    p.Master_Volume = 0.75f;
    p.Balance       = 0.5f;
    p.Bypass        = seed % 2;

    for (std::size_t r = 0; r < p.lv.size(); ++r)
        for (std::size_t c = 0; c < p.lv[r].size(); ++c)
            // Negatives matter: several effect parameters are signed.
            p.lv[r][c] = static_cast<int>(r * 20 + c) - 300 + seed;

    return p;
}

bool sameText(const char* a, const char* b, std::size_t n)
{
    return std::strncmp(a, b, n) == 0;
}

bool samePreset(const Preset_Bank_Struct& a, const Preset_Bank_Struct& b)
{
    if (!sameText(a.Preset_Name.data(), b.Preset_Name.data(), a.Preset_Name.size())) return false;
    if (!sameText(a.Author.data(), b.Author.data(), a.Author.size())) return false;
    if (!sameText(a.Classe.data(), b.Classe.data(), a.Classe.size())) return false;
    if (!sameText(a.Type.data(), b.Type.data(), a.Type.size())) return false;
    if (!sameText(a.ConvoFiname.data(), b.ConvoFiname.data(), a.ConvoFiname.size())) return false;
    if (!sameText(a.RevFiname.data(), b.RevFiname.data(), a.RevFiname.size())) return false;
    if (!sameText(a.EchoFiname.data(), b.EchoFiname.data(), a.EchoFiname.size())) return false;

    if (a.Input_Gain != b.Input_Gain) return false;
    if (a.Master_Volume != b.Master_Volume) return false;
    if (a.Balance != b.Balance) return false;
    if (a.Bypass != b.Bypass) return false;

    if (a.lv != b.lv) return false;
    if (a.XUserMIDI != b.XUserMIDI) return false;
    if (a.XMIDIrangeMin != b.XMIDIrangeMin) return false;
    if (a.XMIDIrangeMax != b.XMIDIrangeMax) return false;

    return true;
}

} // namespace

int main()
{
    // ---- a full bank survives a write/read cycle unchanged
    {
        std::vector<Preset_Bank_Struct> original(kBankPresetCount);
        for (std::size_t i = 0; i < kBankPresetCount; ++i)
            original[i] = makePreset(static_cast<int>(i));

        // One preset carries MIDI data; the rest leave it empty so both the
        // written and the omitted paths are covered in the same file.
        original[3].XUserMIDI[7][2] = 99;
        original[3].XMIDIrangeMin[11] = -5;
        original[3].XMIDIrangeMax[11] = 120;

        const std::string text = bankToJson(original.data(), kBankPresetCount);
        check(!text.empty(), "bank serialises to non-empty text");
        check(looksLikeJsonBank(text.data(), text.size()), "output is recognised as JSON");

        std::vector<Preset_Bank_Struct> loaded(kBankPresetCount);
        std::string error;
        check(bankFromJson(text, loaded.data(), kBankPresetCount, error),
              "bank parses back without error");

        bool all = true;
        for (std::size_t i = 0; i < kBankPresetCount; ++i)
            if (!samePreset(original[i], loaded[i]))
            {
                all = false;
                std::printf("   first difference at preset %zu\n", i);
                break;
            }
        check(all, "every preset survives the round trip unchanged");

        // Serialising the reloaded bank must reproduce the same text, which
        // catches anything that decodes into a different-but-equal state.
        const std::string again = bankToJson(loaded.data(), kBankPresetCount);
        check(again == text, "re-serialising produces identical text");

        // A comparator that cannot fail would make everything above vacuous.
        Preset_Bank_Struct tweaked = original[0];
        tweaked.lv[3][4] += 1;
        check(!samePreset(original[0], tweaked),
              "the comparison notices a changed parameter");

        Preset_Bank_Struct renamed = original[0];
        setText(renamed.Preset_Name, "something else");
        check(!samePreset(original[0], renamed),
              "the comparison notices a changed name");

        Preset_Bank_Struct regained = original[0];
        regained.Input_Gain += 0.125f;
        check(!samePreset(original[0], regained),
              "the comparison notices a changed gain");
    }

    // ---- the empty MIDI tables really are omitted, not written as zeros
    {
        std::vector<Preset_Bank_Struct> bank(kBankPresetCount);
        const std::string text = bankToJson(bank.data(), kBankPresetCount);
        check(text.find("midi_user") == std::string::npos,
              "all-zero MIDI tables are left out of the file");

        std::vector<Preset_Bank_Struct> loaded(kBankPresetCount);
        for (auto& p : loaded)
            p.XUserMIDI[0][0] = 1234;   // must be cleared by loading

        std::string error;
        check(bankFromJson(text, loaded.data(), kBankPresetCount, error),
              "bank with omitted MIDI parses");
        check(loaded[0].XUserMIDI[0][0] == 0,
              "an omitted MIDI table loads as zeros");
    }

    // ---- binary banks are not mistaken for JSON
    {
        const char binary[] = { '\x00', '\x01', 'r', 'a', 'k', '\x7f' };
        check(!looksLikeJsonBank(binary, sizeof(binary)),
              "a binary bank is not taken for JSON");
        const char spaced[] = "\n\t  { \"format\": \"rakarrack-bank\" }";
        check(looksLikeJsonBank(spaced, sizeof(spaced) - 1),
              "leading whitespace does not hide JSON");
        check(!looksLikeJsonBank("", 0), "empty input is not JSON");
    }

    // ---- bad input is rejected without disturbing the caller's bank
    {
        std::vector<Preset_Bank_Struct> bank(kBankPresetCount);
        bank[0].Bypass = 42;

        std::string error;
        check(!bankFromJson("{ not json", bank.data(), kBankPresetCount, error),
              "malformed JSON is rejected");
        check(!error.empty(), "rejection explains itself");
        check(bank[0].Bypass == 42, "a rejected file leaves the bank untouched");

        error.clear();
        check(!bankFromJson(R"({"format":"something-else","presets":[]})",
                            bank.data(), kBankPresetCount, error),
              "a foreign format is rejected");

        error.clear();
        check(!bankFromJson(R"({"format":"rakarrack-bank","version":99,"presets":[]})",
                            bank.data(), kBankPresetCount, error),
              "a newer format version is rejected");

        error.clear();
        check(!bankFromJson(R"({"format":"rakarrack-bank"})",
                            bank.data(), kBankPresetCount, error),
              "a bank with no preset list is rejected");
        check(bank[0].Bypass == 42, "still untouched after every rejection");
    }

    // ---- a short bank leaves the remaining slots as the caller had them
    {
        std::vector<Preset_Bank_Struct> bank(kBankPresetCount);
        for (auto& p : bank)
            setText(p.Preset_Name, "default");

        std::string error;
        const std::string text =
            R"({"format":"rakarrack-bank","version":1,"presets":[{"name":"only one"}]})";
        check(bankFromJson(text, bank.data(), kBankPresetCount, error),
              "a partial bank parses");
        check(std::strcmp(bank[0].Preset_Name.data(), "only one") == 0,
              "the preset that was present is applied");
        check(std::strcmp(bank[1].Preset_Name.data(), "default") == 0,
              "slots the file omits keep the caller's defaults");
        // Absent numbers fall back the way the binary reader's convert_IO did.
        check(bank[0].Input_Gain == 0.5f, "missing input gain falls back to 0.5");
        check(bank[0].Balance == 1.0f, "missing balance falls back to 1.0");
    }

    // ---- a preset's node patch travels with it
    {
        std::vector<Preset_Bank_Struct> bank(kBankPresetCount);
        std::vector<GraphLayout> graphs(kBankPresetCount);

        // A split and a merge: routing the sixteen-slot effect order cannot
        // describe, which is the whole reason presets carry a layout.
        GraphLayout& patch = graphs[3];
        for (int i = 0; i < 4; ++i)
        {
            GraphNodeLayout n;
            n.id   = i + 1;
            n.type = i * 3;
            n.mix  = MixMode::WetDry;
            n.x    = 10.0f * static_cast<float>(i);
            n.y    = 20.0f * static_cast<float>(i);
            n.settings.preset = i;
            n.settings.params = {i, i + 1, i + 2};
            patch.nodes.push_back(n);
        }
        patch.nodes[2].bypassed = true;
        patch.connections = {{kInputNodeId, 1}, {1, 2}, {1, 3},
                             {2, 4}, {3, 4}, {4, kOutputNodeId}};

        const std::string text =
            bankToJson(bank.data(), kBankPresetCount, graphs.data());

        std::vector<Preset_Bank_Struct> back(kBankPresetCount);
        std::vector<GraphLayout> backGraphs(kBankPresetCount);
        // Seed a patch where the file has none, to prove it gets cleared.
        backGraphs[4].nodes.push_back(GraphNodeLayout{});

        std::string error;
        check(bankFromJson(text, back.data(), kBankPresetCount, error,
                           backGraphs.data()),
              "a bank carrying a patch parses");

        const GraphLayout& r = backGraphs[3];
        check(r.nodes.size() == 4, "every node comes back");
        check(r.connections.size() == 6, "the split and merge come back");

        bool nodesMatch = (r.nodes.size() == patch.nodes.size());
        for (std::size_t i = 0; nodesMatch && i < r.nodes.size(); ++i)
            nodesMatch = r.nodes[i].id == patch.nodes[i].id
                      && r.nodes[i].type == patch.nodes[i].type
                      && r.nodes[i].mix == patch.nodes[i].mix
                      && r.nodes[i].bypassed == patch.nodes[i].bypassed
                      && r.nodes[i].settings.preset == patch.nodes[i].settings.preset
                      && r.nodes[i].settings.params == patch.nodes[i].settings.params;
        check(nodesMatch, "node ids, types, mix, bypass and settings survive");

        bool edgesMatch = (r.connections.size() == patch.connections.size());
        for (std::size_t i = 0; edgesMatch && i < r.connections.size(); ++i)
            edgesMatch = r.connections[i].from == patch.connections[i].from
                      && r.connections[i].to == patch.connections[i].to;
        check(edgesMatch, "the wiring survives, endpoints included");

        check(backGraphs[4].nodes.empty(),
              "a preset with no patch is cleared, not left holding the old one");
        check(text.find("\"graph\"") != std::string::npos,
              "the patch is written under a graph key");

        // Presets without a patch must not grow one, or every existing bank
        // would gain noise on the next save.
        const std::string plain = bankToJson(bank.data(), kBankPresetCount, nullptr);
        check(plain.find("\"graph\"") == std::string::npos,
              "a bank with no patches writes no graph keys");
    }

    // ---- one preset in a file of its own
    {
        Preset_Bank_Struct preset{};
        setText(preset.Preset_Name, "alone");
        setText(preset.Author, "somebody");
        preset.Input_Gain = 0.25f;
        preset.Master_Volume = 0.75f;
        preset.Balance = 0.5f;
        preset.lv[3][4] = -17;
        // The slot the old line-based writer dropped: it wrote index 10 twice
        // and never index 11, so this value could not survive a save.
        for (int i = 0; i < 128; ++i)
            preset.XUserMIDI[i][11] = 77;

        GraphLayout patch;
        GraphNodeLayout n;
        n.id   = 1;
        n.type = 5;
        n.mix  = MixMode::WetDry;
        n.settings.params = {1, 2, 3};
        patch.nodes.push_back(n);
        patch.connections = {{kInputNodeId, 1}, {1, kOutputNodeId}};

        const std::string text = singlePresetToJson(preset, &patch);
        check(looksLikeJsonPreset(text.data(), text.size()),
              "a written preset is recognised as JSON");

        Preset_Bank_Struct back{};
        GraphLayout backPatch;
        std::string error;
        check(singlePresetFromJson(text, back, error, &backPatch),
              "a single preset parses");
        check(std::strcmp(back.Preset_Name.data(), "alone") == 0, "the name survives");
        check(back.Input_Gain == 0.25f && back.Master_Volume == 0.75f
              && back.Balance == 0.5f, "the levels survive");
        check(back.lv[3][4] == -17, "a negative parameter survives");
        check(back.XUserMIDI[7][11] == 77,
              "MIDI slot 11 survives, which the old format dropped");
        check(backPatch.nodes.size() == 1 && backPatch.connections.size() == 2,
              "the node patch travels with the preset");

        // A bank must not be mistaken for a preset, or loading one as the
        // other would silently produce an empty preset.
        Preset_Bank_Struct bank[1]{};
        const std::string bankText = bankToJson(bank, 1, nullptr);
        Preset_Bank_Struct ignored{};
        error.clear();
        check(!singlePresetFromJson(bankText, ignored, error),
              "a bank is refused as a single preset");

        error.clear();
        check(!singlePresetFromJson("{", ignored, error),
              "a truncated preset is refused");
        check(!error.empty(), "and says why");
    }

    std::printf("\n%d checks, %d failed\n", g_checks, g_failures);
    return g_failures == 0 ? 0 : 1;
}
