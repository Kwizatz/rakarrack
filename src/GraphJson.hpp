/*
  rakarrack - guitar multi-effects processor
  SPDX-License-Identifier: GPL-2.0-only

  GraphJson.hpp - Reading and writing effect graphs as JSON.

  A graph is a different shape from the legacy preset: the old format stored a
  flat list of sixteen effect type indices, which can only describe a chain of
  at most one instance per type. A graph has arbitrary nodes, arbitrary edges,
  and canvas positions, so it needs its own representation.

  Works on GraphLayout rather than EffectGraph, so nothing here has to know how
  to construct an effect and the whole thing stays testable on its own.
*/

#ifndef GRAPH_JSON_H
#define GRAPH_JSON_H

#include "EffectGraph.hpp"

#include <string>

/// Serialise a graph layout as JSON text.
[[nodiscard]] std::string graphToJson(const GraphLayout& layout);

/// Parse JSON graph text. Returns false and fills `error` on failure, leaving
/// `layout` untouched.
[[nodiscard]] bool graphFromJson(const std::string& text,
                                 GraphLayout& layout,
                                 std::string& error);

#endif
