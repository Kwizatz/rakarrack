/*
  rakarrack - a guitar effects software

  Preferences.cpp - Toolkit-agnostic preferences storage
  JSON-backed preferences using nlohmann::json.

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

#include "Preferences.hpp"
#include <fstream>
#include <iostream>

Preferences::Preferences(Root /*root*/, const char* /*vendor*/,
                         const char* application)
{
    // Determine base config directory (XDG_CONFIG_HOME or ~/.config).
    const char* xdg = std::getenv("XDG_CONFIG_HOME");
    if (xdg && xdg[0] != '\0') {
        m_filepath = std::filesystem::path(xdg) / application / "preferences.json";
    } else {
        const char* home = std::getenv("HOME");
        m_filepath = std::filesystem::path(home ? home : "/tmp")
                     / ".config" / application / "preferences.json";
    }
    load();
}

Preferences::~Preferences()
{
    save();
}

// --------------- int ---------------
void Preferences::get(const char* key, int& value, int defaultValue)
{
    value = m_data.value(key, defaultValue);
}

void Preferences::set(const char* key, int value)
{
    m_data[key] = value;
}

// --------------- float ---------------
void Preferences::get(const char* key, float& value, float defaultValue)
{
    value = m_data.value(key, defaultValue);
}

void Preferences::set(const char* key, float value)
{
    m_data[key] = value;
}

// --------------- double ---------------
void Preferences::get(const char* key, double& value, double defaultValue)
{
    value = m_data.value(key, defaultValue);
}

void Preferences::set(const char* key, double value)
{
    m_data[key] = value;
}

// --------------- C-string ---------------
void Preferences::get(const char* key, char* value,
                       const char* defaultValue, int maxSize)
{
    std::string s = m_data.value(key, std::string(defaultValue));
    if (maxSize > 0) {
        std::strncpy(value, s.c_str(), static_cast<std::size_t>(maxSize - 1));
        value[maxSize - 1] = '\0';
    }
}

void Preferences::set(const char* key, const char* value)
{
    m_data[key] = std::string(value);
}

// --------------- persistence ---------------
void Preferences::flush()
{
    save();
}

void Preferences::load()
{
    if (std::filesystem::exists(m_filepath)) {
        std::ifstream f(m_filepath);
        if (f.good()) {
            try {
                m_data = nlohmann::json::parse(f);
            } catch (const std::exception& e) {
                std::cerr << "[Preferences] Failed to parse "
                          << m_filepath << ": " << e.what() << '\n';
                m_data = nlohmann::json::object();
            }
        }
    } else {
        m_data = nlohmann::json::object();
    }
}

void Preferences::save()
{
    try {
        std::filesystem::create_directories(m_filepath.parent_path());
        std::ofstream f(m_filepath);
        if (f.good()) {
            f << m_data.dump(2) << '\n';
        }
    } catch (const std::exception& e) {
        std::cerr << "[Preferences] Failed to save "
                  << m_filepath << ": " << e.what() << '\n';
    }
}
