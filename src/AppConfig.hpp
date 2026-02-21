#pragma once

#include <array>

// GUI and application configuration extracted from the RKR class.
// These are persisted via Fl_Preferences and skin files.
struct AppConfig {
    // Font & schema
    int font{};
    int relfontsize{};
    int sschema{};

    // Colors
    int slabel_color{};
    int sfore_color{};
    int sback_color{};
    int sleds_color{};

    // Window layout
    int resolution{};
    int flpos{};

    // Background image
    std::array<char, 256> BackgroundImage{};
    int EnableBackgroundImage{};

    // Auto-connect preferences
    int aconnect_MI{};
    int aconnect_JA{};
    int aconnect_JIA{};
    int comemouse{};
};
