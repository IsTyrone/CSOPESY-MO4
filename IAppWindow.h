/*
 *  CSOPESY Semi-Major Output 2 / MO4 — Desktop-Style OS Mock-up
 *  IAppWindow — Abstract Application Window Interface
 */

#pragma once
#include <string>
#include "imgui.h"

class Compositor;

class IAppWindow {
public:
    virtual ~IAppWindow() = default;

    // Unique identifier for registration and lookup
    virtual const std::string& id() const = 0;

    // Human-readable title displayed on titlebars and taskbar chips
    virtual const std::string& title() const = 0;

    // Render the application window frame and content
    virtual void render(Compositor& compositor, bool* keepOpen) = 0;

    // Render the application vector icon at the given center and bounding size
    virtual void drawIcon(ImDrawList* dl, const ImVec2& center, float size) = 0;
};
