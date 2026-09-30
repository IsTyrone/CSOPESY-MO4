#include "Desktop.h"
#include "Compositor.h"

#include <cmath>
#include <ctime>

namespace {

// ── Wallpaper palette ───────────────────────────────────────────
const ImU32 kWallTop     = IM_COL32(24, 34, 58, 255);
const ImU32 kWallBottom  = IM_COL32(12, 16, 28, 255);
const ImU32 kWallAccentA = IM_COL32(58, 110, 190, 70);
const ImU32 kWallAccentB = IM_COL32(120, 78, 190, 55);
const ImU32 kClockColor  = IM_COL32(232, 240, 255, 255);
const ImU32 kTextShadow  = IM_COL32(0, 0, 0, 170);
const ImU32 kPwrIdle     = IM_COL32(46, 54, 70, 235);
const ImU32 kPwrHover    = IM_COL32(178, 56, 56, 245);

void drawGradient(ImDrawList* dl, const ImVec2& size) {
    dl->AddRectFilledMultiColor(ImVec2(0.0f, 0.0f), size, kWallTop, kWallTop, kWallBottom, kWallBottom);
}

void drawPattern(ImDrawList* dl, const ImVec2& size) {
    // Diagonal hairlines — a "pattern drawn using ImGui draw commands".
    const float step = 46.0f;
    for (float x = -size.y; x < size.x; x += step)
        dl->AddLine(ImVec2(x, 0.0f), ImVec2(x + size.y, size.y), IM_COL32(255, 255, 255, 9), 1.0f);

    // Two soft blooms for depth.
    dl->AddCircleFilled(ImVec2(size.x * 0.18f, size.y * 0.26f), size.x * 0.20f, kWallAccentA);
    dl->AddCircleFilled(ImVec2(size.x * 0.82f, size.y * 0.72f), size.x * 0.24f, kWallAccentB);
}

// Power glyph: an open ring plus the vertical stroke through the gap.
void drawPowerGlyph(ImDrawList* dl, const ImVec2& centre, float radius, ImU32 col) {
    const int   segments = 32;
    const float gapHalf  = 0.42f;                        // radians of missing arc
    const float start    = -1.5707963f + gapHalf;        // just right of top-centre
    for (int i = 0; i < segments; ++i) {
        const float a0 = start + (2.0f * 3.14159265f - 2.0f * gapHalf) * (static_cast<float>(i) / segments);
        const float a1 = start + (2.0f * 3.14159265f - 2.0f * gapHalf) * (static_cast<float>(i + 1) / segments);
        dl->AddLine(ImVec2(centre.x + std::cos(a0) * radius, centre.y + std::sin(a0) * radius),
                    ImVec2(centre.x + std::cos(a1) * radius, centre.y + std::sin(a1) * radius),
                    col, 1.8f);
    }
    dl->AddLine(ImVec2(centre.x, centre.y - radius - 3.5f),
                ImVec2(centre.x, centre.y - 1.0f), col, 1.8f);
}

} // namespace

void Desktop::draw(Compositor& compositor, const ImVec2& areaMin, const ImVec2& areaMax) {
    const ImVec2 view = compositor.viewportSize();

    // ── Wallpaper: the very first thing submitted each frame ──
    ImDrawList* bg = ImGui::GetBackgroundDrawList();
    const std::string& mode = compositor.config().wallpaperMode;
    if (mode == "plain") {
        bg->AddRectFilled(ImVec2(0.0f, 0.0f), view, kWallBottom);
    } else {
        drawGradient(bg, view);
        if (mode == "gradient" || mode == "pattern") drawPattern(bg, view);
    }

    // ── This layer's own full-screen window carries the clock and PWR ──
    const ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoNavFocus |
        ImGuiWindowFlags_NoInputs;   // prevent this full-screen layer from stealing hover

    ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
    ImGui::SetNextWindowSize(view);
    ImGui::SetNextWindowBgAlpha(0.0f);
    ImGui::Begin("##DesktopLayer", nullptr, flags);

    ImDrawList* fg = ImGui::GetWindowDrawList();
    const float fontH = ImGui::GetFontSize();

    // ── Real-time clock: rebuilt from the system clock every frame ──
    const std::time_t now = std::time(nullptr);
    std::tm local {};
    localtime_s(&local, &now);

    char clock[32];
    std::strftime(clock, sizeof(clock), "%I:%M:%S  %p", &local);
    const char* clockText = (clock[0] == '0') ? clock + 1 : clock;   // drop leading zero

    const ImVec2 clockPos(areaMin.x + kClockX, areaMin.y + kClockY);
    fg->AddText(ImVec2(clockPos.x + 1.0f, clockPos.y + 1.0f), kTextShadow, clockText);
    fg->AddText(clockPos, kClockColor, clockText);

    char date[48];
    std::strftime(date, sizeof(date), "%A, %d %B %Y", &local);
    const ImVec2 datePos(clockPos.x, clockPos.y + fontH * 1.2f);
    fg->AddText(ImVec2(datePos.x + 1.0f, datePos.y + 1.0f), kTextShadow, date);
    fg->AddText(datePos, IM_COL32(178, 194, 224, 255), date);

    ImGui::End();   // end ##DesktopLayer (NoInputs — clock only, no hit-testing)
}

void Desktop::drawPwr(Compositor& compositor, const ImVec2& areaMin, const ImVec2& areaMax) {
    // ── PWR button in its own small window so it can receive input ──
    // Drawn LAST each frame so it sits on top of the z-order.
    const ImVec2 pwrOrigin(areaMax.x - kPwrMargin - kPwrW, areaMax.y - kPwrMargin - kPwrH);
    const ImVec2 pwrSize(kPwrW, kPwrH);

    const ImGuiWindowFlags pwrFlags =
        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoNavFocus;

    ImGui::SetNextWindowPos(pwrOrigin);
    ImGui::SetNextWindowSize(pwrSize);
    ImGui::SetNextWindowBgAlpha(0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("##PwrLayer", nullptr, pwrFlags);
    ImGui::PopStyleVar();

    const ImVec2 pwrMin = ImGui::GetWindowPos();
    const ImVec2 pwrMax(pwrMin.x + kPwrW, pwrMin.y + kPwrH);

    ImGui::SetCursorScreenPos(pwrMin);
    ImGui::InvisibleButton("##PwrButton", pwrSize);
    const bool hovered = ImGui::IsItemHovered();
    const bool clicked  = ImGui::IsItemClicked();

    ImDrawList* pwrDl = ImGui::GetWindowDrawList();
    pwrDl->AddRectFilled(pwrMin, pwrMax, hovered ? kPwrHover : kPwrIdle, 6.0f);
    pwrDl->AddRect(pwrMin, pwrMax, hovered ? IM_COL32(255, 190, 190, 255) : IM_COL32(226, 96, 96, 220),
                6.0f, 0, hovered ? 1.4f : 1.0f);

    const ImVec2 glyphCentre(pwrMin.x + 32.0f, (pwrMin.y + pwrMax.y) * 0.5f);
    drawPowerGlyph(pwrDl, glyphCentre, 8.0f, IM_COL32(240, 216, 216, 255));

    const ImVec2 labelSize = ImGui::CalcTextSize("PWR");
    pwrDl->AddText(ImVec2(pwrMin.x + 54.0f, glyphCentre.y - labelSize.y * 0.5f),
                IM_COL32(244, 226, 226, 255), "PWR");

    if (hovered) ImGui::SetTooltip("Shut down CSOPESY  (Requirement A)");

    // Requirement A: this is the one and only shutdown path.
    if (clicked) compositor.requestShutdown();

    ImGui::End();   // end ##PwrLayer
}

