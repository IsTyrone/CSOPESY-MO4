/*
 *  CSOPESY Semi-Major Output 2  —  Desktop-Style OS Mock-up
 *  Requirement A — Desktop Background
 *
 *    • A full-screen background that serves as the base layer of the OS;
 *      this is the first thing rendered each frame.
 *    • Fills the entire application window.
 *    • Displays a wallpaper (gradient, plus a pattern drawn with ImGui draw
 *      commands — no image asset to go missing on a lab machine).
 *    • Displays a real-time clock (current time, updated every frame) in a
 *      corner of the screen.
 *    • Provides a PWR button that shuts the system down.
 *
 *  The wallpaper goes on ImGui's background draw list, which the compositor
 *  submits ahead of every window — genuinely the bottom layer.  The clock and
 *  the PWR button live on this layer's own full-screen window, which is
 *  submitted before the app windows and the taskbar, so they stay in front.
 */

#pragma once
#include "imgui.h"

class Compositor;

namespace Desktop {

constexpr float kClockX      = 28.0f;
constexpr float kClockY      = 22.0f;
constexpr float kPwrW        = 116.0f;
constexpr float kPwrH        = 38.0f;
constexpr float kPwrMargin   = 28.0f;

// Paints the wallpaper, the clock and the PWR button.
void draw(Compositor& compositor, const ImVec2& areaMin, const ImVec2& areaMax);

// Paints the PWR shutdown button.  Called as the very last layer each frame
// so the button sits on top of all other windows in the z-order.
void drawPwr(Compositor& compositor, const ImVec2& areaMin, const ImVec2& areaMax);

} // namespace Desktop
