/*
 *  CSOPESY Semi-Major Output 2 / MO4 — Desktop-Style OS Mock-up
 *  Desktop — Wallpaper, Desktop Icons, Real-time Clock, and PWR button
 */

#pragma once
#include "imgui.h"

class Compositor;

namespace Desktop {

// Render the desktop layer: wallpaper, desktop icons, and desktop clock
void draw(Compositor& compositor, const ImVec2& areaMin, const ImVec2& areaMax);

// Render the PWR button (drawn last to remain top-most in z-order)
void drawPwr(Compositor& compositor, const ImVec2& areaMin, const ImVec2& areaMax);

} // namespace Desktop
