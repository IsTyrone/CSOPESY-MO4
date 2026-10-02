/*
 *  CSOPESY Semi-Major Output 2 / MO4 — Desktop-Style OS Mock-up
 *  Compositor — Owns GLFW window, OpenGL context, ImGui backends, layer stack, and WindowManager
 */

#pragma once
#include <string>
#include <vector>
#include <memory>
#include "imgui.h"
#include "Config.h"
#include "ProcessSimulator.h"
#include "WindowManager.h"
#include "WallpaperTexture.h"

struct GLFWwindow;

class Compositor {
public:
    bool init();
    void run();
    void shutdown();
    void requestShutdown() { m_shutdownRequested = true; }
    bool shutdownRequested() const { return m_shutdownRequested; }

    // ── Window Management ──
    WindowManager&       windowManager()       { return m_windowManager; }
    const WindowManager& windowManager() const { return m_windowManager; }

    // Backward-compatibility delegation helpers
    bool                     isAppOpen(const std::string& id) const { return m_windowManager.isAppOpen(id); }
    void                     setAppOpen(const std::string& id, bool open);
    std::vector<std::string> openAppTitles() const { return m_windowManager.openAppTitles(); }
    int                      openAppCount() const { return m_windowManager.openAppCount(); }
    void                     setFocusedApp(const std::string& id) { m_windowManager.setFocusedApp(id); }
    const std::string&       focusedApp() const { return m_windowManager.focusedApp(); }

    // ── Screen Geometry ──
    ImVec2 viewportSize() const;
    float  taskbarHeight() const;
    void   desktopArea(ImVec2& outMin, ImVec2& outMax) const;

    const Config&           config() const    { return m_config; }
    const ProcessSimulator& simulator() const { return m_simulator; }

    WallpaperTexture& wallpaperTexture() { return m_wallpaperTexture; }
    const WallpaperTexture& wallpaperTexture() const { return m_wallpaperTexture; }

    static const char* appFiles()       { return "files"; }
    static const char* appSettings()    { return "settings"; }
    static const char* appTaskManager() { return "taskmanager"; }

private:
    void applyStyle();
    void beginFrame();
    void endFrame();
    void drawDesktopLayer();
    void drawAppWindows();
    void drawTaskBarLayer();
    void drawPwrLayer();

    WallpaperTexture m_wallpaperTexture;

    GLFWwindow*      m_window = nullptr;
    Config           m_config;
    ProcessSimulator m_simulator;
    WindowManager    m_windowManager;

    bool   m_shutdownRequested = false;
    double m_lastTime = 0.0;
    ImVec2 m_viewport = ImVec2(0.0f, 0.0f);
};
