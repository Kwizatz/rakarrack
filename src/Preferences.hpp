/*
  rakarrack - a guitar effects software

  Preferences.hpp - Toolkit-agnostic preferences storage
  Replaces Fl_Preferences with nlohmann::json backend.

  Copyright (C) 2008-2010 Josep Andreu
  Copyright (C) 2024 Rodrigo Jose Hernandez Cordoba

 This program is free software; you can redistribute it and/or modify
 it under the terms of version 2 of the GNU General Public License
 as published by the Free Software Foundation.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License (version 2) for more details.

 You should have received a copy of the GNU General Public License
 (version2)  along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
*/

#pragma once

#include <nlohmann/json.hpp>
#include <filesystem>
#include <string>
#include <array>
#include <cstring>

/// Toolkit-agnostic user/system preferences store.
/// Drop-in replacement for Fl_Preferences with matching method signatures.
/// Data is persisted as JSON under $XDG_CONFIG_HOME/<application>/preferences.json.
class Preferences {
public:
    /// Mirrors Fl_Preferences::Root so call-sites compile unchanged.
    enum Root { SYSTEM = 0, USER = 1 };

    Preferences(Root root, const char* vendor, const char* application);
    ~Preferences();

    // Non-copyable, movable.
    Preferences(const Preferences&) = delete;
    Preferences& operator=(const Preferences&) = delete;
    Preferences(Preferences&&) noexcept = default;
    Preferences& operator=(Preferences&&) noexcept = default;

    // ---- int ----
    void get(const char* key, int& value, int defaultValue);
    void set(const char* key, int value);

    // ---- float ----
    void get(const char* key, float& value, float defaultValue);
    void set(const char* key, float value);

    // ---- double ----
    void get(const char* key, double& value, double defaultValue);
    void set(const char* key, double value);

    // ---- C-string (caller-owned buffer) ----
    void get(const char* key, char* value, const char* defaultValue, int maxSize);
    void set(const char* key, const char* value);

    // ---- std::array<char, N> convenience overload ----
    //  Handles members like std::array<char,128> MID directly.
    template <std::size_t N>
    void get(const char* key, std::array<char, N>& value,
             const char* defaultValue, int maxSize)
    {
        std::string s = m_data.value(key, std::string(defaultValue));
        auto len = std::min({s.size(),
                             static_cast<std::size_t>(maxSize > 0 ? maxSize - 1 : 0),
                             N - 1});
        std::memcpy(value.data(), s.c_str(), len);
        value[len] = '\0';
    }

    /// Explicitly flush to disk.
    void flush();

private:
    void load();
    void save();

    nlohmann::json          m_data;
    std::filesystem::path   m_filepath;
};
