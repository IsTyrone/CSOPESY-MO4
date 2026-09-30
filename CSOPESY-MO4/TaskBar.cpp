#include "TaskBar.h"
#include "Compositor.h"

#include <cmath>
#include <cstdio>


namespace {

const ImU32 kBarBg        = IM_COL32(16, 19, 26, 242);
const ImU32 kBarBorder    = IM_COL32(46, 56, 76, 255);
const ImU32 kIconIdle     = IM_COL32(178, 192, 214, 255);
const ImU32 kIconHover    = IM_COL32(120, 186, 255, 255);
const ImU32 kIconOpen     = IM_COL32(96, 220, 170, 255);
const ImU32 kChipBg       = IM_COL32(32, 39, 52, 235);
const ImU32 kChipBgActive = IM_COL32(44, 78, 132, 255);

// ── Icon painters (Requirement B: clickable *icon* buttons) ──
void drawFolderIcon(ImDrawList* dl, const ImVec2& c, float s, ImU32 col) {
    const float w = s * 0.92f, h = s * 0.68f;
    const ImVec2 tl(c.x - w * 0.5f, c.y - h * 0.5f);
    // Tab
    dl->AddRectFilled(ImVec2(tl.x, tl.y), ImVec2(tl.x + w * 0.42f, tl.y + h * 0.28f), col, 2.0f);
    // Body
    dl->AddRectFilled(ImVec2(tl.x, tl.y + h * 0.20f), ImVec2(tl.x + w, tl.y + h), col, 2.0f);
}

void drawGearIcon(ImDrawList* dl, const ImVec2& c, float s, ImU32 col) {
    const float r = s * 0.34f;
    for (int i = 0; i < 8; ++i) {
        const float a = i * 3.14159265f / 4.0f;
        const ImVec2 d(std::cos(a), std::sin(a));
        dl->AddLine(ImVec2(c.x + d.x * r * 1.05f, c.y + d.y * r * 1.05f),
                    ImVec2(c.x + d.x * r * 1.62f, c.y + d.y * r * 1.62f), col, 2.0f);
    }
    dl->AddCircleFilled(c, r, col, 20);
    dl->AddCircleFilled(c, r * 0.42f, IM_COL32(16, 19, 26, 255), 16);
}

void drawChartIcon(ImDrawList* dl, const ImVec2& c, float s, ImU32 col) {
    const float w = s * 0.17f, gap = s * 0.10f;
    const float base = c.y + s * 0.32f;
    const float hs[3] = { s * 0.30f, s * 0.52f, s * 0.68f };
    for (int i = 0; i < 3; ++i) {
        const float x = c.x - s * 0.34f + static_cast<float>(i) * (w + gap);
        dl->AddRectFilled(ImVec2(x, base - hs[i]), ImVec2(x + w, base), col, 1.5f);
    }
}

// A launcher button: invisible hit area, then a vector icon on top.
bool launcherButton(const char* id, ImVec2& cursor, const char* tooltip,
                    bool open, void (*icon)(ImDrawList*, const ImVec2&, float, ImU32)) {
    ImGui::SetCursorScreenPos(cursor);
    ImGui::InvisibleButton(id, ImVec2(TaskBar::kIconSize, TaskBar::kIconSize));

    ImDrawList* dl = ImGui::GetWindowDrawList();
    const ImVec2 pMin = ImGui::GetItemRectMin();
    const ImVec2 pMax = ImGui::GetItemRectMax();
    const bool  hov  = ImGui::IsItemHovered();
    const bool  clk  = ImGui::IsItemClicked();


    if (hov || open) {
        dl->AddRectFilled(pMin, pMax,
                          open ? IM_COL32(30, 58, 52, 235) : IM_COL32(36, 48, 68, 235), 6.0f);
        dl->AddRect(pMin, pMax, open ? IM_COL32(64, 150, 118, 200) : IM_COL32(72, 96, 136, 220),
                    6.0f, 0, 1.0f);
    }

    const ImU32 col = open ? kIconOpen : (hov ? kIconHover : kIconIdle);
    const ImVec2 centre((pMin.x + pMax.x) * 0.5f, (pMin.y + pMax.y) * 0.5f);
    icon(dl, centre, TaskBar::kIconSize, col);

    // Open indicator pip.
    if (open)
        dl->AddCircleFilled(ImVec2(centre.x, pMax.y - 3.0f), 2.0f, kIconOpen, 8);

    if (hov) ImGui::SetTooltip("%s%s", tooltip, open ? "  (running)" : "");

    cursor.x += TaskBar::kIconSize + TaskBar::kIconGap;
    return clk;
}

} // namespace

void TaskBar::draw(Compositor& compositor) {
    const ImVec2 view = compositor.viewportSize();
    const float  barH = compositor.taskbarHeight();
    const ImVec2 origin = compositor.config().taskbarAtTop ? ImVec2(0.0f, 0.0f)
                                                           : ImVec2(0.0f, view.y - barH);

    const ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoNavFocus;

    ImGui::SetNextWindowPos(origin);
    ImGui::SetNextWindowSize(ImVec2(view.x, barH));
    ImGui::SetNextWindowBgAlpha(0.0f);
    ImGui::Begin("##TaskBar", nullptr, flags);

    ImDrawList* dl = ImGui::GetWindowDrawList();
    const ImVec2 pMin = ImGui::GetWindowPos();
    const ImVec2 pMax(pMin.x + view.x, pMin.y + barH);
    dl->AddRectFilled(pMin, pMax, kBarBg, 0.0f);
    dl->AddLine(pMin, ImVec2(pMax.x, pMin.y), kBarBorder, 1.0f);
    dl->AddLine(ImVec2(pMin.x, pMax.y - 1.0f), ImVec2(pMax.x, pMax.y - 1.0f), kBarBorder, 1.0f);

    const float padY = (barH - kIconSize) * 0.5f;
    ImVec2 cursor(pMin.x + kIconPadX, pMin.y + padY);

    // ── The three icon buttons ──
    const bool openFiles = compositor.isAppOpen(Compositor::appFiles());
    const bool openSet   = compositor.isAppOpen(Compositor::appSettings());
    const bool openTm    = compositor.isAppOpen(Compositor::appTaskManager());

    if (launcherButton("##btnFiles", cursor, "Files", openFiles, drawFolderIcon))
        compositor.setAppOpen(Compositor::appFiles(), !openFiles);

    if (launcherButton("##btnSettings", cursor, "Settings", openSet, drawGearIcon))
        compositor.setAppOpen(Compositor::appSettings(), !openSet);

    if (launcherButton("##btnTaskManager", cursor, "Task Manager", openTm, drawChartIcon))
        compositor.setAppOpen(Compositor::appTaskManager(), !openTm);

    // ── Divider, then the running-applications strip ──
    const float divX = cursor.x + 4.0f;
    dl->AddLine(ImVec2(divX, pMin.y + barH * 0.22f), ImVec2(divX, pMin.y + barH * 0.78f),
                IM_COL32(58, 70, 92, 255), 1.0f);

    float x = divX + 14.0f;
    const float textH = ImGui::GetTextLineHeight();
    const float cy = pMin.y + (barH - textH) * 0.5f;

    const ProcessSimulator& sim = compositor.simulator();
    char status[96];
    std::snprintf(status, sizeof(status), "%d running  |  CPU %.0f%%  |  RAM %d/%d MB",
                  sim.runningCount(), static_cast<double>(sim.totalCpuPercent()),
                  sim.usedMemoryKb() / 1024, sim.totalMemoryKb() / 1024);
    dl->AddText(ImVec2(x, cy), IM_COL32(150, 166, 192, 255), status);
    x += ImGui::CalcTextSize(status).x + 22.0f;

    // One clickable chip per open application window.
    for (const std::string& title : compositor.openAppTitles()) {
        const ImVec2 ts = ImGui::CalcTextSize(title.c_str());
        const ImVec2 chipMin(x, pMin.y + padY);
        const ImVec2 chipMax(x + ts.x + kChipPadX * 2.0f, pMin.y + padY + kIconSize);

        ImGui::SetCursorScreenPos(chipMin);
        ImGui::InvisibleButton(("##chip" + title).c_str(),
                               ImVec2(chipMax.x - chipMin.x, chipMax.y - chipMin.y));
        const bool hov = ImGui::IsItemHovered();
        const bool clk = ImGui::IsItemClicked();

        const bool focused = (compositor.focusedApp() == title);
        dl->AddRectFilled(chipMin, chipMax,
                          hov || focused ? kChipBgActive : kChipBg, 6.0f);
        dl->AddRect(chipMin, chipMax, focused ? IM_COL32(96, 152, 232, 220) : IM_COL32(64, 76, 98, 220),
                    6.0f, 0, 1.0f);
        dl->AddText(ImVec2(chipMin.x + kChipPadX, pMin.y + (barH - textH) * 0.5f),
                    IM_COL32(220, 230, 245, 255), title.c_str());

        if (hov) ImGui::SetTooltip("%s — click to bring to front", title.c_str());
        if (clk) compositor.setFocusedApp(title);

        x = chipMax.x + 8.0f;
        if (x > pMax.x - 200.0f) break;   // leave room for the tray
    }

    if (compositor.openAppCount() == 0) {
        dl->AddText(ImVec2(x, cy), IM_COL32(104, 116, 138, 255), "no applications running");
    }

    // ── System tray ──
    const std::string focused = compositor.focusedApp();
    const std::string trayText = focused.empty() ? std::string("CSOPESY SMO2  |  shell ready")
                                                 : ("focus: " + focused);
    const ImVec2 traySize = ImGui::CalcTextSize(trayText.c_str());
    dl->AddText(ImVec2(pMax.x - traySize.x - 16.0f, cy), IM_COL32(128, 142, 168, 255), trayText.c_str());

    ImGui::End();
}
