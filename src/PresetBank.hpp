#pragma once

#include <array>
#include <cstring>

// Struct definitions for the preset/bank system.
// These are binary-serialized via fread/fwrite — do NOT change layout.

struct Preset_Bank_Struct {
    std::array<char, 64> Preset_Name{};
    std::array<char, 64> Author{};
    std::array<char, 36> Classe{};
    std::array<char, 4> Type{};
    std::array<char, 128> ConvoFiname{};
    std::array<char, 64> cInput_Gain{};
    std::array<char, 64> cMaster_Volume{};
    std::array<char, 64> cBalance{};
    float Input_Gain;
    float Master_Volume;
    float Balance;
    int Bypass;
    std::array<char, 128> RevFiname{};
    std::array<char, 128> EchoFiname{};
    std::array<std::array<int, 20>, 70> lv{};
    std::array<std::array<int, 20>, 128> XUserMIDI{};
    std::array<int, 128> XMIDIrangeMin{};
    std::array<int, 128> XMIDIrangeMax{};
};

struct MIDI_Table_Entry {
    int bank;
    int preset;
};

struct Bank_Names {
    std::array<char, 64> Preset_Name{};
};

// Groups all preset/bank-related data that was previously scattered in the RKR class.
struct PresetBank {
    // Current preset
    std::array<char, 64> Preset_Name{};
    std::array<char, 64> Author{};

    // Paths
    std::array<char, 128> Bank_Saved{};
    std::array<char, 128> BankFilename{};
    std::array<char, 128> UDirFilename{};
    std::array<char, 128> UserRealName{};

    // State
    int Selected_Preset{};
    int a_bank{};
    int new_bank_loaded{};

    // Storage
    Preset_Bank_Struct Bank[62]{};
    MIDI_Table_Entry M_table[128]{};
    Bank_Names B_Names[4][62]{};
};
