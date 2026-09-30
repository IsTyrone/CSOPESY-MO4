/*
 *  CSOPESY Semi-Major Output 2  —  Desktop-Style OS Mock-up
 *  Compositor
 *
 *  The spec's framing: "your CSOPESY emulator will have its own compositor
 *  built on top of the GLFW + OpenGL + Dear ImGui stack".  This class owns
 *  the host window, the ImGui backends, the ordered layer stack, and the
 *  registry of running application windows.  Every frame it drives the
 *  same sequence a real compositor uses: poll events, begin a frame, paint
 *  each layer back-to-front, submit, and swap.
 *
 *  Shutdown is deliberately one-way and PWR-gated: requestShutdown() is the
 *  only path that ends the loop, satisfying "the application must be closed
 *  using this button, and not by force".
 */

#pragma once
#include <string>
#include <vector>
#include "imgui.h"
#include "Config.h"
#include "ProcessSimulator.h"

struct GLFWwindow;

class Compositor {
public:
    struct AppEntry {
        std::string title;
        bool        open = false;
    };

    bool init();
    void run();
    void shutdown();
    void requestShutdown() { m_shutdownRequested = true; }
    bool shutdownRequested() const { return m_shutdownRequested; }

    // ── Running-application registry (TaskBar "shows running applications") ──
    void                     registerApp(const std::string& title);
    void                     setAppOpen(const std::string& title, bool open);
    bool                     isAppOpen(const std::string& title) const;
    std::vector<std::string> openAppTitles() const;
    int                      openAppCount() const { return m_openAppCount; }
    void                     setFocusedApp(const std::string& title) { m_focusedApp = title; }
    const std::string&       focusedApp() const { return m_focusedApp; }

    // ── Screen geometry ──
    ImVec2 viewportSize() const;
    float   taskbarHeight() const;
    // Usable desktop rectangle, i.e. the viewport minus the taskbar strip.
    void desktopArea(ImVec2& outMin, ImVec2& outMax) const;

    const Config&           config() const { return m_config; }
    const ProcessSimulator& simulator() const { return m_simulator; }

    static const char* appFiles();
    static const char* appSettings();
    static const char* appTaskManager();

private:
    void applyStyle();
    void beginFrame();
    void endFrame();
    void drawDesktopLayer();
    void drawAppWindows();
    void drawTaskBarLayer();
    void drawPwrLayer();

    GLFWwindow*   m_window = nullptr;
    Config        m_config;
    ProcessSimulator m_simulator;

    std::vector<AppEntry> m_apps;
    std::string m_focusedApp;
    int         m_openAppCount = 0;
    bool        m_shutdownRequested = false;
    double      m_lastTime = 0.0;
    ImVec2      m_viewport = ImVec2(0.0f, 0.0f);
};
