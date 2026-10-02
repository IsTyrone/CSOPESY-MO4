/*
 *  CSOPESY Semi-Major Output 2 / MO4 — Desktop-Style OS Mock-up
 *  Desktop — Implementation with Windows Classic styling, Desktop Icons, Clock, and PWR button
 */

#include "Desktop.h"
#include "Compositor.h"
#include "RetroGfx.h"
#include <ctime>
#include <algorithm>

namespace {

void drawWallpaper(ImDrawList* dl, const ImVec2& view, const std::string& mode) {
    if (mode == "classic-teal" || mode.empty()) {
        // Authentic Windows 95/98 Teal: #008080
        dl->AddRectFilled(ImVec2(0.0f, 0.0f), view, RetroGfx::kTealDesktop);
    } else if (mode == "gradient") {
        // Windows 2000 / Setup gradient (Deep Navy to Slate Blue)
        const ImU32 topCol = IM_COL32(0, 0, 128, 255);
        const ImU32 botCol = IM_COL32(16, 72, 138, 255);
        dl->AddRectFilledMultiColor(ImVec2(0.0f, 0.0f), view, topCol, topCol, botCol, botCol);
    } else if (mode == "bliss") {
        // Windows XP Bliss-inspired Sky and Hill
        const float skyH = view.y * 0.62f;
        dl->AddRectFilledMultiColor(ImVec2(0.0f, 0.0f), ImVec2(view.x, skyH),
                                    IM_COL32(32, 120, 218, 255), IM_COL32(32, 120, 218, 255),
                                    IM_COL32(160, 212, 255, 255), IM_COL32(160, 212, 255, 255));
        dl->AddRectFilledMultiColor(ImVec2(0.0f, skyH), view,
                                    IM_COL32(64, 168, 52, 255), IM_COL32(64, 168, 52, 255),
                                    IM_COL32(34, 112, 26, 255), IM_COL32(34, 112, 26, 255));
    } else if (mode == "pattern") {
        dl->AddRectFilled(ImVec2(0.0f, 0.0f), view, RetroGfx::kTealDesktop);
        for (float y = 0.0f; y < view.y; y += 4.0f) {
            dl->AddLine(ImVec2(0.0f, y), ImVec2(view.x, y), IM_COL32(0, 0, 0, 18), 1.0f);
        }
    } else {
        // Plain dark charcoal
        dl->AddRectFilled(ImVec2(0.0f, 0.0f), view, IM_COL32(24, 28, 36, 255));
    }
}

// Draw desktop clock directly on background draw list so it never steals mouse input
void drawDesktopClock(ImDrawList* dl, const ImVec2& areaMin, const ImVec2& areaMax) {
    const std::time_t now = std::time(nullptr);
    std::tm local {};
    localtime_s(&local, &now);

    char clockStr[32];
    std::strftime(clockStr, sizeof(clockStr), "%I:%M:%S %p", &local);
    const char* clockText = (clockStr[0] == '0') ? clockStr + 1 : clockStr;

    char dateStr[48];
    std::strftime(dateStr, sizeof(dateStr), "%A, %B %d, %Y", &local);

    const float clockX = areaMax.x - 270.0f;
    const float clockY = areaMin.y + 20.0f;
    const ImVec2 clockMin(clockX, clockY);
    const ImVec2 clockMax(areaMax.x - 20.0f, clockY + 68.0f);

    // Beveled frame for desktop clock widget
    RetroGfx::draw3DBox(dl, clockMin, clockMax, false, RetroGfx::kGrayFace);

    // Sunken inner display screen (LCD/LED feel)
    const ImVec2 lcdMin(clockMin.x + 4.0f, clockMin.y + 4.0f);
    const ImVec2 lcdMax(clockMax.x - 4.0f, clockMax.y - 4.0f);
    RetroGfx::drawSunkenBorder(dl, lcdMin, lcdMax, IM_COL32(10, 24, 18, 255));

    // Green LCD Clock Text
    const ImVec2 cSize = ImGui::CalcTextSize(clockText);
    dl->AddText(ImVec2(lcdMin.x + (lcdMax.x - lcdMin.x - cSize.x) * 0.5f, lcdMin.y + 8.0f),
                IM_COL32(0, 255, 120, 255), clockText);

    const ImVec2 dSize = ImGui::CalcTextSize(dateStr);
    dl->AddText(ImVec2(lcdMin.x + (lcdMax.x - lcdMin.x - dSize.x) * 0.5f, lcdMin.y + 32.0f),
                IM_COL32(140, 210, 180, 255), dateStr);
}

// Single desktop shortcut icon button
void drawDesktopShortcut(const char* label, const char* appId, Compositor& compositor,
                         float boxW, float iconGraphicSize,
                         void (*iconDrawer)(ImDrawList*, const ImVec2&, float)) {
    const ImVec2 cur = ImGui::GetCursorScreenPos();
    const ImVec2 boxSize(boxW, boxW + 20.0f);
    const ImVec2 pMax(cur.x + boxSize.x, cur.y + boxSize.y);

    ImGui::InvisibleButton(label, boxSize);

    const bool hovered = ImGui::IsItemHovered();
    const bool clicked = ImGui::IsItemClicked(0);
    const bool open    = appId ? compositor.windowManager().isAppOpen(appId) : false;

    ImDrawList* dl = ImGui::GetWindowDrawList();

    if (hovered || open) {
        // Windows 95/98 dotted or highlighted selection rectangle
        dl->AddRectFilled(cur, pMax, IM_COL32(0, 0, 128, open ? 140 : 80), 2.0f);
        dl->AddRect(cur, pMax, IM_COL32(255, 255, 255, 160), 2.0f, 0, 1.0f);
    }

    // Icon graphic in top half
    const ImVec2 iconCenter(cur.x + boxW * 0.5f, cur.y + boxW * 0.5f - 2.0f);
    iconDrawer(dl, iconCenter, iconGraphicSize);

    // Label text centered underneath icon
    const ImVec2 textSize = ImGui::CalcTextSize(label);
    const ImVec2 textPos(cur.x + (boxW - textSize.x) * 0.5f, cur.y + boxW + 2.0f);

    dl->AddText(ImVec2(textPos.x + 1.0f, textPos.y + 1.0f), IM_COL32(0, 0, 0, 220), label);
    dl->AddText(textPos, RetroGfx::kWhite, label);

    if (clicked && appId) {
        compositor.windowManager().toggleApp(appId);
    }

    ImGui::Spacing();
}

} // namespace

void Desktop::draw(Compositor& compositor, const ImVec2& areaMin, const ImVec2& areaMax) {
    const ImVec2 view = compositor.viewportSize();

    // ── 1. Wallpaper Layer (Lowest layer) ──
    ImDrawList* bg = ImGui::GetBackgroundDrawList();
    drawWallpaper(bg, view, compositor.config().wallpaperMode);

    // ── 2. Desktop Digital Clock (Drawn to background draw list — zero input blocking) ──
    drawDesktopClock(bg, areaMin, areaMax);

    // ── 3. Desktop Shortcut Icons (Constrained to left sidebar — never covers full screen) ──
    if (compositor.config().showDesktopIcons) {
        const float uiScale = compositor.config().uiScale;
        const float boxW = 86.0f * uiScale;
        const float iconGraphicSize = 44.0f * uiScale;
        const float shortcutsW = boxW + 20.0f;
        const float shortcutsH = (boxW + 36.0f) * 3.5f;

        const ImGuiWindowFlags scFlags =
            ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus |
            ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoNavFocus;

        ImGui::SetNextWindowPos(ImVec2(areaMin.x + 16.0f, areaMin.y + 16.0f));
        ImGui::SetNextWindowSize(ImVec2(shortcutsW, shortcutsH));
        ImGui::SetNextWindowBgAlpha(0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(4.0f, 4.0f));
        ImGui::Begin("##DesktopShortcuts", nullptr, scFlags);
        ImGui::PopStyleVar();

        drawDesktopShortcut("Files", "files", compositor, boxW, iconGraphicSize, RetroGfx::drawFolderIcon);
        drawDesktopShortcut("Settings", "settings", compositor, boxW, iconGraphicSize, RetroGfx::drawSettingsIcon);
        drawDesktopShortcut("Task Manager", "taskmanager", compositor, boxW, iconGraphicSize, RetroGfx::drawTaskManagerIcon);

        ImGui::End();
    }
}

void Desktop::drawPwr(Compositor& compositor, const ImVec2& /*areaMin*/, const ImVec2& areaMax) {
    const float uiScale = compositor.config().uiScale;
    const float pwrW = 142.0f * uiScale;
    const float pwrH = 44.0f * uiScale;
    const float margin = 20.0f;

    // Position PWR button at bottom-right of desktop area (just above taskbar)
    const ImVec2 pwrOrigin(areaMax.x - margin - pwrW, areaMax.y - margin - pwrH);

    const ImGuiWindowFlags pwrFlags =
        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoSavedSettings;

    ImGui::SetNextWindowPos(pwrOrigin);
    ImGui::SetNextWindowSize(ImVec2(pwrW, pwrH));
    ImGui::SetNextWindowBgAlpha(0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("##PwrLayer", nullptr, pwrFlags);
    ImGui::PopStyleVar();

    const ImVec2 pwrMin = ImGui::GetWindowPos();
    const ImVec2 pwrMax(pwrMin.x + pwrW, pwrMin.y + pwrH);

    ImGui::SetCursorPos(ImVec2(0.0f, 0.0f));
    const bool clickedBtn = ImGui::InvisibleButton("##PwrButton", ImVec2(pwrW, pwrH));
    const bool hovered = ImGui::IsItemHovered();
    const bool pressed = ImGui::IsItemActive();
    const bool clicked = clickedBtn || (hovered && ImGui::IsMouseClicked(0));

    ImDrawList* dl = ImGui::GetWindowDrawList();

    // Classic 3D Beveled Button
    RetroGfx::draw3DBox(dl, pwrMin, pwrMax, pressed, RetroGfx::kGrayFace);

    // Power Icon and Text
    const float iconSize = 22.0f * uiScale;
    const ImVec2 iconCenter(pwrMin.x + 24.0f * uiScale, (pwrMin.y + pwrMax.y) * 0.5f);
    RetroGfx::drawPowerIcon(dl, iconCenter, iconSize);

    const ImVec2 labelSize = ImGui::CalcTextSize("Shut Down");
    const ImVec2 textPos(iconCenter.x + iconSize * 0.8f, (pwrMin.y + pwrMax.y - labelSize.y) * 0.5f);
    dl->AddText(textPos, RetroGfx::kBlack, "Shut Down");

    if (hovered) {
        ImGui::SetTooltip("Shut down the CSOPESY OS Mockup (Requirement A: Clean exit path)");
    }

    if (clicked) {
        compositor.requestShutdown();
    }

    ImGui::End();
}
