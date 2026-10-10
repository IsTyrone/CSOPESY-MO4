/*
 *  CSOPESY Semi-Major Output 2 / MO4 — Desktop-Style OS Mock-up
 *  WindowManager — Implementation with scalable window state management and title bar controls
 */

#include "WindowManager.h"
#include "Compositor.h"
#include "RetroGfx.h"
#include <algorithm>
#include <cmath>

void WindowManager::registerApp(std::shared_ptr<IAppWindow> app) {
    if (!app) return;
    for (const auto& entry : m_entries) {
        if (entry.app->id() == app->id()) return;
    }
    m_entries.push_back(Entry{app, false, false, false, false, false, ImVec2(-1.0f, -1.0f), ImVec2(-1.0f, -1.0f)});
}

void WindowManager::openApp(const std::string& id) {
    for (auto& entry : m_entries) {
        if (entry.app->id() == id) {
            entry.open = true;
            entry.minimized = false;
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
            entry.minimized = false;
            if (m_focusedApp == id) {
                m_focusedApp.clear();
                // Focus another open, visible app if available
                for (const auto& other : m_entries) {
                    if (other.open && !other.minimized) {
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
            if (!entry.open) {
                openApp(id);
            } else if (entry.minimized) {
                entry.minimized = false;
                entry.bringToFront = true;
                m_focusedApp = id;
            } else if (m_focusedApp == id) {
                // Clicking active app toggles minimize
                setAppMinimized(id, true);
            } else {
                entry.bringToFront = true;
                m_focusedApp = id;
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

bool WindowManager::isAppMaximized(const std::string& id) const {
    for (const auto& entry : m_entries) {
        if (entry.app->id() == id) return entry.maximized;
    }
    return false;
}

bool WindowManager::isAppMinimized(const std::string& id) const {
    for (const auto& entry : m_entries) {
        if (entry.app->id() == id) return entry.minimized;
    }
    return false;
}

void WindowManager::setFocusedApp(const std::string& id) {
    for (auto& entry : m_entries) {
        if (entry.app->id() == id) {
            m_focusedApp = id;
            entry.bringToFront = true;
            if (entry.minimized) {
                entry.minimized = false;
            }
            return;
        }
    }
}

void WindowManager::setAppMaximized(const std::string& id, bool max) {
    for (auto& entry : m_entries) {
        if (entry.app->id() == id) {
            entry.maximized = max;
            entry.minimized = false;
            entry.needsReapply = true;
            entry.bringToFront = true;
            m_focusedApp = id;
            return;
        }
    }
}

void WindowManager::toggleAppMaximized(const std::string& id) {
    for (auto& entry : m_entries) {
        if (entry.app->id() == id) {
            setAppMaximized(id, !entry.maximized);
            return;
        }
    }
}

void WindowManager::setAppMinimized(const std::string& id, bool min) {
    for (auto& entry : m_entries) {
        if (entry.app->id() == id) {
            entry.minimized = min;
            if (min && m_focusedApp == id) {
                m_focusedApp.clear();
                for (const auto& other : m_entries) {
                    if (other.open && !other.minimized) {
                        m_focusedApp = other.app->id();
                        break;
                    }
                }
            }
            return;
        }
    }
}

void WindowManager::restoreApp(const std::string& id) {
    for (auto& entry : m_entries) {
        if (entry.app->id() == id) {
            entry.minimized = false;
            entry.bringToFront = true;
            m_focusedApp = id;
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

void WindowManager::onViewportOrScaleChanged(const ImVec2& oldDesktopMin, const ImVec2& oldDesktopMax,
                                              const ImVec2& newDesktopMin, const ImVec2& newDesktopMax,
                                              float scaleRatio) {
    const float oldW = oldDesktopMax.x - oldDesktopMin.x;
    const float oldH = oldDesktopMax.y - oldDesktopMin.y;
    const float newW = newDesktopMax.x - newDesktopMin.x;
    const float newH = newDesktopMax.y - newDesktopMin.y;

    for (auto& entry : m_entries) {
        if (entry.normalSize.x > 0.0f) {
            if (scaleRatio > 0.0f && std::abs(scaleRatio - 1.0f) > 0.01f) {
                entry.normalSize.x *= scaleRatio;
                entry.normalSize.y *= scaleRatio;
            }
            entry.normalSize.x = std::clamp(entry.normalSize.x, 260.0f, newW);
            entry.normalSize.y = std::clamp(entry.normalSize.y, 200.0f, newH);
        }

        if (entry.normalPos.x >= 0.0f) {
            if (oldW > 10.0f && oldH > 10.0f) {
                const float rx = (entry.normalPos.x - oldDesktopMin.x) / oldW;
                const float ry = (entry.normalPos.y - oldDesktopMin.y) / oldH;
                entry.normalPos.x = newDesktopMin.x + rx * newW;
                entry.normalPos.y = newDesktopMin.y + ry * newH;
            }
            const float maxX = std::max(newDesktopMin.x, newDesktopMax.x - entry.normalSize.x);
            const float maxY = std::max(newDesktopMin.y, newDesktopMax.y - entry.normalSize.y);
            entry.normalPos.x = std::clamp(entry.normalPos.x, newDesktopMin.x, maxX);
            entry.normalPos.y = std::clamp(entry.normalPos.y, newDesktopMin.y, maxY);
        }
        entry.needsReapply = true;
    }
}

void WindowManager::renderTitleBarButtons(Compositor& compositor, const std::string& appId, bool* keepOpen) {
    Entry* pEntry = nullptr;
    for (auto& e : m_entries) {
        if (e.app->id() == appId) {
            pEntry = &e;
            break;
        }
    }
    if (!pEntry) return;

    const float scale = compositor.effectiveScale();
    const ImVec2 wPos = ImGui::GetWindowPos();
    const ImVec2 wSize = ImGui::GetWindowSize();
    const float titleH = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;

    const ImVec2 mousePos = ImGui::GetIO().MousePos;
    const bool inTitleBar = (mousePos.x >= wPos.x && mousePos.x <= wPos.x + wSize.x &&
                             mousePos.y >= wPos.y && mousePos.y <= wPos.y + titleH);

    const float btnSize = std::floor(std::max(16.0f * scale, titleH - 4.0f * scale));
    const float pad = 4.0f * scale;
    const float gap = 2.0f * scale;
    const float btnY = wPos.y + (titleH - btnSize) * 0.5f;

    const ImVec2 cMin(wPos.x + wSize.x - pad - btnSize, btnY);
    const ImVec2 cMax(cMin.x + btnSize, btnY + btnSize);

    const ImVec2 mMin(cMin.x - gap - btnSize, btnY);
    const ImVec2 mMax(mMin.x + btnSize, btnY + btnSize);

    const ImVec2 nMin(mMin.x - gap - btnSize, btnY);
    const ImVec2 nMax(nMin.x + btnSize, btnY + btnSize);

    const bool inButtons = (mousePos.x >= nMin.x && mousePos.x <= cMax.x &&
                            mousePos.y >= btnY && mousePos.y <= btnY + btnSize);

    // Double-click on title bar toggles maximize
    if (inTitleBar && !inButtons && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
        toggleAppMaximized(appId);
    }

    const ImVec2 savedCursor = ImGui::GetCursorScreenPos();

    // 1. Minimize Button
    ImGui::SetCursorScreenPos(nMin);
    const std::string nId = "##tb_min_" + appId;
    const bool nClicked = ImGui::InvisibleButton(nId.c_str(), ImVec2(btnSize, btnSize));
    const bool nActive = ImGui::IsItemActive();

    // 2. Maximize Button
    ImGui::SetCursorScreenPos(mMin);
    const std::string mId = "##tb_max_" + appId;
    const bool mClicked = ImGui::InvisibleButton(mId.c_str(), ImVec2(btnSize, btnSize));
    const bool mActive = ImGui::IsItemActive();

    // 3. Close Button
    ImGui::SetCursorScreenPos(cMin);
    const std::string cId = "##tb_close_" + appId;
    const bool cClicked = ImGui::InvisibleButton(cId.c_str(), ImVec2(btnSize, btnSize));
    const bool cActive = ImGui::IsItemActive();

    ImGui::SetCursorScreenPos(savedCursor);

    if (nClicked) {
        setAppMinimized(appId, true);
    }
    if (mClicked) {
        toggleAppMaximized(appId);
    }
    if (cClicked && keepOpen) {
        *keepOpen = false;
    }

    ImDrawList* dl = ImGui::GetWindowDrawList();

    RetroGfx::draw3DBox(dl, nMin, nMax, nActive, RetroGfx::kGrayFace);
    RetroGfx::draw3DBox(dl, mMin, mMax, mActive, RetroGfx::kGrayFace);
    RetroGfx::draw3DBox(dl, cMin, cMax, cActive, RetroGfx::kGrayFace);

    RetroGfx::drawMinimizeIcon(dl, nMin, nMax, scale);
    RetroGfx::drawMaximizeIcon(dl, mMin, mMax, pEntry->maximized, scale);
    RetroGfx::drawCloseIcon(dl, cMin, cMax, scale);
}

void WindowManager::renderWindows(Compositor& compositor) {
    ImVec2 areaMin, areaMax;
    compositor.desktopArea(areaMin, areaMax);
    const float desktopW = areaMax.x - areaMin.x;
    const float desktopH = areaMax.y - areaMin.y;
    const float scale = compositor.effectiveScale();

    for (size_t i = 0; i < m_entries.size(); ++i) {
        auto& entry = m_entries[i];
        if (!entry.open || entry.minimized) continue;

        if (entry.bringToFront) {
            ImGui::SetNextWindowFocus();
            entry.bringToFront = false;
        }

        if (entry.maximized) {
            ImGui::SetNextWindowPos(areaMin, ImGuiCond_Always);
            ImGui::SetNextWindowSize(ImVec2(desktopW, desktopH), ImGuiCond_Always);
        } else {
            if (entry.normalSize.x <= 0.0f) {
                ImVec2 defSize;
                if (entry.app->id() == "files") {
                    defSize = ImVec2(std::min(760.0f * scale, desktopW * 0.90f),
                                     std::min(500.0f * scale, desktopH * 0.88f));
                } else if (entry.app->id() == "settings") {
                    defSize = ImVec2(std::min(600.0f * scale, desktopW * 0.85f),
                                     std::min(520.0f * scale, desktopH * 0.85f));
                } else {
                    defSize = ImVec2(std::min(700.0f * scale, desktopW * 0.88f),
                                     std::min(520.0f * scale, desktopH * 0.85f));
                }
                entry.normalSize = defSize;
                const float stagger = static_cast<float>(i) * 26.0f * scale;
                entry.normalPos = ImVec2(areaMin.x + (desktopW - defSize.x) * 0.5f + stagger,
                                         areaMin.y + (desktopH - defSize.y) * 0.5f + stagger);
                entry.normalPos.x = std::clamp(entry.normalPos.x, areaMin.x + 4.0f,
                                               std::max(areaMin.x + 4.0f, areaMax.x - defSize.x - 4.0f));
                entry.normalPos.y = std::clamp(entry.normalPos.y, areaMin.y + 4.0f,
                                               std::max(areaMin.y + 4.0f, areaMax.y - defSize.y - 4.0f));
                ImGui::SetNextWindowPos(entry.normalPos, ImGuiCond_Always);
                ImGui::SetNextWindowSize(entry.normalSize, ImGuiCond_Always);
            } else if (entry.needsReapply) {
                ImGui::SetNextWindowPos(entry.normalPos, ImGuiCond_Always);
                ImGui::SetNextWindowSize(entry.normalSize, ImGuiCond_Always);
                entry.needsReapply = false;
            }
        }

        bool keepOpen = true;
        entry.app->render(compositor, &keepOpen);

        if (!keepOpen) {
            closeApp(entry.app->id());
            continue;
        }

        if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows)) {
            m_focusedApp = entry.app->id();
        }

        if (!entry.maximized) {
            entry.normalPos = ImGui::GetWindowPos();
            entry.normalSize = ImGui::GetWindowSize();
        }
    }
}
