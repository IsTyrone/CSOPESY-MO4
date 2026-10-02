/*
 *  CSOPESY Semi-Major Output 2 / MO4 — Desktop-Style OS Mock-up
 *  Requirement B — Files (Explorer) & Settings (Control Panel) Implementation
 */

#include "AppScreen.h"
#include "Compositor.h"
#include "RetroGfx.h"
#include <cstdio>

namespace {

struct FileItem {
    const char* name;
    const char* size;
    const char* type;
    const char* modified;
    bool isFolder;
};

const FileItem kExplorerFiles[] = {
    { "..",              "",         "Folder",        "2026-10-01", true  },
    { "third_party",     "",         "File Folder",   "2026-10-01", true  },
    { "AppScreen.cpp",   "6 KB",     "C++ Source",    "2026-10-02", false },
    { "AppScreen.h",     "1 KB",     "C++ Header",    "2026-10-02", false },
    { "Compositor.cpp",  "9 KB",     "C++ Source",    "2026-10-02", false },
    { "Compositor.h",    "3 KB",     "C++ Header",    "2026-10-02", false },
    { "Config.cpp",      "4 KB",     "C++ Source",    "2026-10-02", false },
    { "Config.h",        "1 KB",     "C++ Header",    "2026-10-02", false },
    { "Desktop.cpp",     "7 KB",     "C++ Source",    "2026-10-02", false },
    { "Desktop.h",       "2 KB",     "C++ Header",    "2026-10-02", false },
    { "ProcessSimulator.cpp", "5 KB", "C++ Source",   "2026-10-02", false },
    { "ProcessSimulator.h",   "2 KB", "C++ Header",   "2026-10-02", false },
    { "RetroGfx.cpp",    "6 KB",     "C++ Source",    "2026-10-02", false },
    { "RetroGfx.h",      "2 KB",     "C++ Header",    "2026-10-02", false },
    { "TaskBar.cpp",     "8 KB",     "C++ Source",    "2026-10-02", false },
    { "TaskBar.h",       "1 KB",     "C++ Header",    "2026-10-02", false },
    { "TaskManager.cpp", "6 KB",     "C++ Source",    "2026-10-02", false },
    { "TaskManager.h",   "1 KB",     "C++ Header",    "2026-10-02", false },
    { "WindowManager.cpp", "3 KB",   "C++ Source",    "2026-10-02", false },
    { "WindowManager.h",   "1 KB",   "C++ Header",    "2026-10-02", false },
    { "main.cpp",        "1 KB",     "C++ Source",    "2026-10-02", false },
    { "build.bat",       "4 KB",     "Batch File",    "2026-10-02", false },
    { "CMakeLists.txt",  "3 KB",     "CMake Script",  "2026-10-02", false },
    { "config.txt",      "1 KB",     "Config File",   "2026-10-02", false },
    { "README.md",       "8 KB",     "Markdown Doc",  "2026-10-02", false },
    { "README.txt",      "4 KB",     "Text Document", "2026-10-02", false },
};

} // namespace

// ============================================================================
// Files Application (Windows Explorer style)
// ============================================================================

void FilesApp::drawIcon(ImDrawList* dl, const ImVec2& center, float size) {
    RetroGfx::drawFolderIcon(dl, center, size);
}

void FilesApp::render(Compositor& compositor, bool* keepOpen) {
    const ImVec2 view = compositor.viewportSize();

    ImGui::SetNextWindowSize(ImVec2(720.0f, 460.0f), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(view.x * 0.5f - 360.0f, view.y * 0.5f - 230.0f), ImGuiCond_FirstUseEver);

    if (ImGui::Begin("Exploring - C:\\CSOPESY", keepOpen, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar)) {

        // Classic Menu Bar
        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("Close", "Alt+F4")) { *keepOpen = false; }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Edit")) {
                ImGui::MenuItem("Cut", "Ctrl+X", false, false);
                ImGui::MenuItem("Copy", "Ctrl+C", false, false);
                ImGui::MenuItem("Paste", "Ctrl+V", false, false);
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("View")) {
                ImGui::MenuItem("Toolbar", nullptr, true);
                ImGui::MenuItem("Status Bar", nullptr, true);
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Help")) {
                ImGui::MenuItem("About CSOPESY Explorer");
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        // Classic Address Bar
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Address");
        ImGui::SameLine();
        static char addrBuffer[128] = "C:\\CSOPESY\\src";
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 60.0f);
        ImGui::InputText("##address", addrBuffer, sizeof(addrBuffer));
        ImGui::SameLine();
        if (ImGui::Button("Go", ImVec2(50.0f, 0.0f))) {}

        ImGui::Separator();

        // Split Panes: Tree on left, Files on right
        const float availH = ImGui::GetContentRegionAvail().y;
        const float paneH = std::max(140.0f, availH - 32.0f);
        const ImGuiTableFlags tableFlags = ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg;

        if (ImGui::BeginTable("##explorer_panes", 2, tableFlags, ImVec2(0.0f, paneH))) {
            ImGui::TableSetupColumn("Folders", ImGuiTableColumnFlags_WidthFixed, 180.0f);
            ImGui::TableSetupColumn("Contents", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableHeadersRow();

            // ── Left: Folder Tree ──
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);

            if (ImGui::TreeNodeEx("Desktop", ImGuiTreeNodeFlags_DefaultOpen)) {
                if (ImGui::TreeNodeEx("My Computer", ImGuiTreeNodeFlags_DefaultOpen)) {
                    if (ImGui::TreeNodeEx("Local Disk (C:)", ImGuiTreeNodeFlags_DefaultOpen)) {
                        if (ImGui::TreeNodeEx("CSOPESY", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Selected)) {
                            ImGui::BulletText("bin");
                            ImGui::BulletText("config");
                            ImGui::BulletText("src");
                            ImGui::BulletText("third_party");
                            ImGui::TreePop();
                        }
                        ImGui::TreePop();
                    }
                    ImGui::TreePop();
                }
                ImGui::BulletText("Recycle Bin");
                ImGui::TreePop();
            }

            // ── Right: File List ──
            ImGui::TableSetColumnIndex(1);
            const ImGuiTableFlags fileTableFlags =
                ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY | ImGuiTableFlags_SizingStretchProp;

            if (ImGui::BeginTable("##file_list", 4, fileTableFlags, ImVec2(0.0f, paneH - 24.0f))) {
                ImGui::TableSetupScrollFreeze(0, 1);
                ImGui::TableSetupColumn("Name",     ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("Size",     ImGuiTableColumnFlags_WidthFixed, 65.0f);
                ImGui::TableSetupColumn("Type",     ImGuiTableColumnFlags_WidthFixed, 100.0f);
                ImGui::TableSetupColumn("Modified", ImGuiTableColumnFlags_WidthFixed, 90.0f);
                ImGui::TableHeadersRow();

                for (const auto& f : kExplorerFiles) {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    if (f.isFolder) {
                        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(190, 130, 20, 255));
                        ImGui::Text("[DIR] %s", f.name);
                        ImGui::PopStyleColor();
                    } else {
                        ImGui::Text("  %s", f.name);
                    }

                    ImGui::TableSetColumnIndex(1);
                    ImGui::TextUnformatted(f.size);

                    ImGui::TableSetColumnIndex(2);
                    ImGui::TextUnformatted(f.type);

                    ImGui::TableSetColumnIndex(3);
                    ImGui::TextUnformatted(f.modified);
                }
                ImGui::EndTable();
            }

            ImGui::EndTable();
        }

        // Status Bar
        ImGui::Separator();
        const float sbH = 22.0f;
        const ImVec2 sbPos = ImGui::GetCursorScreenPos();
        const float availW = ImGui::GetContentRegionAvail().x;
        ImDrawList* dl = ImGui::GetWindowDrawList();

        RetroGfx::drawSunkenBorder(dl, sbPos, ImVec2(sbPos.x + availW * 0.6f, sbPos.y + sbH), RetroGfx::kGrayFace);
        RetroGfx::drawSunkenBorder(dl, ImVec2(sbPos.x + availW * 0.6f + 6.0f, sbPos.y), ImVec2(sbPos.x + availW, sbPos.y + sbH), RetroGfx::kGrayFace);

        const float ty = sbPos.y + (sbH - ImGui::GetFontSize()) * 0.5f;
        dl->AddText(ImVec2(sbPos.x + 8.0f, ty), RetroGfx::kBlack, "26 object(s)  (Disk free space: 248 MB)");
        dl->AddText(ImVec2(sbPos.x + availW * 0.6f + 14.0f, ty), RetroGfx::kBlack, "My Computer");

        ImGui::Dummy(ImVec2(0.0f, sbH + 2.0f));
    }
    ImGui::End();
}

// ============================================================================
// Settings Application (Windows Control Panel property sheet)
// ============================================================================

void SettingsApp::drawIcon(ImDrawList* dl, const ImVec2& center, float size) {
    RetroGfx::drawSettingsIcon(dl, center, size);
}

void SettingsApp::render(Compositor& compositor, bool* keepOpen) {
    const ImVec2 view = compositor.viewportSize();

    ImGui::SetNextWindowSize(ImVec2(560.0f, 480.0f), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(view.x * 0.5f - 280.0f, view.y * 0.5f - 240.0f), ImGuiCond_FirstUseEver);

    if (ImGui::Begin("Control Panel - System Settings", keepOpen, ImGuiWindowFlags_NoCollapse)) {

        Config& cfg = const_cast<Config&>(compositor.config());

        if (ImGui::BeginTabBar("##SettingsTabs", ImGuiTabBarFlags_None)) {
            // ── Display Properties ──
            if (ImGui::BeginTabItem("Display")) {
                ImGui::Spacing();
                ImGui::Text("Desktop Wallpaper & Color Scheme");
                ImGui::Separator();

                static int wpIndex = (cfg.wallpaperMode == "classic-teal") ? 0 :
                                     (cfg.wallpaperMode == "gradient")     ? 1 :
                                     (cfg.wallpaperMode == "bliss")        ? 2 :
                                     (cfg.wallpaperMode == "pattern")      ? 3 :
                                     (cfg.wallpaperMode == "image")        ? 5 : 4;

                const char* wpModes[] = {
                    "Classic Teal (Windows 95/98 default)",
                    "Retro Setup Gradient (Windows 2000)",
                    "Bliss Hills (Windows XP style)",
                    "Subtle Grid Pattern",
                    "Plain Solid Slate",
                    "Image (assets/wallpapers/...)"
                };

                if (ImGui::Combo("Wallpaper Style", &wpIndex, wpModes, IM_ARRAYSIZE(wpModes))) {
                    switch (wpIndex) {
                        case 0: cfg.wallpaperMode = "classic-teal"; break;
                        case 1: cfg.wallpaperMode = "gradient"; break;
                        case 2: cfg.wallpaperMode = "bliss"; break;
                        case 3: cfg.wallpaperMode = "pattern"; break;
                        case 4: cfg.wallpaperMode = "plain"; break;
                        case 5: cfg.wallpaperMode = "image"; break;
                    }
                }

                ImGui::Spacing();
                // Color Preview Box
                ImGui::Text("Preview:");
                const ImVec2 prevPos = ImGui::GetCursorScreenPos();
                const ImVec2 prevSize(ImGui::GetContentRegionAvail().x, 80.0f);
                ImDrawList* dl = ImGui::GetWindowDrawList();
                const ImU32 previewCol =
                    (wpIndex == 0) ? RetroGfx::kTealDesktop :
                    (wpIndex == 1) ? IM_COL32(16, 32, 72, 255) :
                    (wpIndex == 2) ? IM_COL32(50, 140, 220, 255) :
                    (wpIndex == 3) ? IM_COL32(28, 42, 60, 255) :
                    (wpIndex == 5) ? IM_COL32(80, 140, 80, 255) :
                                     IM_COL32(30, 30, 30, 255);
                RetroGfx::drawSunkenBorder(dl, prevPos,
                                           ImVec2(prevPos.x + prevSize.x, prevPos.y + prevSize.y),
                                           previewCol);
                ImGui::Dummy(prevSize);

                ImGui::Spacing();
                ImGui::Checkbox("Show Desktop Shortcut Icons", &cfg.showDesktopIcons);
                ImGui::EndTabItem();
            }

            // ── Taskbar & Start Menu ──
            if (ImGui::BeginTabItem("Taskbar")) {
                ImGui::Spacing();
                ImGui::Text("Taskbar Options");
                ImGui::Separator();

                static int tbPos = cfg.taskbarAtTop ? 0 : 1;
                if (ImGui::RadioButton("Dock to Top of Screen", &tbPos, 0)) {
                    cfg.taskbarAtTop = true;
                }
                if (ImGui::RadioButton("Dock to Bottom of Screen (Standard Windows)", &tbPos, 1)) {
                    cfg.taskbarAtTop = false;
                }

                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Text("Icon & Interface Scale (Fix for small icons)");
                ImGui::SliderFloat("UI Scale Factor", &cfg.uiScale, 1.0f, 2.0f, "%.2fx");
                ImGui::TextDisabled("Note: Adjusting scale updates icon sizing and font proportions.");
                ImGui::EndTabItem();
            }

            // ── System Information ──
            if (ImGui::BeginTabItem("System")) {
                ImGui::Spacing();
                ImGui::Text("System Information");
                ImGui::Separator();

                ImGui::BulletText("Operating System : CSOPESY Desktop-Style OS Emulator v1.0");
                ImGui::BulletText("Course           : Operating Systems (CSOPESY)");
                ImGui::BulletText("Project          : Semi-Major Output 2 / MO4");
                ImGui::BulletText("Target Platform  : Windows 10/11 (x64) via OpenGL 3.3 Core Profile");
                ImGui::BulletText("GUI Framework    : Dear ImGui v1.91.9b + GLFW 3.4");
                ImGui::BulletText("Processor Model  : Intel Pentium III / Modern x86_64 host");
                ImGui::BulletText("Total RAM        : %d MB", cfg.totalMemKb / 1024);

                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Text("Credits:");
                ImGui::TextDisabled("Prepared by: Gregory Cu | Created by: Dr. Neil Patrick Del Gallego");
                ImGui::EndTabItem();
            }

            // ── Config.txt Raw Parameters ──
            if (ImGui::BeginTabItem("Config Dump")) {
                ImGui::Spacing();
                ImGui::Text("Active Runtime Parameters (Loaded from config.txt):");
                ImGui::Separator();
                for (const std::string& line : cfg.dump()) {
                    ImGui::TextUnformatted(line.c_str());
                }
                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }

        // Bottom Dialog Action Buttons
        ImGui::Spacing();
        ImGui::Separator();
        const float btnW = 90.0f;
        const float rightAlignX = ImGui::GetContentRegionAvail().x - (btnW * 3.0f + 16.0f);
        if (rightAlignX > 0) ImGui::SetCursorPosX(ImGui::GetCursorPosX() + rightAlignX);

        if (ImGui::Button("OK", ImVec2(btnW, 26.0f))) {
            *keepOpen = false;
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(btnW, 26.0f))) {
            *keepOpen = false;
        }
        ImGui::SameLine();
        if (ImGui::Button("Apply", ImVec2(btnW, 26.0f))) {
            // Apply instantly
        }
    }
    ImGui::End();
}
