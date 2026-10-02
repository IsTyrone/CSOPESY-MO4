/*
 *  CSOPESY Semi-Major Output 2 / MO4 — Desktop-Style OS Mock-up
 *  RetroGfx — Windows Classic (95/98/2000) Visual Engine & Iconography
 */

#pragma once
#include "imgui.h"

namespace RetroGfx {

// ── Authentic Windows Classic Palette ─────────────────────────────
constexpr ImU32 kTealDesktop     = IM_COL32(0, 128, 128, 255);       // #008080 Classic Desktop
constexpr ImU32 kGrayFace        = IM_COL32(198, 198, 198, 255);     // Standard Button / Panel Face
constexpr ImU32 kGrayLight       = IM_COL32(224, 224, 224, 255);     // Inner Bevel Highlight
constexpr ImU32 kWhite           = IM_COL32(255, 255, 255, 255);     // Outer Top/Left Highlight
constexpr ImU32 kGrayShadow      = IM_COL32(128, 128, 128, 255);     // Inner Shadow
constexpr ImU32 kBlack           = IM_COL32(0, 0, 0, 255);           // Outer Bottom/Right Shadow
constexpr ImU32 kNavyActive      = IM_COL32(0, 0, 128, 255);         // Active Window Titlebar Left
constexpr ImU32 kNavyActiveGrad  = IM_COL32(16, 96, 192, 255);       // Active Window Titlebar Right
constexpr ImU32 kNavyInactive    = IM_COL32(128, 128, 128, 255);     // Inactive Window Titlebar
constexpr ImU32 kTitleText       = IM_COL32(255, 255, 255, 255);     // Titlebar text
constexpr ImU32 kClientWhite     = IM_COL32(255, 255, 255, 255);     // Sunken client area (lists, tables)

// ── 3D Bevel & Box Drawing ───────────────────────────────────────
// Standard 2-pixel beveled rectangular box (raised or sunken)
void draw3DBox(ImDrawList* dl, const ImVec2& min, const ImVec2& max, bool sunken = false, ImU32 fillCol = kGrayFace);

// Thin 1-pixel border box for recessed controls
void drawSunkenBorder(ImDrawList* dl, const ImVec2& min, const ImVec2& max, ImU32 fillCol = kClientWhite);

// ── Windows Classic Title Bar ────────────────────────────────────
void drawClassicTitleBar(ImDrawList* dl, const ImVec2& min, const ImVec2& max,
                         const char* title, bool active);

// ── Scalable Retro Vector Icons ──────────────────────────────────
// Windows 4-color flag (for Start button)
void drawWindowsFlag(ImDrawList* dl, const ImVec2& center, float size);

// Manila Folder icon (for Files / Explorer)
void drawFolderIcon(ImDrawList* dl, const ImVec2& center, float size);

// Computer / Control Panel icon (for Settings)
void drawSettingsIcon(ImDrawList* dl, const ImVec2& center, float size);

// Performance Monitor CRT icon (for Task Manager)
void drawTaskManagerIcon(ImDrawList* dl, const ImVec2& center, float size);

// Red Shut Down / Power icon
void drawPowerIcon(ImDrawList* dl, const ImVec2& center, float size);

// System Tray: Audio Speaker
void drawSpeakerIcon(ImDrawList* dl, const ImVec2& center, float size);

// System Tray: Network Monitors
void drawNetworkIcon(ImDrawList* dl, const ImVec2& center, float size);

} // namespace RetroGfx
