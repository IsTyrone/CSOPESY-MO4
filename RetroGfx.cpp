/*
 *  CSOPESY Semi-Major Output 2 / MO4 — Desktop-Style OS Mock-up
 *  RetroGfx — Windows Classic (95/98/2000) Visual Engine Implementation
 */

#include "RetroGfx.h"
#include <cmath>
#include <algorithm>

namespace RetroGfx {

void draw3DBox(ImDrawList* dl, const ImVec2& min, const ImVec2& max, bool sunken, ImU32 fillCol) {
    if (max.x <= min.x || max.y <= min.y) return;

    // Fill face
    dl->AddRectFilled(ImVec2(min.x + 2.0f, min.y + 2.0f), ImVec2(max.x - 2.0f, max.y - 2.0f), fillCol);

    const ImU32 cTopOuter    = sunken ? kBlack      : kWhite;
    const ImU32 cTopInner    = sunken ? kGrayShadow : kGrayLight;
    const ImU32 cBottomInner = sunken ? kGrayLight  : kGrayShadow;
    const ImU32 cBottomOuter = sunken ? kWhite      : kBlack;

    // Outer Top & Left
    dl->AddLine(ImVec2(min.x, min.y), ImVec2(max.x - 1.0f, min.y), cTopOuter, 1.0f);
    dl->AddLine(ImVec2(min.x, min.y), ImVec2(min.x, max.y - 1.0f), cTopOuter, 1.0f);

    // Inner Top & Left
    dl->AddLine(ImVec2(min.x + 1.0f, min.y + 1.0f), ImVec2(max.x - 2.0f, min.y + 1.0f), cTopInner, 1.0f);
    dl->AddLine(ImVec2(min.x + 1.0f, min.y + 1.0f), ImVec2(min.x + 1.0f, max.y - 2.0f), cTopInner, 1.0f);

    // Inner Bottom & Right
    dl->AddLine(ImVec2(min.x + 1.0f, max.y - 2.0f), ImVec2(max.x - 1.0f, max.y - 2.0f), cBottomInner, 1.0f);
    dl->AddLine(ImVec2(max.x - 2.0f, min.y + 1.0f), ImVec2(max.x - 2.0f, max.y - 1.0f), cBottomInner, 1.0f);

    // Outer Bottom & Right
    dl->AddLine(ImVec2(min.x, max.y - 1.0f), ImVec2(max.x, max.y - 1.0f), cBottomOuter, 1.0f);
    dl->AddLine(ImVec2(max.x - 1.0f, min.y), ImVec2(max.x - 1.0f, max.y), cBottomOuter, 1.0f);
}

void drawSunkenBorder(ImDrawList* dl, const ImVec2& min, const ImVec2& max, ImU32 fillCol) {
    if (max.x <= min.x || max.y <= min.y) return;

    dl->AddRectFilled(ImVec2(min.x + 1.0f, min.y + 1.0f), ImVec2(max.x - 1.0f, max.y - 1.0f), fillCol);
    // Dark top/left, white bottom/right
    dl->AddLine(ImVec2(min.x, min.y), ImVec2(max.x - 1.0f, min.y), kGrayShadow, 1.0f);
    dl->AddLine(ImVec2(min.x, min.y), ImVec2(min.x, max.y - 1.0f), kGrayShadow, 1.0f);
    dl->AddLine(ImVec2(min.x + 1.0f, max.y - 1.0f), ImVec2(max.x, max.y - 1.0f), kWhite, 1.0f);
    dl->AddLine(ImVec2(max.x - 1.0f, min.y + 1.0f), ImVec2(max.x - 1.0f, max.y), kWhite, 1.0f);
}

void drawClassicTitleBar(ImDrawList* dl, const ImVec2& min, const ImVec2& max,
                         const char* title, bool active) {
    if (active) {
        // Windows 98/2000 style gradient title bar: Navy left to blue right
        dl->AddRectFilledMultiColor(min, max, kNavyActive, kNavyActiveGrad, kNavyActiveGrad, kNavyActive);
    } else {
        dl->AddRectFilled(min, max, kNavyInactive);
    }

    if (title && title[0] != '\0') {
        const float textY = (min.y + max.y - ImGui::GetFontSize()) * 0.5f;
        dl->AddText(ImVec2(min.x + 6.0f, textY), kTitleText, title);
    }
}

void drawWindowsFlag(ImDrawList* dl, const ImVec2& center, float size) {
    const float h = size * 0.5f;
    const float w = size * 0.5f;
    const float gap = 2.0f;

    const ImVec2 tl(center.x - w * 0.5f, center.y - h * 0.5f);
    const float qw = (w - gap) * 0.5f;
    const float qh = (h - gap) * 0.5f;

    // 4 quadrants: Red (top-left), Green (top-right), Blue (bottom-left), Yellow (bottom-right)
    dl->AddRectFilled(ImVec2(tl.x, tl.y), ImVec2(tl.x + qw, tl.y + qh), IM_COL32(236, 52, 42, 255), 1.0f);
    dl->AddRectFilled(ImVec2(tl.x + qw + gap, tl.y), ImVec2(tl.x + w, tl.y + qh), IM_COL32(0, 168, 80, 255), 1.0f);
    dl->AddRectFilled(ImVec2(tl.x, tl.y + qh + gap), ImVec2(tl.x + qw, tl.y + h), IM_COL32(32, 112, 224, 255), 1.0f);
    dl->AddRectFilled(ImVec2(tl.x + qw + gap, tl.y + qh + gap), ImVec2(tl.x + w, tl.y + h), IM_COL32(252, 198, 28, 255), 1.0f);

    // Subtle dark outlines
    dl->AddRect(ImVec2(tl.x, tl.y), ImVec2(tl.x + qw, tl.y + qh), IM_COL32(40, 40, 40, 200), 1.0f);
    dl->AddRect(ImVec2(tl.x + qw + gap, tl.y), ImVec2(tl.x + w, tl.y + qh), IM_COL32(40, 40, 40, 200), 1.0f);
    dl->AddRect(ImVec2(tl.x, tl.y + qh + gap), ImVec2(tl.x + qw, tl.y + h), IM_COL32(40, 40, 40, 200), 1.0f);
    dl->AddRect(ImVec2(tl.x + qw + gap, tl.y + qh + gap), ImVec2(tl.x + w, tl.y + h), IM_COL32(40, 40, 40, 200), 1.0f);
}

void drawFolderIcon(ImDrawList* dl, const ImVec2& center, float size) {
    const float w = size * 0.88f;
    const float h = size * 0.72f;
    const ImVec2 tl(center.x - w * 0.5f, center.y - h * 0.5f);

    // Back Tab
    dl->AddRectFilled(ImVec2(tl.x, tl.y), ImVec2(tl.x + w * 0.45f, tl.y + h * 0.35f),
                      IM_COL32(210, 150, 40, 255), 1.5f);
    dl->AddRect(ImVec2(tl.x, tl.y), ImVec2(tl.x + w * 0.45f, tl.y + h * 0.35f),
                IM_COL32(80, 50, 10, 255), 1.5f);

    // White Paper Insert
    dl->AddRectFilled(ImVec2(tl.x + w * 0.12f, tl.y + h * 0.14f),
                      ImVec2(tl.x + w * 0.82f, tl.y + h * 0.70f),
                      IM_COL32(250, 250, 250, 255), 1.0f);
    dl->AddLine(ImVec2(tl.x + w * 0.20f, tl.y + h * 0.28f),
                ImVec2(tl.x + w * 0.74f, tl.y + h * 0.28f), IM_COL32(140, 180, 230, 255), 1.0f);
    dl->AddLine(ImVec2(tl.x + w * 0.20f, tl.y + h * 0.42f),
                ImVec2(tl.x + w * 0.65f, tl.y + h * 0.42f), IM_COL32(140, 180, 230, 255), 1.0f);

    // Front Folder Flap (Golden Manila with 3D highlight)
    const ImVec2 flapMin(tl.x, tl.y + h * 0.26f);
    const ImVec2 flapMax(tl.x + w, tl.y + h);
    dl->AddRectFilled(flapMin, flapMax, IM_COL32(255, 206, 78, 255), 1.5f);
    // Outer flap border
    dl->AddRect(flapMin, flapMax, IM_COL32(90, 55, 10, 255), 1.5f, 0, 1.2f);
    // Top highlight line on front flap
    dl->AddLine(ImVec2(flapMin.x + 1.0f, flapMin.y + 1.0f),
                ImVec2(flapMax.x - 1.0f, flapMin.y + 1.0f), IM_COL32(255, 245, 170, 255), 1.5f);
}

void drawSettingsIcon(ImDrawList* dl, const ImVec2& center, float size) {
    const float s = size * 0.82f;
    const ImVec2 tl(center.x - s * 0.5f, center.y - s * 0.5f);

    // Classic Beige CRT Monitor
    const ImVec2 monMin(tl.x, tl.y);
    const ImVec2 monMax(tl.x + s * 0.88f, tl.y + s * 0.68f);
    dl->AddRectFilled(monMin, monMax, IM_COL32(218, 214, 200, 255), 2.0f);
    dl->AddRect(monMin, monMax, IM_COL32(70, 68, 62, 255), 2.0f, 0, 1.2f);

    // CRT Screen (Dark Blue/Teal)
    const ImVec2 scrMin(monMin.x + s * 0.10f, monMin.y + s * 0.10f);
    const ImVec2 scrMax(monMax.x - s * 0.10f, monMax.y - s * 0.12f);
    dl->AddRectFilled(scrMin, scrMax, IM_COL32(0, 110, 160, 255), 1.0f);
    dl->AddRect(scrMin, scrMax, IM_COL32(20, 40, 60, 255), 1.0f);

    // Monitor stand
    const float standW = s * 0.32f;
    const float standH = s * 0.14f;
    const ImVec2 standMin(center.x - standW * 0.5f - s * 0.05f, monMax.y);
    const ImVec2 standMax(center.x + standW * 0.5f - s * 0.05f, monMax.y + standH);
    dl->AddRectFilled(standMin, standMax, IM_COL32(195, 190, 175, 255));
    dl->AddRect(standMin, standMax, IM_COL32(80, 78, 70, 255));

    // Metallic Gear / Wrench emblem on the lower-right
    const ImVec2 gearC(center.x + s * 0.24f, center.y + s * 0.22f);
    const float  gearR = s * 0.28f;
    for (int i = 0; i < 6; ++i) {
        const float a = static_cast<float>(i) * 3.14159265f / 3.0f;
        const ImVec2 d(std::cos(a), std::sin(a));
        dl->AddLine(ImVec2(gearC.x + d.x * gearR * 0.7f, gearC.y + d.y * gearR * 0.7f),
                    ImVec2(gearC.x + d.x * gearR * 1.35f, gearC.y + d.y * gearR * 1.35f),
                    IM_COL32(60, 75, 95, 255), 2.5f);
    }
    dl->AddCircleFilled(gearC, gearR, IM_COL32(140, 155, 175, 255), 16);
    dl->AddCircle(gearC, gearR, IM_COL32(40, 50, 65, 255), 16, 1.2f);
    dl->AddCircleFilled(gearC, gearR * 0.38f, IM_COL32(218, 214, 200, 255), 12);
    dl->AddCircle(gearC, gearR * 0.38f, IM_COL32(40, 50, 65, 255), 12, 1.0f);
}

void drawTaskManagerIcon(ImDrawList* dl, const ImVec2& center, float size) {
    const float s = size * 0.88f;
    const ImVec2 tl(center.x - s * 0.5f, center.y - s * 0.5f);

    // Beveled frame
    draw3DBox(dl, tl, ImVec2(tl.x + s, tl.y + s), false, kGrayFace);

    // Sunken CRT Screen area
    const ImVec2 scrMin(tl.x + 3.0f, tl.y + 3.0f);
    const ImVec2 scrMax(tl.x + s - 3.0f, tl.y + s - 3.0f);
    drawSunkenBorder(dl, scrMin, scrMax, IM_COL32(0, 24, 8, 255));

    // Green oscilloscope grid lines
    const float gridStep = (scrMax.x - scrMin.x) / 4.0f;
    for (int i = 1; i < 4; ++i) {
        const float gx = scrMin.x + gridStep * i;
        const float gy = scrMin.y + gridStep * i;
        dl->AddLine(ImVec2(gx, scrMin.y), ImVec2(gx, scrMax.y), IM_COL32(0, 70, 20, 255), 1.0f);
        dl->AddLine(ImVec2(scrMin.x, gy), ImVec2(scrMax.x, gy), IM_COL32(0, 70, 20, 255), 1.0f);
    }

    // Dynamic green pulse graph line (ECG / CPU activity curve)
    const float baseY = scrMin.y + (scrMax.y - scrMin.y) * 0.65f;
    const ImVec2 pts[] = {
        ImVec2(scrMin.x + 2.0f, baseY),
        ImVec2(scrMin.x + gridStep * 0.8f, baseY),
        ImVec2(scrMin.x + gridStep * 1.3f, scrMin.y + 4.0f),
        ImVec2(scrMin.x + gridStep * 1.8f, scrMax.y - 4.0f),
        ImVec2(scrMin.x + gridStep * 2.3f, baseY),
        ImVec2(scrMin.x + gridStep * 3.0f, baseY - 6.0f),
        ImVec2(scrMax.x - 2.0f, baseY)
    };

    for (size_t i = 0; i < 6; ++i) {
        dl->AddLine(pts[i], pts[i + 1], IM_COL32(0, 255, 64, 255), 1.8f);
    }
}

void drawPowerIcon(ImDrawList* dl, const ImVec2& center, float size) {
    const float r = size * 0.38f;
    // Red power button with silver bezel
    dl->AddCircleFilled(center, r + 2.0f, IM_COL32(230, 230, 230, 255), 24);
    dl->AddCircle(center, r + 2.0f, IM_COL32(90, 90, 90, 255), 24, 1.2f);
    dl->AddCircleFilled(center, r, IM_COL32(204, 40, 40, 255), 24);

    // Power circle with top gap
    const int segments = 24;
    const float gapHalf = 0.44f;
    const float start = -1.5707963f + gapHalf;
    const float innerR = r * 0.58f;

    for (int i = 0; i < segments; ++i) {
        const float a0 = start + (2.0f * 3.14159265f - 2.0f * gapHalf) * (static_cast<float>(i) / segments);
        const float a1 = start + (2.0f * 3.14159265f - 2.0f * gapHalf) * (static_cast<float>(i + 1) / segments);
        dl->AddLine(ImVec2(center.x + std::cos(a0) * innerR, center.y + std::sin(a0) * innerR),
                    ImVec2(center.x + std::cos(a1) * innerR, center.y + std::sin(a1) * innerR),
                    IM_COL32(255, 255, 255, 255), 2.0f);
    }
    // Vertical line in gap
    dl->AddLine(ImVec2(center.x, center.y - innerR - 2.0f),
                ImVec2(center.x, center.y - 1.0f), IM_COL32(255, 255, 255, 255), 2.0f);
}

void drawSpeakerIcon(ImDrawList* dl, const ImVec2& center, float size) {
    const float s = size * 0.65f;
    const ImVec2 tl(center.x - s * 0.5f, center.y - s * 0.5f);

    // Speaker body
    dl->AddRectFilled(ImVec2(tl.x, tl.y + s * 0.3f), ImVec2(tl.x + s * 0.35f, tl.y + s * 0.7f),
                      IM_COL32(30, 30, 30, 255));
    // Speaker cone
    const ImVec2 cone[3] = {
        ImVec2(tl.x + s * 0.35f, tl.y + s * 0.3f),
        ImVec2(tl.x + s * 0.7f,  tl.y + s * 0.05f),
        ImVec2(tl.x + s * 0.7f,  tl.y + s * 0.95f)
    };
    dl->AddTriangleFilled(cone[0], cone[1], cone[2], IM_COL32(30, 30, 30, 255));
    // Sound wave arcs
    dl->AddLine(ImVec2(tl.x + s * 0.82f, tl.y + s * 0.3f),
                ImVec2(tl.x + s * 0.82f, tl.y + s * 0.7f), IM_COL32(50, 100, 180, 255), 1.5f);
    dl->AddLine(ImVec2(tl.x + s * 0.96f, tl.y + s * 0.15f),
                ImVec2(tl.x + s * 0.96f, tl.y + s * 0.85f), IM_COL32(50, 100, 180, 255), 1.5f);
}

void drawNetworkIcon(ImDrawList* dl, const ImVec2& center, float size) {
    const float s = size * 0.60f;
    // Monitor 1 (back)
    const ImVec2 m1(center.x - s * 0.35f, center.y - s * 0.35f);
    dl->AddRectFilled(m1, ImVec2(m1.x + s * 0.6f, m1.y + s * 0.45f), IM_COL32(180, 180, 180, 255));
    dl->AddRectFilled(ImVec2(m1.x + 2.0f, m1.y + 2.0f), ImVec2(m1.x + s * 0.6f - 2.0f, m1.y + s * 0.45f - 2.0f),
                      IM_COL32(0, 180, 80, 255)); // Active green signal
    // Monitor 2 (front)
    const ImVec2 m2(center.x - s * 0.10f, center.y - s * 0.10f);
    dl->AddRectFilled(m2, ImVec2(m2.x + s * 0.6f, m2.y + s * 0.45f), IM_COL32(210, 210, 210, 255));
    dl->AddRect(m2, ImVec2(m2.x + s * 0.6f, m2.y + s * 0.45f), IM_COL32(60, 60, 60, 255));
    dl->AddRectFilled(ImVec2(m2.x + 2.0f, m2.y + 2.0f), ImVec2(m2.x + s * 0.6f - 2.0f, m2.y + s * 0.45f - 2.0f),
                      IM_COL32(0, 200, 100, 255));
}

} // namespace RetroGfx
