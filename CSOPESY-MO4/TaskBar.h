/*
 *  CSOPESY Semi-Major Output 2  —  Desktop-Style OS Mock-up
 *  Requirement B — TaskBar
 *
 *    • A fixed panel at the top of the screen that gives access to system
 *      functions and shows running applications.
 *    • Contains at least three clickable icon buttons.
 *      - Files        → unique UI screen with placeholder information
 *      - Settings     → unique UI screen with placeholder information
 *      - Task Manager → the Task Manager (Requirement C)
 *
 *  The buttons are drawn as vector icons on ImGui draw lists rather than
 *  glyphs from a bundled icon font, so the taskbar has no font dependency.
 */

#pragma once
#include "imgui.h"

class Compositor;

namespace TaskBar {

constexpr float kIconSize   = 34.0f;
constexpr float kIconGap    = 8.0f;
constexpr float kIconPadX   = 14.0f;
constexpr float kChipPadX   = 12.0f;

void draw(Compositor& compositor);

} // namespace TaskBar
