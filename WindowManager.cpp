/*
 *  CSOPESY Semi-Major Output 2 / MO4 — Desktop-Style OS Mock-up
 *  WindowManager — Implementation
 */

#include "WindowManager.h"
#include "Compositor.h"

void WindowManager::registerApp(std::shared_ptr<IAppWindow> app) {
    if (!app) return;
    for (const auto& entry : m_entries) {
        if (entry.app->id() == app->id()) return;
    }
    m_entries.push_back(Entry{app, false, false});
}

void WindowManager::openApp(const std::string& id) {
    for (auto& entry : m_entries) {
        if (entry.app->id() == id) {
            entry.open = true;
            entry.bringToFront = true;
            m_focusedApp = id;
            return;
        }
    }
}

void WindowManager::closeApp(const std::string& id) {
    for (auto& entry : m_entries) {
        if (entry.app->id() == id) {
            entry.open = false;
            if (m_focusedApp == id) {
                m_focusedApp.clear();
                // Focus another open app if available
                for (const auto& other : m_entries) {
                    if (other.open) {
                        m_focusedApp = other.app->id();
                        break;
                    }
                }
            }
            return;
        }
    }
}

void WindowManager::toggleApp(const std::string& id) {
    for (auto& entry : m_entries) {
        if (entry.app->id() == id) {
            if (entry.open) {
                closeApp(id);
            } else {
                openApp(id);
            }
            return;
        }
    }
}

bool WindowManager::isAppOpen(const std::string& id) const {
    for (const auto& entry : m_entries) {
        if (entry.app->id() == id) return entry.open;
    }
    return false;
}

bool WindowManager::isAppFocused(const std::string& id) const {
    return m_focusedApp == id;
}

void WindowManager::setFocusedApp(const std::string& id) {
    for (auto& entry : m_entries) {
        if (entry.app->id() == id) {
            m_focusedApp = id;
            entry.bringToFront = true;
            return;
        }
    }
}

int WindowManager::openAppCount() const {
    int count = 0;
    for (const auto& entry : m_entries) {
        if (entry.open) count++;
    }
    return count;
}

std::vector<std::string> WindowManager::openAppTitles() const {
    std::vector<std::string> titles;
    for (const auto& entry : m_entries) {
        if (entry.open) titles.push_back(entry.app->title());
    }
    return titles;
}

std::vector<std::string> WindowManager::openAppIds() const {
    std::vector<std::string> ids;
    for (const auto& entry : m_entries) {
        if (entry.open) ids.push_back(entry.app->id());
    }
    return ids;
}

std::shared_ptr<IAppWindow> WindowManager::findApp(const std::string& id) const {
    for (const auto& entry : m_entries) {
        if (entry.app->id() == id) return entry.app;
    }
    return nullptr;
}

void WindowManager::renderWindows(Compositor& compositor) {
    for (auto& entry : m_entries) {
        if (!entry.open) continue;

        if (entry.bringToFront) {
            ImGui::SetNextWindowFocus();
            entry.bringToFront = false;
        }

        bool keepOpen = true;
        entry.app->render(compositor, &keepOpen);

        if (!keepOpen) {
            closeApp(entry.app->id());
        }
    }
}
