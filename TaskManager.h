/*
 *  CSOPESY Semi-Major Output 2  —  Desktop-Style OS Mock-up
 *  Requirement C — Task Manager
 *
 *    • A window that closely resembles the Windows task manager.
 *    • Must have a placeholder table showing "Processes" and their respective
 *      CPU and memory usage.
 *    • Uses values from the live ProcessSimulator (placeholder-grade, driven
 *      by a seeded generator so runs are reproducible).
 */

#pragma once
#include "imgui.h"

class Compositor;

namespace TaskManager {

constexpr float kHeaderH = 74.0f;

// keepOpen is set to false when the user closes the window.
void draw(Compositor& compositor, bool* keepOpen);

} // namespace TaskManager
