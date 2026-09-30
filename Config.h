/*
 *  CSOPESY Semi-Major Output 2  —  Desktop-Style OS Mock-up
 *  Runtime parameters
 *
 *  Every value here can be overridden from config.txt at startup, so the
 *  mock-up can be re-configured without recompiling the project.
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
    bool        taskbarAtTop      = true;
    std::string wallpaperMode     = "gradient";   // gradient | pattern | plain

    bool loaded = false;

    // Load from file.  Returns true on success.
    bool loadFromFile(const std::string& path = "config.txt");

    // Pretty-print for verification
    std::vector<std::string> dump() const;
};
