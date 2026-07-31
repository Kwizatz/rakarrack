/*
  rakarrack - guitar multi-effects processor
  SPDX-License-Identifier: GPL-2.0-only

  BankJson.cpp - Reading and writing preset banks as JSON.
*/

#include "BankJson.hpp"
#include "GraphJson.hpp"

#include <nlohmann/json.hpp>

#include <algorithm>
#include <cstring>
#include <vector>

using nlohmann::json;

namespace {

/// Schema version, so a future layout change can be detected rather than
/// guessed at from the file size the way the binary format had to.
constexpr int kBankFormatVersion = 1;
constexpr const char* kBankFormatName = "rakarrack-bank";
constexpr const char* kPresetFormatName = "rakarrack-preset";

/// The fixed-size char arrays are C strings padded with NULs; JSON carries
/// just the text.
template <std::size_t N>
std::string toText(const std::array<char, N>& field)
{
    return std::string(field.data(), ::strnlen(field.data(), N));
}

template <std::size_t N>
void fromText(std::array<char, N>& field, const std::string& text)
{
    field.fill('\0');
    const std::size_t n = std::min(text.size(), N - 1);
    if (n > 0)
        std::memcpy(field.data(), text.data(), n);
}

/// Reads a string field, tolerating absence so hand-edited files still load.
template <std::size_t N>
void readText(const json& j, const char* key, std::array<char, N>& field)
{
    field.fill('\0');
    if (auto it = j.find(key); it != j.end() && it->is_string())
        fromText(field, it->get<std::string>());
}

template <typename T>
T readScalar(const json& j, const char* key, T fallback)
{
    if (auto it = j.find(key); it != j.end() && it->is_number())
        return it->get<T>();
    return fallback;
}

template <std::size_t Rows, std::size_t Cols>
bool allZero(const std::array<std::array<int, Cols>, Rows>& m)
{
    for (const auto& row : m)
        for (int v : row)
            if (v != 0)
                return false;
    return true;
}

template <std::size_t N>
bool allZero(const std::array<int, N>& v)
{
    return std::all_of(v.begin(), v.end(), [](int x) { return x == 0; });
}

template <std::size_t Rows, std::size_t Cols>
json toJson(const std::array<std::array<int, Cols>, Rows>& m)
{
    json out = json::array();
    for (const auto& row : m)
        out.push_back(json(row));
    return out;
}

/// Missing or short rows keep their zero fill, so an absent key means "all
/// zero" and files stay small.
template <std::size_t Rows, std::size_t Cols>
void fromJson(const json& j, const char* key, std::array<std::array<int, Cols>, Rows>& m)
{
    for (auto& row : m)
        row.fill(0);

    auto it = j.find(key);
    if (it == j.end() || !it->is_array())
        return;

    const std::size_t rows = std::min<std::size_t>(it->size(), Rows);
    for (std::size_t r = 0; r < rows; ++r)
    {
        const json& row = (*it)[r];
        if (!row.is_array())
            continue;
        const std::size_t cols = std::min<std::size_t>(row.size(), Cols);
        for (std::size_t c = 0; c < cols; ++c)
            if (row[c].is_number())
                m[r][c] = row[c].get<int>();
    }
}

template <std::size_t N>
void fromJson(const json& j, const char* key, std::array<int, N>& v)
{
    v.fill(0);

    auto it = j.find(key);
    if (it == j.end() || !it->is_array())
        return;

    const std::size_t n = std::min<std::size_t>(it->size(), N);
    for (std::size_t i = 0; i < n; ++i)
        if ((*it)[i].is_number())
            v[i] = (*it)[i].get<int>();
}

json presetToJson(const Preset_Bank_Struct& p)
{
    json j;
    j["name"]   = toText(p.Preset_Name);
    j["author"] = toText(p.Author);
    j["class"]  = toText(p.Classe);
    j["type"]   = toText(p.Type);

    // Stored as numbers rather than the printed strings the binary format
    // needed; cInput_Gain and friends exist only to serve that format.
    j["input_gain"]    = p.Input_Gain;
    j["master_volume"] = p.Master_Volume;
    j["balance"]       = p.Balance;
    j["bypass"]        = p.Bypass;

    const std::string convo = toText(p.ConvoFiname);
    const std::string rev   = toText(p.RevFiname);
    const std::string echo  = toText(p.EchoFiname);
    if (!convo.empty()) j["convolution_file"] = convo;
    if (!rev.empty())   j["reverb_file"]      = rev;
    if (!echo.empty())  j["echo_file"]        = echo;

    j["parameters"] = toJson(p.lv);

    // Usually untouched, and 128x20 of zeros per preset dwarfs everything
    // else in the file, so it is only written when it holds something.
    if (!allZero(p.XUserMIDI))     j["midi_user"]      = toJson(p.XUserMIDI);
    if (!allZero(p.XMIDIrangeMin)) j["midi_range_min"] = json(p.XMIDIrangeMin);
    if (!allZero(p.XMIDIrangeMax)) j["midi_range_max"] = json(p.XMIDIrangeMax);

    return j;
}

Preset_Bank_Struct presetFromJson(const json& j)
{
    Preset_Bank_Struct p{};

    readText(j, "name",   p.Preset_Name);
    readText(j, "author", p.Author);
    readText(j, "class",  p.Classe);
    readText(j, "type",   p.Type);

    readText(j, "convolution_file", p.ConvoFiname);
    readText(j, "reverb_file",      p.RevFiname);
    readText(j, "echo_file",        p.EchoFiname);

    // Same fallbacks the binary reader applies in convert_IO(): a zero here
    // means the value was never set, not that it was set to silence.
    p.Input_Gain    = readScalar<float>(j, "input_gain", 0.5f);
    p.Master_Volume = readScalar<float>(j, "master_volume", 0.5f);
    p.Balance       = readScalar<float>(j, "balance", 1.0f);
    if (p.Input_Gain == 0.0f)    p.Input_Gain = 0.5f;
    if (p.Master_Volume == 0.0f) p.Master_Volume = 0.5f;
    if (p.Balance == 0.0f)       p.Balance = 1.0f;

    p.Bypass = readScalar<int>(j, "bypass", 0);

    fromJson(j, "parameters",     p.lv);
    fromJson(j, "midi_user",      p.XUserMIDI);
    fromJson(j, "midi_range_min", p.XMIDIrangeMin);
    fromJson(j, "midi_range_max", p.XMIDIrangeMax);

    return p;
}

} // namespace

std::string bankToJson(const Preset_Bank_Struct* bank, std::size_t count,
                       const GraphLayout* graphs)
{
    json j;
    j["format"]  = kBankFormatName;
    j["version"] = kBankFormatVersion;

    json presets = json::array();
    for (std::size_t i = 0; i < count; ++i)
    {
        json preset = presetToJson(bank[i]);

        // Only when there is one. A preset whose chain the effect order can
        // already describe is written exactly as it was before, so this does
        // not churn every existing bank.
        if (graphs != nullptr && !graphs[i].nodes.empty())
            preset["graph"] = layoutToJson(graphs[i]);

        presets.push_back(std::move(preset));
    }
    j["presets"] = std::move(presets);

    return j.dump(2);
}

bool bankFromJson(const std::string& text,
                  Preset_Bank_Struct* bank,
                  std::size_t count,
                  std::string& error,
                  GraphLayout* graphs)
{
    json j;
    try
    {
        j = json::parse(text);
    }
    catch (const json::parse_error& e)
    {
        error = std::string("not valid JSON: ") + e.what();
        return false;
    }

    if (auto it = j.find("format"); it != j.end() && it->is_string()
        && it->get<std::string>() != kBankFormatName)
    {
        error = "not a rakarrack bank: format is \"" + it->get<std::string>() + "\"";
        return false;
    }

    if (auto it = j.find("version"); it != j.end() && it->is_number_integer()
        && it->get<int>() > kBankFormatVersion)
    {
        error = "bank was written by a newer version of rakarrack";
        return false;
    }

    auto presets = j.find("presets");
    if (presets == j.end() || !presets->is_array())
    {
        error = "bank has no preset list";
        return false;
    }

    // Built aside and only committed once parsing has succeeded, so a bad
    // file cannot leave half of the previous bank in place. Slots the file
    // does not mention keep whatever the caller put there, which lets the
    // caller seed defaults first.
    const std::size_t n = std::min<std::size_t>(presets->size(), count);
    std::vector<Preset_Bank_Struct> parsed(n);
    std::vector<GraphLayout> parsedGraphs(n);
    for (std::size_t i = 0; i < n; ++i)
    {
        parsed[i] = presetFromJson((*presets)[i]);

        if (auto it = (*presets)[i].find("graph");
            it != (*presets)[i].end() && it->is_object())
        {
            std::string graphError;
            if (!layoutFromJson(*it, parsedGraphs[i], graphError))
            {
                error = "preset " + std::to_string(i) + ": " + graphError;
                return false;
            }
        }
    }

    std::copy(parsed.begin(), parsed.end(), bank);
    if (graphs != nullptr)
    {
        // Every slot, not just the ones with a layout: a preset without one
        // must not inherit the patch left behind by the previous bank.
        for (std::size_t i = 0; i < count; ++i)
            graphs[i] = (i < n) ? std::move(parsedGraphs[i]) : GraphLayout{};
    }
    return true;
}

bool looksLikeJsonBank(const char* data, std::size_t len)
{
    for (std::size_t i = 0; i < len; ++i)
    {
        const unsigned char c = static_cast<unsigned char>(data[i]);
        if (c == ' ' || c == '\t' || c == '\r' || c == '\n')
            continue;
        return c == '{';
    }
    return false;
}

// ─── Single presets ──────────────────────────────────────────────────

std::string singlePresetToJson(const Preset_Bank_Struct& preset,
                               const GraphLayout* graph)
{
    json j = presetToJson(preset);
    j["format"]  = kPresetFormatName;
    j["version"] = kBankFormatVersion;

    if (graph != nullptr && !graph->nodes.empty())
        j["graph"] = layoutToJson(*graph);

    return j.dump(2);
}

bool singlePresetFromJson(const std::string& text,
                          Preset_Bank_Struct& preset,
                          std::string& error,
                          GraphLayout* graph)
{
    json j;
    try
    {
        j = json::parse(text);
    }
    catch (const json::parse_error& e)
    {
        error = std::string("not valid JSON: ") + e.what();
        return false;
    }

    if (auto it = j.find("format"); it != j.end() && it->is_string()
        && it->get<std::string>() != kPresetFormatName)
    {
        error = "not a rakarrack preset: format is \"" + it->get<std::string>() + "\"";
        return false;
    }

    if (auto it = j.find("version"); it != j.end() && it->is_number_integer()
        && it->get<int>() > kBankFormatVersion)
    {
        error = "preset was written by a newer version of rakarrack";
        return false;
    }

    // Parsed aside and only committed once the whole document has been read,
    // so a bad file cannot leave the caller with half a preset.
    Preset_Bank_Struct parsed = presetFromJson(j);
    GraphLayout parsedGraph;

    if (auto it = j.find("graph"); it != j.end() && it->is_object())
    {
        std::string graphError;
        if (!layoutFromJson(*it, parsedGraph, graphError))
        {
            error = graphError;
            return false;
        }
    }

    preset = parsed;
    if (graph != nullptr)
        *graph = std::move(parsedGraph);
    return true;
}

bool looksLikeJsonPreset(const char* data, std::size_t len)
{
    return looksLikeJsonBank(data, len);
}
