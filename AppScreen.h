/*
 *  CSOPESY Semi-Major Output 2  —  Desktop-Style OS Mock-up
 *  Requirement B — the two unique UI screens
 *
 *  "Two of these buttons must open up a unique UI screen, with placeholder
 *   information. Any UI screen with your own design or layout is acceptable."
 *
 *  Both screens are intentionally different from each other and from the Task
 *  Manager: Files is a two-pane browser with a sidebar; Settings is a
 *  single-column form of grouped controls.  Every value shown is a
 *  placeholder — no filesystem or registry is touched.
 */

#pragma once
#include "imgui.h"

class Compositor;

namespace AppScreen {

void drawFiles(Compositor& compositor, bool* keepOpen);
void drawSettings(Compositor& compositor, bool* keepOpen);

} // namespace AppScreen
