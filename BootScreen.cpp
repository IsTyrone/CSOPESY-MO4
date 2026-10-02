/*
 *  CSOPESY Semi-Major Output 2 / MO4 — Desktop-Style OS Mock-up
 *  BootScreen — Windows XP style loading / starting screen implementation
 */

#include "BootScreen.h"
#include <cfloat>
#include <cmath>

namespace {

constexpr ImU32 kFlagRed   = IM_COL32(236, 52, 42, 255);
constexpr ImU32 kFlagGreen = IM_COL32(0, 168, 80, 255);
constexpr ImU32 kFlagBlue  = IM_COL32(32, 112, 224, 255);
constexpr ImU32 kFlagGold  = IM_COL32(252, 198, 28, 255);

constexpr ImU32 kWhite     = IM_COL32(255, 255, 255, 255);
constexpr ImU32 kXpOrange  = IM_COL32(232, 122, 24, 255);   // "xp" wordmark
constexpr ImU32 kXpBarBlue = IM_COL32(34, 110, 220, 255);   // progress blocks
constexpr ImU32 kXpBarHi   = IM_COL32(140, 195, 255, 255);  // block gloss
constexpr ImU32 kBootGray  = IM_COL32(160, 160, 160, 255);  // footer text

// Vertical wave offset for the waving-flag look. nx in [0,1] across the flag.
float flagWave(float nx, float h) {
    return (std::sin(nx * 3.14159265f * 1.15f) * 0.055f - nx * 0.045f) * h;
}

// Windows XP style waving flag: 4 color panes bent by a sine wave.
// Drawn as vertical strips so the wave bends each pane smoothly.
void drawXpFlag(ImDrawList* dl, const ImVec2& center, float w, float h) {
    if (w <= 0.0f || h <= 0.0f) return;

    const float x0 = center.x - w * 0.5f;
    const float yT = center.y - h * 0.5f;
    const float yM = center.y;
    const float yB = center.y + h * 0.5f;

    constexpr int kStrips = 28;
    for (int i = 0; i < kStrips; ++i) {
        const float n0 = static_cast<float>(i) / kStrips;
        const float n1 = static_cast<float>(i + 1) / kStrips;
        const float sx0 = x0 + n0 * w;
        const float sx1 = x0 + n1 * w + 0.6f; // slight overlap: no seams
        const float o0 = flagWave(n0, h);
        const float o1 = flagWave(n1, h);

        const float nc = (n0 + n1) * 0.5f;
        const ImU32 topCol = (nc < 0.5f) ? kFlagRed : kFlagGreen;
        const ImU32 botCol = (nc < 0.5f) ? kFlagBlue : kFlagGold;

        dl->AddQuadFilled(ImVec2(sx0, yT + o0), ImVec2(sx1, yT + o1),
                          ImVec2(sx1, yM + o1), ImVec2(sx0, yM + o0), topCol);
        dl->AddQuadFilled(ImVec2(sx0, yM + o0), ImVec2(sx1, yM + o1),
                          ImVec2(sx1, yB + o1), ImVec2(sx0, yB + o0), botCol);
    }

    // Thin dark seam between the left/right panes, following the wave.
    const float xc = center.x;
    const float oc = flagWave(0.5f, h);
    dl->AddLine(ImVec2(xc, yT + oc), ImVec2(xc, yB + oc), IM_COL32(0, 0, 0, 110), 2.0f);
}

} // namespace

void BootScreen::draw(const ImVec2& view, double elapsedSec,
                      ImTextureID logoTex, int logoW, int logoH) {
    ImDrawList* dl = ImGui::GetBackgroundDrawList();

    // ── 1. Black screen ──
    dl->AddRectFilled(ImVec2(0.0f, 0.0f), view, IM_COL32(0, 0, 0, 255));

    ImFont* font = ImGui::GetFont();
    const float cx = view.x * 0.5f;
    const float logoCy = view.y * 0.40f;

    // ── 2. Center logo: image file, vector lockup as fallback ──
    constexpr float kBarW = 210.0f, kBarH = 18.0f;
    const float barX = cx - kBarW * 0.5f;
    float barY = logoCy + 96.0f;

    if (logoTex != 0 && logoW > 0 && logoH > 0) {
        // Shrink-to-fit inside a box; never upscale so the logo stays crisp.
        const float maxW = view.x * 0.70f;
        const float maxH = view.y * 0.42f;
        float fit = std::min(maxW / static_cast<float>(logoW),
                             maxH / static_cast<float>(logoH));
        if (fit > 1.0f) fit = 1.0f;
        const float dw = static_cast<float>(logoW) * fit;
        const float dh = static_cast<float>(logoH) * fit;

        const float imgCy = logoCy - 10.0f;
        dl->AddImage(logoTex,
                     ImVec2(cx - dw * 0.5f, imgCy - dh * 0.5f),
                     ImVec2(cx + dw * 0.5f, imgCy + dh * 0.5f));

        barY = imgCy + dh * 0.5f + 44.0f;
    } else {
        // Built-in vector lockup: waving flag (left) + wordmark (right)
        constexpr float kFlagW = 150.0f, kFlagH = 116.0f, kGap = 26.0f;
        constexpr float kBrandSize = 25.0f, kNameSize = 56.0f, kEditionSize = 23.0f;

        const float windowsW =
            font->CalcTextSizeA(kNameSize, FLT_MAX, 0.0f, "Windows").x;
        const float xpW =
            font->CalcTextSizeA(kNameSize, FLT_MAX, 0.0f, "xp").x;
        const float brandW =
            font->CalcTextSizeA(kBrandSize, FLT_MAX, 0.0f, "Microsoft").x;
        const float editionW =
            font->CalcTextSizeA(kEditionSize, FLT_MAX, 0.0f, "Professional").x;

        float textW = windowsW + 6.0f + xpW;
        if (brandW > textW)   textW = brandW;
        if (editionW > textW) textW = editionW;

        const float lockX = cx - (kFlagW + kGap + textW) * 0.5f;
        const float tx = lockX + kFlagW + kGap;

        drawXpFlag(dl, ImVec2(lockX + kFlagW * 0.5f, logoCy - 8.0f), kFlagW, kFlagH);

        dl->AddText(font, kBrandSize, ImVec2(tx, logoCy - 80.0f), kWhite, "Microsoft");

        const ImVec2 namePos(tx, logoCy - 50.0f);
        dl->AddText(font, kNameSize, namePos, kWhite, "Windows");
        dl->AddText(font, kNameSize, ImVec2(namePos.x + windowsW + 6.0f, namePos.y),
                    kXpOrange, "xp");

        dl->AddText(font, kEditionSize, ImVec2(tx + 4.0f, logoCy + 24.0f),
                    kWhite, "Professional");

        barY = logoCy + 96.0f;
    }

    // ── 3. Animated progress bar (blue blocks sweeping left to right) ──

    dl->AddRect(ImVec2(barX, barY), ImVec2(barX + kBarW, barY + kBarH),
                IM_COL32(128, 128, 128, 255));
    dl->AddRectFilled(ImVec2(barX + 1.0f, barY + 1.0f),
                      ImVec2(barX + kBarW - 1.0f, barY + kBarH - 1.0f),
                      IM_COL32(0, 0, 0, 255));

    const float ix0 = barX + 2.0f, ix1 = barX + kBarW - 2.0f;
    const float iy0 = barY + 3.0f, iy1 = barY + kBarH - 3.0f;

    constexpr float kBlockW = 10.0f, kBlockGap = 5.0f;
    constexpr int kBlocks = 3;
    const float groupW = kBlocks * kBlockW + (kBlocks - 1) * kBlockGap;
    const float travel = (ix1 - ix0) + groupW;
    const float head = std::fmod(static_cast<float>(elapsedSec) * 130.0f, travel);
    const float sx = ix0 - groupW + head;

    dl->PushClipRect(ImVec2(ix0, iy0), ImVec2(ix1, iy1), true);
    for (int i = 0; i < kBlocks; ++i) {
        const float bx0 = sx + i * (kBlockW + kBlockGap);
        dl->AddRectFilled(ImVec2(bx0, iy0), ImVec2(bx0 + kBlockW, iy1), kXpBarBlue);
        dl->AddRectFilled(ImVec2(bx0, iy0), ImVec2(bx0 + kBlockW, iy0 + (iy1 - iy0) * 0.45f),
                          kXpBarHi);
    }
    dl->PopClipRect();

    // ── 4. Footer: copyright line ──
    constexpr float kFootSize = 15.0f;
    const float footY = view.y - 34.0f;
    dl->AddText(font, kFootSize, ImVec2(24.0f, footY), kBootGray,
                "Copyright (c) Microsoft Corporation");
}

bool BootScreen::skipRequested() {
    if (ImGui::IsMouseClicked(0, false) || ImGui::IsMouseClicked(1, false))
        return true;
    for (int k = ImGuiKey_NamedKey_BEGIN; k < ImGuiKey_NamedKey_END; ++k) {
        if (ImGui::IsKeyPressed(static_cast<ImGuiKey>(k), false))
            return true;
    }
    return false;
}
