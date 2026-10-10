/*
 *  CSOPESY Semi-Major Output 2 / MO4 — Desktop-Style OS Mock-up
 *  BootScreen — Windows XP style loading / starting screen
 *
 *  Shown fullscreen at startup before the desktop appears: black screen,
 *  waving Windows flag + wordmark, animated blue-block progress bar,
 *  and the classic copyright footer. Skippable with any key or click.
 */

#pragma once
#include "imgui.h"

namespace BootScreen {

// Seconds the boot splash stays fullscreen before the desktop appears.
constexpr double kDurationSec = 5.0;

// Render the splash. `elapsedSec` drives the progress-bar animation.
// `logoTex`/`logoW`/`logoH` is the center logo image; when null the
// built-in vector lockup (flag + wordmark) is drawn instead.
void draw(const ImVec2& view, double elapsedSec,
          ImTextureID logoTex = 0, int logoW = 0, int logoH = 0);

// True if the user pressed any key or clicked (request to skip the splash).
bool skipRequested();

} // namespace BootScreen
