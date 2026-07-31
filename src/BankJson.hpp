/*
  rakarrack - guitar multi-effects processor
  SPDX-License-Identifier: GPL-2.0-only

  BankJson.hpp - Reading and writing preset banks as JSON.

  The historical .rkrb format is a raw fwrite of the bank array, which pins the
  file to the exact memory layout of Preset_Bank_Struct: adding a field, or
  loading on a machine of different endianness, silently reinterprets the whole
  file. That is why the format carries floats as printed strings and why banks
  are identified by their byte count. JSON removes all of that.

  Kept free of RKR so it can be tested without an audio engine.
*/

#ifndef BANK_JSON_H
#define BANK_JSON_H

#include "PresetBank.hpp"

#include <cstddef>
#include <string>

/// Preset slots in a bank file.
inline constexpr std::size_t kBankPresetCount = 62;

/// Serialise `count` presets as JSON text.
///
/// `graphs`, if given, is an array of `count` node layouts written alongside
/// each preset. A layout with no nodes is omitted, so a bank of plain chains
/// looks exactly as it did before.
[[nodiscard]] std::string bankToJson(const Preset_Bank_Struct* bank, std::size_t count,
                                    const GraphLayout* graphs = nullptr);

/// Parse JSON bank text into `bank`. Returns false and fills `error` on
/// failure, leaving `bank` untouched so a bad file cannot half-load.
///
/// `graphs`, if given, receives `count` layouts; presets without one are
/// cleared rather than left holding the previous bank's patch.
[[nodiscard]] bool bankFromJson(const std::string& text,
                                Preset_Bank_Struct* bank,
                                std::size_t count,
                                std::string& error,
                                GraphLayout* graphs = nullptr);

/// True when the data looks like a JSON bank rather than the legacy binary
/// blob. Used to pick a reader, so old and new files can share an extension.
[[nodiscard]] bool looksLikeJsonBank(const char* data, std::size_t len);

// ─── Single presets ────────────────────────────────────────────────
//
// One preset in a file of its own, written with the same field names as a
// preset inside a bank so the two cannot drift apart. The format it replaces
// was a sequence of unlabelled CSV lines whose reader had to know how many
// there would be, which it worked out by assuming a fixed number of active
// effects.

/// Serialise one preset, with its node layout if it has one.
[[nodiscard]] std::string singlePresetToJson(const Preset_Bank_Struct& preset,
                                             const GraphLayout* graph = nullptr);

/// Parse a single-preset document. Returns false and fills `error` on
/// failure, leaving both outputs untouched.
[[nodiscard]] bool singlePresetFromJson(const std::string& text,
                                        Preset_Bank_Struct& preset,
                                        std::string& error,
                                        GraphLayout* graph = nullptr);

/// True when the data looks like a JSON single preset rather than the legacy
/// line-based one.
[[nodiscard]] bool looksLikeJsonPreset(const char* data, std::size_t len);

#endif
