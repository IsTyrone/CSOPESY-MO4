/*
 *  CSOPESY Semi-Major Output 2 / MO4 — Desktop-Style OS Mock-up
 *  Config — Runtime parameters loaded from config.txt
 */

#pragma once
#include <string>
#include <vector>

struct Config {
    int         framebufferWidth  = 1280;
    int         framebufferHeight = 800;
    int         processCount      = 6;
    float       minCpu            = 0.5f;
    float       maxCpu            = 42.0f;
    int         minMemKb          = 2048;
    int         maxMemKb          = 65536;
    int         totalMemKb        = 262144;
    int         updateIntervalMs  = 180;
    unsigned    rngSeed           = 20260918u;
    bool        taskbarAtTop      = false;          // false = classic bottom taskbar, true = top
    std::string wallpaperMode     = "classic-teal"; // classic-teal | gradient | bliss | pattern | plain
    float       uiScale           = 1.25f;          // Scale factor to ensure crisp, readable icons
    bool        showDesktopIcons  = true;           // Windows 95/98 desktop shortcuts
    std::string theme             = "classic";      // classic | modern

    bool loaded = false;

    // Load from file. Returns true on success.
    bool loadFromFile(const std::string& path = "config.txt");

    // Pretty-print for verification
    std::vector<std::string> dump() const;
};
