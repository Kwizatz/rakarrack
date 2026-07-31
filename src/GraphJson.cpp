/*
  rakarrack - guitar multi-effects processor
  SPDX-License-Identifier: GPL-2.0-only

  GraphJson.cpp - Reading and writing effect graphs as JSON.
*/

#include "GraphJson.hpp"

#include <nlohmann/json.hpp>

#include <string>
#include <vector>

using nlohmann::json;

namespace {

constexpr int kGraphFormatVersion = 1;
constexpr const char* kGraphFormatName = "rakarrack-graph";

/// Mix modes are written by name. The numeric values are an implementation
/// detail, and a preset that silently changed a reverb's dry/wet staging
/// because an enum was reordered would be very hard to notice.
const char* mixModeName(MixMode mix)
{
    switch (mix)
    {
    case MixMode::Gain2x: return "gain2x";
    case MixMode::WetDry: return "wetdry";
    case MixMode::Replace:
    default:              return "replace";
    }
}

bool mixModeFromName(const std::string& name, MixMode& out)
{
    if (name == "replace") { out = MixMode::Replace; return true; }
    if (name == "gain2x")  { out = MixMode::Gain2x;  return true; }
    if (name == "wetdry")  { out = MixMode::WetDry;  return true; }
    return false;
}

/// The endpoints are named rather than written as their negative ids, so the
/// file reads as a wiring diagram instead of a puzzle.
json endpointToJson(int id)
{
    if (id == kInputNodeId)  return "input";
    if (id == kOutputNodeId) return "output";
    return id;
}

bool endpointFromJson(const json& j, int& out)
{
    if (j.is_string())
    {
        const std::string s = j.get<std::string>();
        if (s == "input")  { out = kInputNodeId;  return true; }
        if (s == "output") { out = kOutputNodeId; return true; }
        return false;
    }
    if (j.is_number_integer())
    {
        out = j.get<int>();
        return true;
    }
    return false;
}

} // namespace

std::string graphToJson(const GraphLayout& layout)
{
    json j = layoutToJson(layout);
    j["format"]  = kGraphFormatName;
    j["version"] = kGraphFormatVersion;
    return j.dump(2);
}

json layoutToJson(const GraphLayout& layout)
{
    json j;

    json nodes = json::array();
    for (const GraphNodeLayout& n : layout.nodes)
    {
        json node;
        node["id"]   = n.id;
        node["type"] = n.type;
        node["mix"]  = mixModeName(n.mix);
        if (n.bypassed)
            node["bypassed"] = true;
        node["x"] = n.x;
        node["y"] = n.y;

        // The node's own settings, which is what lets two nodes of the same
        // type differ. Written even when empty so a hand-edited file has an
        // obvious place to put them.
        node["preset"] = n.settings.preset;
        node["params"] = n.settings.params;

        nodes.push_back(std::move(node));
    }
    j["nodes"] = std::move(nodes);

    json connections = json::array();
    for (const Connection& c : layout.connections)
    {
        json edge;
        edge["from"] = endpointToJson(c.from);
        edge["to"]   = endpointToJson(c.to);
        connections.push_back(std::move(edge));
    }
    j["connections"] = std::move(connections);

    return j;
}

bool graphFromJson(const std::string& text, GraphLayout& layout, std::string& error)
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
        && it->get<std::string>() != kGraphFormatName)
    {
        error = "not a rakarrack graph: format is \"" + it->get<std::string>() + "\"";
        return false;
    }

    if (auto it = j.find("version"); it != j.end() && it->is_number_integer()
        && it->get<int>() > kGraphFormatVersion)
    {
        error = "graph was written by a newer version of rakarrack";
        return false;
    }

    return layoutFromJson(j, layout, error);
}

bool layoutFromJson(const json& j, GraphLayout& layout, std::string& error)
{
    auto nodesIt = j.find("nodes");
    if (nodesIt == j.end() || !nodesIt->is_array())
    {
        error = "graph has no node list";
        return false;
    }

    GraphLayout parsed;

    for (const json& node : *nodesIt)
    {
        if (!node.is_object())
        {
            error = "a node is not an object";
            return false;
        }

        GraphNodeLayout n;

        auto idIt = node.find("id");
        if (idIt == node.end() || !idIt->is_number_integer())
        {
            error = "a node has no id";
            return false;
        }
        n.id = idIt->get<int>();
        if (n.id == kInputNodeId || n.id == kOutputNodeId || n.id < 0)
        {
            error = "a node uses a reserved id";
            return false;
        }

        auto typeIt = node.find("type");
        if (typeIt == node.end() || !typeIt->is_number_integer())
        {
            error = "a node has no effect type";
            return false;
        }
        n.type = typeIt->get<int>();

        if (auto it = node.find("mix"); it != node.end() && it->is_string())
        {
            if (!mixModeFromName(it->get<std::string>(), n.mix))
            {
                error = "a node has an unknown mix mode \"" + it->get<std::string>() + "\"";
                return false;
            }
        }
        else
        {
            n.mix = defaultMixModeForType(n.type);
        }

        if (auto it = node.find("bypassed"); it != node.end() && it->is_boolean())
            n.bypassed = it->get<bool>();
        if (auto it = node.find("x"); it != node.end() && it->is_number())
            n.x = it->get<float>();
        if (auto it = node.find("y"); it != node.end() && it->is_number())
            n.y = it->get<float>();

        if (auto it = node.find("preset"); it != node.end() && it->is_number_integer())
            n.settings.preset = it->get<int>();

        if (auto it = node.find("params"); it != node.end())
        {
            if (!it->is_array())
            {
                error = "a node's parameters are not a list";
                return false;
            }
            for (const json& value : *it)
            {
                if (!value.is_number_integer())
                {
                    error = "a node has a non-integer parameter";
                    return false;
                }
                n.settings.params.push_back(value.get<int>());
            }
        }

        for (const GraphNodeLayout& seen : parsed.nodes)
        {
            if (seen.id == n.id)
            {
                error = "two nodes share id " + std::to_string(n.id);
                return false;
            }
        }

        parsed.nodes.push_back(n);
    }

    if (auto edgesIt = j.find("connections"); edgesIt != j.end() && edgesIt->is_array())
    {
        for (const json& edge : *edgesIt)
        {
            if (!edge.is_object())
            {
                error = "a connection is not an object";
                return false;
            }

            Connection c;
            auto fromIt = edge.find("from");
            auto toIt   = edge.find("to");
            if (fromIt == edge.end() || toIt == edge.end()
                || !endpointFromJson(*fromIt, c.from)
                || !endpointFromJson(*toIt, c.to))
            {
                error = "a connection has a bad endpoint";
                return false;
            }
            parsed.connections.push_back(c);
        }
    }

    layout = std::move(parsed);
    return true;
}
