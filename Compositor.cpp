#include "Compositor.h"
#include "Desktop.h"
#include "TaskBar.h"
#include "TaskManager.h"
#include "AppScreen.h"

#include <algorithm>
#include <cstdio>
#include <cstring>

#include <GLFW/glfw3.h>
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

namespace {

void glfwErrorCallback(int code, const char* description) {
    std::fprintf(stderr, "GLFW error %d: %s\n", code, description);
}

} // namespace

const char* Compositor::appFiles()      { return "Files"; }
const char* Compositor::appSettings()   { return "Settings"; }
const char* Compositor::appTaskManager(){ return "Task Manager"; }

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
    glfwSwapInterval(1);   // vsync — the compositor presents on a steady cadence

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // Scale the UI up: the submission MP4 has to stay legible on a projector.
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;              // keep the working folder clean
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.FontGlobalScale = 1.2f;
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

    registerApp(appFiles());
    registerApp(appSettings());
    registerApp(appTaskManager());

    m_simulator.init(m_config.processCount, m_config.rngSeed, m_config.minMemKb,
                     m_config.maxMemKb, m_config.minCpu, m_config.maxCpu,
                     m_config.updateIntervalMs, m_config.totalMemKb);

    m_lastTime = glfwGetTime();
    return true;
}

void Compositor::applyStyle() {
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding    = 6.0f;
    style.ChildRounding     = 4.0f;
    style.FrameRounding     = 4.0f;
    style.PopupRounding     = 4.0f;
    style.ScrollbarRounding = 6.0f;
    style.GrabRounding      = 4.0f;
    style.WindowBorderSize  = 1.0f;
    style.FrameBorderSize   = 0.0f;
    style.WindowPadding     = ImVec2(12.0f, 10.0f);
    style.ItemSpacing       = ImVec2(8.0f, 6.0f);
    style.WindowTitleAlign  = ImVec2(0.5f, 0.5f);
    style.Colors[ImGuiCol_WindowBg]            = ImVec4(0.10f, 0.11f, 0.14f, 0.96f);
    style.Colors[ImGuiCol_TitleBg]              = ImVec4(0.13f, 0.15f, 0.19f, 1.00f);
    style.Colors[ImGuiCol_TitleBgActive]        = ImVec4(0.20f, 0.30f, 0.52f, 1.00f);
    style.Colors[ImGuiCol_Header]               = ImVec4(0.20f, 0.30f, 0.52f, 0.85f);
    style.Colors[ImGuiCol_HeaderHovered]        = ImVec4(0.26f, 0.40f, 0.68f, 1.00f);
    style.Colors[ImGuiCol_Button]               = ImVec4(0.18f, 0.22f, 0.30f, 1.00f);
    style.Colors[ImGuiCol_ButtonHovered]        = ImVec4(0.26f, 0.38f, 0.62f, 1.00f);
    style.Colors[ImGuiCol_ButtonActive]         = ImVec4(0.20f, 0.30f, 0.52f, 1.00f);
    style.Colors[ImGuiCol_TableHeaderBg]        = ImVec4(0.16f, 0.19f, 0.25f, 1.00f);
    style.Colors[ImGuiCol_TableRowBgAlt]        = ImVec4(1.00f, 1.00f, 1.00f, 0.025f);
    style.Colors[ImGuiCol_CheckMark]            = ImVec4(0.42f, 0.72f, 1.00f, 1.00f);
    style.ScaleAllSizes(1.15f);
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
        drawPwrLayer();      // must be last: PWR needs to sit on top of z-order
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
    glClearColor(0.05f, 0.06f, 0.08f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(m_window);
}

void Compositor::shutdown() {
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
    return ImGui::GetFontSize() * 2.6f;
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

void Compositor::registerApp(const std::string& title) {
    for (const AppEntry& a : m_apps)
        if (a.title == title) return;
    m_apps.push_back(AppEntry{title, false});
}

void Compositor::setAppOpen(const std::string& title, bool open) {


    for (AppEntry& a : m_apps) {
        if (a.title != title) continue;
        if (a.open == open) return;
        a.open = open;
        m_openAppCount += open ? 1 : -1;
        if (open) m_focusedApp = title;
        return;
    }
}

bool Compositor::isAppOpen(const std::string& title) const {
    for (const AppEntry& a : m_apps)
        if (a.title == title) return a.open;
    return false;
}

std::vector<std::string> Compositor::openAppTitles() const {
    std::vector<std::string> titles;
    for (const AppEntry& a : m_apps)
        if (a.open) titles.push_back(a.title);
    return titles;
}

void Compositor::drawDesktopLayer() {
    ImVec2 areaMin, areaMax;
    desktopArea(areaMin, areaMax);
    Desktop::draw(*this, areaMin, areaMax);
}

void Compositor::drawAppWindows() {
    for (AppEntry& app : m_apps) {
        if (!app.open) continue;

        bool keepOpen = true;
        if (app.title == appFiles()) {
            AppScreen::drawFiles(*this, &keepOpen);
        } else if (app.title == appSettings()) {
            AppScreen::drawSettings(*this, &keepOpen);
        } else if (app.title == appTaskManager()) {
            TaskManager::draw(*this, &keepOpen);
        }
        // Only let the window's own close button clear the flag;
        // never write true back — that would resurrect a window the
        // TaskBar just toggled closed.
        if (!keepOpen) app.open = false;
    }

    // Recount in case a window was closed by its own close button.
    m_openAppCount = 0;
    for (const AppEntry& a : m_apps)
        if (a.open) ++m_openAppCount;
}

void Compositor::drawTaskBarLayer() {
    TaskBar::draw(*this);
}
void Compositor::drawPwrLayer() {
    ImVec2 areaMin, areaMax;
    desktopArea(areaMin, areaMax);
    Desktop::drawPwr(*this, areaMin, areaMax);
}
