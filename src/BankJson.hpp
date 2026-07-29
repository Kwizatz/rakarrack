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
[[nodiscard]] std::string bankToJson(const Preset_Bank_Struct* bank, std::size_t count);

/// Parse JSON bank text into `bank`. Returns false and fills `error` on
/// failure, leaving `bank` untouched so a bad file cannot half-load.
[[nodiscard]] bool bankFromJson(const std::string& text,
                                Preset_Bank_Struct* bank,
                                std::size_t count,
                                std::string& error);

/// True when the data looks like a JSON bank rather than the legacy binary
/// blob. Used to pick a reader, so old and new files can share an extension.
[[nodiscard]] bool looksLikeJsonBank(const char* data, std::size_t len);

#endif
