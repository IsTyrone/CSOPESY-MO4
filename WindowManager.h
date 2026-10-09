/*
 *  CSOPESY Semi-Major Output 2 / MO4 — Desktop-Style OS Mock-up
 *  WindowManager — Manages registered applications, states, focus, and rendering
 */

#pragma once
#include <memory>
#include <string>
#include <vector>
#include "IAppWindow.h"

class Compositor;

class WindowManager {
public:
    struct Entry {
        std::shared_ptr<IAppWindow> app;
        bool open = false;
        bool bringToFront = false;
        bool maximized = false;
        bool minimized = false;
        bool needsReapply = false;
        ImVec2 normalPos = ImVec2(-1.0f, -1.0f);
        ImVec2 normalSize = ImVec2(-1.0f, -1.0f);
    };

    void registerApp(std::shared_ptr<IAppWindow> app);

    void openApp(const std::string& id);
    void closeApp(const std::string& id);
    void toggleApp(const std::string& id);

    bool isAppOpen(const std::string& id) const;
    bool isAppFocused(const std::string& id) const;
    bool isAppMaximized(const std::string& id) const;
    bool isAppMinimized(const std::string& id) const;

    void setFocusedApp(const std::string& id);
    const std::string& focusedApp() const { return m_focusedApp; }
    void setAppMaximized(const std::string& id, bool max);
    void toggleAppMaximized(const std::string& id);
    void setAppMinimized(const std::string& id, bool min);
    void restoreApp(const std::string& id);

    int openAppCount() const;
    std::vector<std::string> openAppTitles() const;
    std::vector<std::string> openAppIds() const;

    std::shared_ptr<IAppWindow> findApp(const std::string& id) const;
    const std::vector<Entry>& entries() const { return m_entries; }

    void onViewportOrScaleChanged(const ImVec2& oldDesktopMin, const ImVec2& oldDesktopMax,
                                  const ImVec2& newDesktopMin, const ImVec2& newDesktopMax,
                                  float scaleRatio);

    void renderWindows(Compositor& compositor);
    void renderTitleBarButtons(Compositor& compositor, const std::string& appId, bool* keepOpen);

private:
    std::vector<Entry> m_entries;
    std::string m_focusedApp;
};
