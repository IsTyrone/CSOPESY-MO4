/*
 *  CSOPESY Semi-Major Output 2 / MO4 — Desktop-Style OS Mock-up
 *  Compositor — Implementation with Windows Classic styling and decoupled WindowManager
 */

#include "Compositor.h"
#include "Desktop.h"
#include "TaskBar.h"
#include "TaskManager.h"
#include "AppScreen.h"
#include "RetroGfx.h"

#include <cstdio>
#include <algorithm>
#include <GLFW/glfw3.h>
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

namespace {

void glfwErrorCallback(int code, const char* description) {
    std::fprintf(stderr, "GLFW error %d: %s\n", code, description);
}

} // namespace

void Compositor::setAppOpen(const std::string& id, bool open) {
    if (open) {
        m_windowManager.openApp(id);
    } else {
        m_windowManager.closeApp(id);
    }
}

bool Compositor::init() {
    m_config.loadFromFile("config.txt");

    glfwSetErrorCallback(glfwErrorCallback);
    if (!glfwInit()) {
        std::fprintf(stderr, "Failed to initialise GLFW.\n");
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

    m_window = glfwCreateWindow(m_config.framebufferWidth, m_config.framebufferHeight,
                                "CSOPESY SMO2 — Desktop-Style OS Mock-up", nullptr, nullptr);
    if (!m_window) {
        std::fprintf(stderr, "Failed to create a 3.3 core-profile OpenGL window.\n");
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1); // vsync

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr; // keep working folder clean
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.FontGlobalScale = m_config.uiScale;

    applyStyle();

    if (!ImGui_ImplGlfw_InitForOpenGL(m_window, true)) {
        std::fprintf(stderr, "Failed to initialise the ImGui GLFW backend.\n");
        glfwDestroyWindow(m_window);
        m_window = nullptr;
        glfwTerminate();
        return false;
    }

    if (!ImGui_ImplOpenGL3_Init("#version 130")) {
        std::fprintf(stderr, "Failed to initialise the ImGui OpenGL 3 backend.\n");
        ImGui_ImplGlfw_Shutdown();
        glfwDestroyWindow(m_window);
        m_window = nullptr;
        glfwTerminate();
        return false;
    }

    // ── Load wallpaper image texture if configured ──
    if (m_config.wallpaperMode == "image") {
        m_wallpaperTexture.loadFromFile(m_config.wallpaperImage);
    }

    // ── Register Application Modules via Polymorphic WindowManager ──
    m_windowManager.registerApp(std::make_shared<FilesApp>());
    m_windowManager.registerApp(std::make_shared<SettingsApp>());
    m_windowManager.registerApp(std::make_shared<TaskManagerApp>());

    m_simulator.init(m_config.processCount, m_config.rngSeed, m_config.minMemKb,
                     m_config.maxMemKb, m_config.minCpu, m_config.maxCpu,
                     m_config.updateIntervalMs, m_config.totalMemKb);

    m_lastTime = glfwGetTime();
    return true;
}

void Compositor::applyStyle() {
    ImGuiStyle& style = ImGui::GetStyle();

    // ── Authentic Windows Classic Metrics (Sharp, square 3D corners) ──
    style.WindowRounding    = 0.0f;
    style.ChildRounding     = 0.0f;
    style.FrameRounding     = 0.0f;
    style.PopupRounding     = 0.0f;
    style.ScrollbarRounding = 0.0f;
    style.GrabRounding      = 0.0f;
    style.TabRounding       = 0.0f;

    style.WindowBorderSize  = 0.0f; // Remove harsh black border around window apps
    style.ChildBorderSize   = 0.0f;
    style.FrameBorderSize   = 0.0f;
    style.PopupBorderSize   = 1.0f;

    style.WindowPadding     = ImVec2(8.0f, 8.0f);
    style.FramePadding      = ImVec2(6.0f, 4.0f);
    style.ItemSpacing       = ImVec2(8.0f, 6.0f);
    style.ItemInnerSpacing  = ImVec2(6.0f, 4.0f);
    style.WindowTitleAlign  = ImVec2(0.0f, 0.5f); // Left-aligned classic title

    // ── Authentic Windows Classic (95/98/2000) Colors ──
    const ImVec4 cBtnFace     = ImVec4(0.776f, 0.776f, 0.776f, 1.00f); // #C6C6C6
    const ImVec4 cNavyActive  = ImVec4(0.000f, 0.000f, 0.502f, 1.00f); // #000080
    const ImVec4 cNavyInact   = ImVec4(0.502f, 0.502f, 0.502f, 1.00f); // #808080
    const ImVec4 cText        = ImVec4(0.000f, 0.000f, 0.000f, 1.00f); // Black text
    const ImVec4 cClientWhite = ImVec4(1.000f, 1.000f, 1.000f, 1.00f); // White edit/list boxes
    const ImVec4 cSoftBorder  = ImVec4(0.502f, 0.502f, 0.502f, 1.00f); // Soft gray border

    style.Colors[ImGuiCol_Text]                  = cText;
    style.Colors[ImGuiCol_TextDisabled]          = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
    style.Colors[ImGuiCol_WindowBg]              = cBtnFace;
    style.Colors[ImGuiCol_ChildBg]               = cBtnFace;
    style.Colors[ImGuiCol_PopupBg]               = cBtnFace;
    style.Colors[ImGuiCol_Border]                = cSoftBorder;
    style.Colors[ImGuiCol_BorderShadow]          = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_FrameBg]               = cClientWhite;
    style.Colors[ImGuiCol_FrameBgHovered]        = cClientWhite;
    style.Colors[ImGuiCol_FrameBgActive]         = cClientWhite;
    style.Colors[ImGuiCol_TitleBg]               = cNavyInact;
    style.Colors[ImGuiCol_TitleBgActive]         = cNavyActive;
    style.Colors[ImGuiCol_TitleBgCollapsed]      = cNavyInact;
    style.Colors[ImGuiCol_MenuBarBg]             = cBtnFace;
    style.Colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrab]         = cBtnFace;
    style.Colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.85f, 0.85f, 0.85f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.65f, 0.65f, 0.65f, 1.00f);
    style.Colors[ImGuiCol_CheckMark]             = cText;
    style.Colors[ImGuiCol_SliderGrab]            = cBtnFace;
    style.Colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.65f, 0.65f, 0.65f, 1.00f);
    style.Colors[ImGuiCol_Button]                = cBtnFace;
    style.Colors[ImGuiCol_ButtonHovered]         = ImVec4(0.85f, 0.85f, 0.85f, 1.00f);
    style.Colors[ImGuiCol_ButtonActive]          = ImVec4(0.65f, 0.65f, 0.65f, 1.00f);
    style.Colors[ImGuiCol_Header]                = cNavyActive;
    style.Colors[ImGuiCol_HeaderHovered]         = ImVec4(0.06f, 0.38f, 0.75f, 1.00f);
    style.Colors[ImGuiCol_HeaderActive]          = cNavyActive;
    style.Colors[ImGuiCol_Separator]             = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    style.Colors[ImGuiCol_SeparatorHovered]      = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    style.Colors[ImGuiCol_SeparatorActive]       = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    style.Colors[ImGuiCol_ResizeGrip]            = ImVec4(0.50f, 0.50f, 0.50f, 0.60f);
    style.Colors[ImGuiCol_ResizeGripHovered]     = ImVec4(0.00f, 0.00f, 0.50f, 0.80f);
    style.Colors[ImGuiCol_ResizeGripActive]      = cNavyActive;
    style.Colors[ImGuiCol_Tab]                   = cBtnFace;
    style.Colors[ImGuiCol_TabHovered]            = ImVec4(0.85f, 0.85f, 0.85f, 1.00f);
    style.Colors[ImGuiCol_TabActive]             = cBtnFace;
    style.Colors[ImGuiCol_TabUnfocused]          = cBtnFace;
    style.Colors[ImGuiCol_TabUnfocusedActive]   = cBtnFace;
    style.Colors[ImGuiCol_PlotLines]             = ImVec4(0.00f, 0.80f, 0.20f, 1.00f);
    style.Colors[ImGuiCol_PlotLinesHovered]      = ImVec4(0.00f, 1.00f, 0.30f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogram]         = ImVec4(0.00f, 0.00f, 0.50f, 1.00f);
    style.Colors[ImGuiCol_TableHeaderBg]         = cBtnFace;
    style.Colors[ImGuiCol_TableRowBg]            = cClientWhite;
    style.Colors[ImGuiCol_TableRowBgAlt]         = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);

    style.ScaleAllSizes(m_config.uiScale);
}

void Compositor::run() {
    while (!m_shutdownRequested && !glfwWindowShouldClose(m_window)) {
        const double now = glfwGetTime();
        const double delta = now - m_lastTime;
        m_lastTime = now;

        m_simulator.update(delta);

        beginFrame();
        drawDesktopLayer();
        drawAppWindows();
        drawTaskBarLayer();
        drawPwrLayer(); // Stays top-most in z-order
        endFrame();
    }
}

void Compositor::beginFrame() {
    glfwPollEvents();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    int w = 0, h = 0;
    glfwGetFramebufferSize(m_window, &w, &h);
    m_viewport = ImVec2(static_cast<float>(w), static_cast<float>(h));
}

void Compositor::endFrame() {
    ImGui::Render();

    glViewport(0, 0, static_cast<int>(m_viewport.x), static_cast<int>(m_viewport.y));
    glClearColor(0.0f, 0.502f, 0.502f, 1.0f); // Default #008080 teal clear
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(m_window);
}

void Compositor::shutdown() {
    m_wallpaperTexture.release();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    if (m_window) {
        glfwDestroyWindow(m_window);
        m_window = nullptr;
    }
    glfwTerminate();
}

float Compositor::taskbarHeight() const {
    return std::max(38.0f * m_config.uiScale, ImGui::GetFontSize() * 2.5f);
}

ImVec2 Compositor::viewportSize() const {
    return m_viewport;
}

void Compositor::desktopArea(ImVec2& outMin, ImVec2& outMax) const {
    const float bar = taskbarHeight();
    if (m_config.taskbarAtTop) {
        outMin = ImVec2(0.0f, bar);
        outMax = m_viewport;
    } else {
        outMin = ImVec2(0.0f, 0.0f);
        outMax = ImVec2(m_viewport.x, m_viewport.y - bar);
    }
}

void Compositor::drawDesktopLayer() {
    ImVec2 areaMin, areaMax;
    desktopArea(areaMin, areaMax);
    Desktop::draw(*this, areaMin, areaMax);
}

void Compositor::drawAppWindows() {
    m_windowManager.renderWindows(*this);
}

void Compositor::drawTaskBarLayer() {
    TaskBar::draw(*this);
}

void Compositor::drawPwrLayer() {
    ImVec2 areaMin, areaMax;
    desktopArea(areaMin, areaMax);
    Desktop::drawPwr(*this, areaMin, areaMax);
}
