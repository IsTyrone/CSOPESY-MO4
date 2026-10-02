/*
 *  CSOPESY Semi-Major Output 2 / MO4 — Desktop-Style OS Mock-up
 *  Config — Implementation
 */

#include "Config.h"
#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <sstream>

// Helper: remove leading/trailing whitespace
static std::string trim(const std::string& s) {
    auto b = s.find_first_not_of(" \t\r\n");
    if (b == std::string::npos) return "";
    auto e = s.find_last_not_of(" \t\r\n");
    return s.substr(b, e - b + 1);
}

// Helper: strip surrounding quotes from a string value
static std::string stripQuotes(const std::string& s) {
    if (s.size() >= 2 && s.front() == '"' && s.back() == '"')
        return s.substr(1, s.size() - 2);
    return s;
}

bool Config::loadFromFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) return false;

    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;   // skip blanks & comments

        std::istringstream iss(line);
        std::string key, value;
        if (!(iss >> key >> value)) continue;

        value = stripQuotes(value);

        if      (key == "framebuffer-width")   framebufferWidth   = std::atoi(value.c_str());
        else if (key == "framebuffer-height")  framebufferHeight  = std::atoi(value.c_str());
        else if (key == "process-count")       processCount       = std::atoi(value.c_str());
        else if (key == "min-cpu")             minCpu             = static_cast<float>(std::atof(value.c_str()));
        else if (key == "max-cpu")             maxCpu             = static_cast<float>(std::atof(value.c_str()));
        else if (key == "min-mem-kb")          minMemKb           = std::atoi(value.c_str());
        else if (key == "max-mem-kb")          maxMemKb           = std::atoi(value.c_str());
        else if (key == "total-mem-kb")        totalMemKb         = std::atoi(value.c_str());
        else if (key == "update-interval-ms")  updateIntervalMs   = std::atoi(value.c_str());
        else if (key == "rng-seed")            rngSeed            = static_cast<unsigned>(std::strtoul(value.c_str(), nullptr, 10));
        else if (key == "taskbar-position")    taskbarAtTop       = (value == "top");
        else if (key == "wallpaper-mode")      wallpaperMode      = value;
        else if (key == "ui-scale")            uiScale            = static_cast<float>(std::atof(value.c_str()));
        else if (key == "show-desktop-icons")  showDesktopIcons   = (value == "true" || value == "1" || value == "yes");
        else if (key == "theme")               theme              = value;
    }

    // Guard rails
    if (processCount < 1)       processCount       = 1;
    if (processCount > 64)      processCount       = 64;
    if (framebufferWidth < 800)  framebufferWidth  = 800;
    if (framebufferHeight < 600) framebufferHeight = 600;
    if (updateIntervalMs < 16)   updateIntervalMs  = 16;
    if (maxCpu < minCpu)         maxCpu            = minCpu;
    if (maxMemKb < minMemKb)     maxMemKb          = minMemKb;
    if (minMemKb < 64)           minMemKb          = 64;
    if (totalMemKb < maxMemKb)   totalMemKb        = maxMemKb * processCount;
    if (uiScale < 0.75f)         uiScale           = 0.75f;
    if (uiScale > 2.50f)         uiScale           = 2.50f;

    loaded = true;
    return true;
}

std::vector<std::string> Config::dump() const {
    std::vector<std::string> out;
    out.push_back("--- Configuration ---");
    out.push_back("  framebuffer:         " + std::to_string(framebufferWidth) + " x " + std::to_string(framebufferHeight));
    out.push_back("  process-count:       " + std::to_string(processCount));
    out.push_back("  cpu range:           " + std::to_string(static_cast<int>(minCpu)) + "% - " + std::to_string(static_cast<int>(maxCpu)) + "%");
    out.push_back("  memory range:        " + std::to_string(minMemKb) + " - " + std::to_string(maxMemKb) + " KB");
    out.push_back("  total memory:        " + std::to_string(totalMemKb) + " KB");
    out.push_back("  update interval:     " + std::to_string(updateIntervalMs) + " ms");
    out.push_back("  rng-seed:            " + std::to_string(rngSeed));
    out.push_back("  taskbar-position:    " + std::string(taskbarAtTop ? "top" : "bottom"));
    out.push_back("  wallpaper-mode:      " + wallpaperMode);
    out.push_back("  ui-scale:            " + std::to_string(uiScale));
    out.push_back("  theme:               " + theme);
    out.push_back("  config.txt loaded:   " + std::string(loaded ? "yes" : "no (using defaults)"));
    out.push_back("---------------------");
    return out;
}
