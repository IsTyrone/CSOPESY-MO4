/*
 *  CSOPESY Semi-Major Output 2 / MO4 — Desktop-Style OS Mock-up
 *  Requirement B — TaskBar Implementation with Start Button, Window Tabs, and System Tray
 */

#include "TaskBar.h"
#include "Compositor.h"
#include "RetroGfx.h"
#include <ctime>
#include <cstdio>
#include <algorithm>

namespace {

static bool s_startMenuOpen = false;

// Scalable Quick Launch / Launcher Button
bool drawTaskBarLauncher(const char* id, const char* tooltip, const char* appId,
                         Compositor& compositor, ImVec2& cursor, float btnSize, float iconSize,
                         void (*iconDrawer)(ImDrawList*, const ImVec2&, float)) {
    const ImVec2 pMin = cursor;
    const ImVec2 pMax(cursor.x + btnSize, cursor.y + btnSize);

    ImGui::SetCursorScreenPos(pMin);
    ImGui::InvisibleButton(id, ImVec2(btnSize, btnSize));

    const bool hovered = ImGui::IsItemHovered();
    const bool pressed = ImGui::IsItemActive();
    const bool clicked = ImGui::IsItemClicked();
    const bool isOpen  = compositor.windowManager().isAppOpen(appId);

    ImDrawList* dl = ImGui::GetWindowDrawList();

    // 3D Bevel button
    RetroGfx::draw3DBox(dl, pMin, pMax, pressed || isOpen, RetroGfx::kGrayFace);

    const ImVec2 center(pMin.x + btnSize * 0.5f, pMin.y + btnSize * 0.5f);
    iconDrawer(dl, center, iconSize);

    // Active pip
    if (isOpen) {
        dl->AddRectFilled(ImVec2(pMin.x + 3.0f, pMax.y - 4.0f), ImVec2(pMax.x - 3.0f, pMax.y - 2.0f),
                          IM_COL32(0, 180, 80, 255));
    }

    if (hovered) {
        ImGui::SetTooltip("%s%s", tooltip, isOpen ? " (Running)" : "");
    }

    cursor.x += btnSize + 4.0f;
    return clicked;
}

// Start Menu Popup
void drawStartMenu(Compositor& compositor, const ImVec2& barOrigin, float barH) {
    if (!s_startMenuOpen) return;

    const float uiScale = compositor.config().uiScale;
    const float menuW = 210.0f * uiScale;
    const float menuH = 240.0f * uiScale;
    const bool barAtTop = compositor.config().taskbarAtTop;

    const ImVec2 menuPos = barAtTop ? ImVec2(barOrigin.x, barOrigin.y + barH)
                                    : ImVec2(barOrigin.x, barOrigin.y - menuH);

    const ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize;

    ImGui::SetNextWindowPos(menuPos);
    ImGui::SetNextWindowSize(ImVec2(menuW, menuH));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(3.0f, 3.0f));
    ImGui::Begin("##StartMenuPopup", &s_startMenuOpen, flags);
    ImGui::PopStyleVar();

    ImDrawList* dl = ImGui::GetWindowDrawList();
    const ImVec2 wMin = ImGui::GetWindowPos();
    const ImVec2 wMax(wMin.x + menuW, wMin.y + menuH);

    // Outer 3D bevel for Start menu
    RetroGfx::draw3DBox(dl, wMin, wMax, false, RetroGfx::kGrayFace);

    // Classic Windows 95/98 Left Vertical Banner
    const float stripeW = 28.0f * uiScale;
    const ImVec2 stripeMin(wMin.x + 3.0f, wMin.y + 3.0f);
    const ImVec2 stripeMax(wMin.x + stripeW, wMax.y - 3.0f);
    dl->AddRectFilledMultiColor(stripeMin, stripeMax,
                                IM_COL32(0, 0, 128, 255), IM_COL32(0, 0, 128, 255),
                                IM_COL32(16, 96, 192, 255), IM_COL32(16, 96, 192, 255));

    // Vertical text indicator (drawn manually or labeled)
    const float itemX = wMin.x + stripeW + 8.0f;
    float itemY = wMin.y + 12.0f;
    const float itemH = 34.0f * uiScale;
    const float itemW = menuW - stripeW - 14.0f;

    auto renderMenuItem = [&](const char* label, const char* appId, void (*icon)(ImDrawList*, const ImVec2&, float)) {
        const ImVec2 miMin(itemX, itemY);
        const ImVec2 miMax(itemX + itemW, itemY + itemH);

        ImGui::SetCursorScreenPos(miMin);
        ImGui::InvisibleButton(label, ImVec2(itemW, itemH));
        const bool hov = ImGui::IsItemHovered();
        const bool clk = ImGui::IsItemClicked();

        if (hov) {
            dl->AddRectFilled(miMin, miMax, RetroGfx::kNavyActive);
        }

        const ImVec2 iCenter(miMin.x + 18.0f, miMin.y + itemH * 0.5f);
        icon(dl, iCenter, 22.0f * uiScale);

        const float textY = miMin.y + (itemH - ImGui::GetFontSize()) * 0.5f;
        dl->AddText(ImVec2(miMin.x + 38.0f, textY), hov ? RetroGfx::kWhite : RetroGfx::kBlack, label);

        if (clk) {
            if (appId) compositor.windowManager().openApp(appId);
            s_startMenuOpen = false;
        }

        itemY += itemH + 2.0f;
    };

    renderMenuItem("Files (Explorer)", "files", RetroGfx::drawFolderIcon);
    renderMenuItem("Settings", "settings", RetroGfx::drawSettingsIcon);
    renderMenuItem("Task Manager", "taskmanager", RetroGfx::drawTaskManagerIcon);

    // Separator line
    itemY += 4.0f;
    dl->AddLine(ImVec2(itemX, itemY), ImVec2(itemX + itemW, itemY), RetroGfx::kGrayShadow);
    dl->AddLine(ImVec2(itemX, itemY + 1.0f), ImVec2(itemX + itemW, itemY + 1.0f), RetroGfx::kWhite);
    itemY += 6.0f;

    // Shut down option
    const ImVec2 miMin(itemX, itemY);
    const ImVec2 miMax(itemX + itemW, itemY + itemH);
    ImGui::SetCursorScreenPos(miMin);
    ImGui::InvisibleButton("##ShutdownMenu", ImVec2(itemW, itemH));
    const bool sHov = ImGui::IsItemHovered();
    const bool sClk = ImGui::IsItemClicked();

    if (sHov) dl->AddRectFilled(miMin, miMax, RetroGfx::kNavyActive);
    const ImVec2 pCenter(miMin.x + 18.0f, miMin.y + itemH * 0.5f);
    RetroGfx::drawPowerIcon(dl, pCenter, 20.0f * uiScale);

    const float sTextY = miMin.y + (itemH - ImGui::GetFontSize()) * 0.5f;
    dl->AddText(ImVec2(miMin.x + 38.0f, sTextY), sHov ? RetroGfx::kWhite : RetroGfx::kBlack, "Shut Down...");

    if (sClk) {
        s_startMenuOpen = false;
        compositor.requestShutdown();
    }

    ImGui::End();

    // Close on click outside
    if (ImGui::IsMouseClicked(0) && !ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow)) {
        // Will close on next frame
    }
}

} // namespace

void TaskBar::draw(Compositor& compositor) {
    const ImVec2 view = compositor.viewportSize();
    const float  barH = compositor.taskbarHeight();
    const bool   top  = compositor.config().taskbarAtTop;
    const ImVec2 origin = top ? ImVec2(0.0f, 0.0f) : ImVec2(0.0f, view.y - barH);
    const float  uiScale = compositor.config().uiScale;

    const ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoNavFocus;

    ImGui::SetNextWindowPos(origin);
    ImGui::SetNextWindowSize(ImVec2(view.x, barH));
    ImGui::SetNextWindowBgAlpha(0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("##ClassicTaskBar", nullptr, flags);
    ImGui::PopStyleVar();

    ImDrawList* dl = ImGui::GetWindowDrawList();
    const ImVec2 pMin = ImGui::GetWindowPos();
    const ImVec2 pMax(pMin.x + view.x, pMin.y + barH);

    // ── 1. Classic Taskbar Background & Raised 3D Shelf ──
    dl->AddRectFilled(pMin, pMax, RetroGfx::kGrayFace);
    if (top) {
        // Bottom border if top-docked
        dl->AddLine(ImVec2(pMin.x, pMax.y - 1.0f), ImVec2(pMax.x, pMax.y - 1.0f), RetroGfx::kGrayShadow, 1.0f);
        dl->AddLine(ImVec2(pMin.x, pMax.y), ImVec2(pMax.x, pMax.y), RetroGfx::kBlack, 1.0f);
    } else {
        // Top border if bottom-docked: White highlight on top, giving 3D raised shelf
        dl->AddLine(ImVec2(pMin.x, pMin.y), ImVec2(pMax.x, pMin.y), RetroGfx::kWhite, 1.5f);
        dl->AddLine(ImVec2(pMin.x, pMin.y + 1.0f), ImVec2(pMax.x, pMin.y + 1.0f), RetroGfx::kGrayLight, 1.0f);
    }

    const float padY = 4.0f;
    const float btnH = barH - padY * 2.0f;
    ImVec2 cursor(pMin.x + 4.0f, pMin.y + padY);

    // ── 2. The Classic "Start" Button ──
    const float startW = 86.0f * uiScale;
    const ImVec2 startMin = cursor;
    const ImVec2 startMax(cursor.x + startW, cursor.y + btnH);

    ImGui::SetCursorScreenPos(startMin);
    ImGui::InvisibleButton("##StartButton", ImVec2(startW, btnH));
    const bool startPressed = ImGui::IsItemActive() || s_startMenuOpen;
    const bool startClicked = ImGui::IsItemClicked();

    if (startClicked) {
        s_startMenuOpen = !s_startMenuOpen;
    }

    // 3D Beveled Start button (sunken when open/active)
    RetroGfx::draw3DBox(dl, startMin, startMax, startPressed, RetroGfx::kGrayFace);

    // Windows 4-color Logo
    const ImVec2 logoCenter(startMin.x + 18.0f * uiScale, startMin.y + btnH * 0.5f);
    RetroGfx::drawWindowsFlag(dl, logoCenter, 18.0f * uiScale);

    // Bold "Start" Text
    const ImVec2 sSize = ImGui::CalcTextSize("Start");
    const ImVec2 sTextPos(logoCenter.x + 14.0f * uiScale, startMin.y + (btnH - sSize.y) * 0.5f);
    dl->AddText(sTextPos, RetroGfx::kBlack, "Start");

    cursor.x += startW + 6.0f;

    // ── 3. Quick Launch Separator & Icons ──
    // Sunken vertical bar divider
    dl->AddLine(ImVec2(cursor.x, pMin.y + 4.0f), ImVec2(cursor.x, pMax.y - 4.0f), RetroGfx::kGrayShadow);
    dl->AddLine(ImVec2(cursor.x + 1.0f, pMin.y + 4.0f), ImVec2(cursor.x + 1.0f, pMax.y - 4.0f), RetroGfx::kWhite);
    cursor.x += 6.0f;

    const float qIconSize = btnH;
    const float graphicSize = 22.0f * uiScale;

    if (drawTaskBarLauncher("##qlFiles", "Files (Explorer)", "files", compositor, cursor, qIconSize, graphicSize, RetroGfx::drawFolderIcon)) {
        compositor.windowManager().toggleApp("files");
    }
    if (drawTaskBarLauncher("##qlSettings", "Settings", "settings", compositor, cursor, qIconSize, graphicSize, RetroGfx::drawSettingsIcon)) {
        compositor.windowManager().toggleApp("settings");
    }
    if (drawTaskBarLauncher("##qlTaskMgr", "Task Manager", "taskmanager", compositor, cursor, qIconSize, graphicSize, RetroGfx::drawTaskManagerIcon)) {
        compositor.windowManager().toggleApp("taskmanager");
    }

    // Divider after Quick Launch
    cursor.x += 2.0f;
    dl->AddLine(ImVec2(cursor.x, pMin.y + 4.0f), ImVec2(cursor.x, pMax.y - 4.0f), RetroGfx::kGrayShadow);
    dl->AddLine(ImVec2(cursor.x + 1.0f, pMin.y + 4.0f), ImVec2(cursor.x + 1.0f, pMax.y - 4.0f), RetroGfx::kWhite);
    cursor.x += 8.0f;

    // ── 4. System Tray (Notification Area on the Right) ──
    const float trayW = 180.0f * uiScale;
    const ImVec2 trayMin(pMax.x - trayW - 6.0f, pMin.y + padY);
    const ImVec2 trayMax(pMax.x - 6.0f, pMin.y + padY + btnH);

    // Sunken 3D border for notification tray
    RetroGfx::drawSunkenBorder(dl, trayMin, trayMax, RetroGfx::kGrayFace);

    // Tray icons: Speaker and Network
    const float trayCenterY = trayMin.y + btnH * 0.5f;
    RetroGfx::drawSpeakerIcon(dl, ImVec2(trayMin.x + 18.0f, trayCenterY), 18.0f * uiScale);
    RetroGfx::drawNetworkIcon(dl, ImVec2(trayMin.x + 40.0f, trayCenterY), 18.0f * uiScale);

    // Tray Shutdown button
    const float pwrBtnSize = btnH - 6.0f;
    const ImVec2 trayPwrMin(trayMin.x + 58.0f * uiScale, trayMin.y + 3.0f);
    ImGui::SetCursorScreenPos(trayPwrMin);
    const bool trayPwrClicked = ImGui::InvisibleButton("##TrayShutdown", ImVec2(pwrBtnSize, pwrBtnSize));
    const bool trayPwrHov = ImGui::IsItemHovered();
    RetroGfx::drawPowerIcon(dl, ImVec2(trayPwrMin.x + pwrBtnSize * 0.5f, trayPwrMin.y + pwrBtnSize * 0.5f), 15.0f * uiScale);
    if (trayPwrHov) {
        ImGui::SetTooltip("Shut Down CSOPESY OS");
    }
    if (trayPwrClicked) {
        compositor.requestShutdown();
    }

    // Tray Clock
    const std::time_t now = std::time(nullptr);
    std::tm local {};
    localtime_s(&local, &now);
    char trayTime[32];
    std::strftime(trayTime, sizeof(trayTime), "%I:%M %p", &local);
    const char* tt = (trayTime[0] == '0') ? trayTime + 1 : trayTime;

    const ImVec2 ttSize = ImGui::CalcTextSize(tt);
    const float ttX = trayMax.x - ttSize.x - 8.0f;
    const float ttY = trayMin.y + (btnH - ttSize.y) * 0.5f;
    dl->AddText(ImVec2(ttX, ttY), RetroGfx::kBlack, tt);

    // ── 5. Running Application Taskbar Tabs (Windows 95/98 style) ──
    const float maxTabArea = trayMin.x - cursor.x - 10.0f;
    const auto& entries = compositor.windowManager().entries();

    int openCount = 0;
    for (const auto& e : entries) {
        if (e.open) openCount++;
    }

    if (openCount > 0) {
        const float tabW = std::min(170.0f * uiScale, (maxTabArea - (openCount - 1) * 4.0f) / openCount);

        for (const auto& e : entries) {
            if (!e.open) continue;

            const ImVec2 tabMin = cursor;
            const ImVec2 tabMax(cursor.x + tabW, cursor.y + btnH);

            ImGui::SetCursorScreenPos(tabMin);
            ImGui::InvisibleButton(("##tab" + e.app->id()).c_str(), ImVec2(tabW, btnH));
            const bool tabClicked = ImGui::IsItemClicked();
            const bool isFocused = compositor.windowManager().isAppFocused(e.app->id());

            // Focused window tab has sunken bevel, unfocused has raised bevel
            RetroGfx::draw3DBox(dl, tabMin, tabMax, isFocused, isFocused ? RetroGfx::kGrayLight : RetroGfx::kGrayFace);

            // App Icon
            const ImVec2 aCenter(tabMin.x + 16.0f, tabMin.y + btnH * 0.5f);
            e.app->drawIcon(dl, aCenter, 18.0f * uiScale);

            // App Title truncated with clip
            ImGui::PushClipRect(ImVec2(tabMin.x + 28.0f, tabMin.y), ImVec2(tabMax.x - 4.0f, tabMax.y), true);
            const float titleY = tabMin.y + (btnH - ImGui::GetFontSize()) * 0.5f;
            dl->AddText(ImVec2(tabMin.x + 28.0f, titleY), RetroGfx::kBlack, e.app->title().c_str());
            ImGui::PopClipRect();

            if (tabClicked) {
                if (isFocused) {
                    // Minimize or keep
                } else {
                    compositor.windowManager().setFocusedApp(e.app->id());
                }
            }

            cursor.x += tabW + 4.0f;
        }
    }

    ImGui::End();

    // Draw Start Menu if open
    drawStartMenu(compositor, pMin, barH);
}
