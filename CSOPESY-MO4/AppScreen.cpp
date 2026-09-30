#include "AppScreen.h"
#include "Compositor.h"

#include <algorithm>
#include <cstdio>

namespace {

const ImU32 kMuted = IM_COL32(146, 160, 186, 255);

// ── Placeholder directory tree (Files screen) ──
struct Node { const char* name; bool isFolder; };

const Node kRoots[] = {
    { "Home",            true  },
    { "System",          true  },
    { "csopesy",         true  },
    { "  bin",           true  },
    { "  config",        true  },
    { "  logs",          true  },
    { "  src",           true  },
    { "README.txt",      false },
    { "config.txt",      false },
};

const Node kChildren[] = {
    { "csopesy",         true  },
    { "bin",             true  },
    { "config",          true  },
    { "logs",            true  },
    { "src",             true  },
    { "third_party",     true  },
    { "build.bat",       false },
    { "config.txt",      false },
    { "csopesy.exe",     false },
};

// ── Placeholder file list (Files screen) ──
const Node kFiles[] = {
    { "src",             true  },
    { "third_party",     true  },
    { "AppScreen.cpp",   false },
    { "Compositor.cpp",  false },
    { "Config.cpp",      false },
    { "Desktop.cpp",     false },
    { "ProcessSimulator.cpp", false },
    { "TaskBar.cpp",     false },
    { "TaskManager.cpp", false },
    { "build.bat",       false },
    { "config.txt",      false },
    { "README.txt",      false },
};

} // namespace

void AppScreen::drawFiles(Compositor& compositor, bool* keepOpen) {
    const ImVec2 view = compositor.viewportSize();

    if (!ImGui::IsWindowAppearing()) {
        ImGui::SetNextWindowSize(ImVec2(680.0f, 440.0f), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowPos(ImVec2(view.x * 0.5f - 340.0f, view.y * 0.5f - 220.0f),
                                ImGuiCond_FirstUseEver);
    }
    ImGui::SetNextWindowBgAlpha(0.97f);

    if (ImGui::Begin("Files", keepOpen, ImGuiWindowFlags_NoCollapse)) {

        // Breadcrumb
        ImGui::TextColored(ImGui::ColorConvertU32ToFloat4(kMuted), "Path:");
        ImGui::SameLine();
        ImGui::TextUnformatted("Home  /  csopesy");

        ImGui::Separator();

        const ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg |
                                      ImGuiTableFlags_SizingStretchProp;

        if (ImGui::BeginTable("##filepanes", 2, flags, ImVec2(0.0f, 0.0f))) {
            ImGui::TableSetupColumn("Sidebar", ImGuiTableColumnFlags_WidthFixed, 190.0f);
            ImGui::TableSetupColumn("Contents", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableHeadersRow();

            // ── Sidebar ──
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            for (const Node& n : kRoots) {
                if (n.isFolder) {
                    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(126, 186, 255, 255));
                    ImGui::BulletText("%s", n.name);
                    ImGui::PopStyleColor();
                } else {
                    ImGui::Indent(0.0f);
                    ImGui::TextUnformatted(n.name);
                    ImGui::Unindent(0.0f);
                }
            }

            // ── Contents ──
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(1);
            for (const Node& n : kFiles) {
                if (n.isFolder) {
                    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(126, 186, 255, 255));
                    ImGui::BulletText("%s", n.name);
                    ImGui::PopStyleColor();
                } else {
                    ImGui::Indent(0.0f);
                    ImGui::TextUnformatted(n.name);
                    ImGui::Unindent(0.0f);
                }
            }
            ImGui::EndTable();
        }

        ImGui::Separator();
        ImGui::TextColored(ImGui::ColorConvertU32ToFloat4(kMuted),
            "Placeholder screen — this mock-up never touches the real filesystem.");
    }
    ImGui::End();
}

void AppScreen::drawSettings(Compositor& compositor, bool* keepOpen) {
    const ImVec2 view = compositor.viewportSize();

    if (!ImGui::IsWindowAppearing()) {
        ImGui::SetNextWindowSize(ImVec2(520.0f, 500.0f), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowPos(ImVec2(view.x * 0.5f - 260.0f, view.y * 0.5f - 250.0f),
                                ImGuiCond_FirstUseEver);
    }
    ImGui::SetNextWindowBgAlpha(0.97f);

    if (ImGui::Begin("Settings", keepOpen, ImGuiWindowFlags_NoCollapse)) {
        const Config& cfg = compositor.config();

        ImGui::TextColored(ImGui::ColorConvertU32ToFloat4(kMuted), "Appearance");
        ImGui::Separator();
        static bool wallpaperPattern = true;
        static bool showDate        = true;
        static bool animations      = true;
        ImGui::Checkbox("Draw wallpaper pattern", &wallpaperPattern);
        ImGui::Checkbox("Show date under the clock", &showDate);
        ImGui::Checkbox("Enable compositor animations", &animations);
        ImGui::Spacing();

        ImGui::TextColored(ImGui::ColorConvertU32ToFloat4(kMuted), "Shell");
        ImGui::Separator();
        static int  taskbarChoice = 0;   // 0 = top, 1 = bottom
        ImGui::Combo("Taskbar position", &taskbarChoice, "Top\0Bottom\0");
        static float iconScale = 1.0f;
        ImGui::SliderFloat("Icon scale", &iconScale, 0.75f, 1.50f, "%.2fx");
        ImGui::Spacing();

        ImGui::TextColored(ImGui::ColorConvertU32ToFloat4(kMuted), "Runtime parameters (read from config.txt)");
        ImGui::Separator();
        for (const std::string& line : cfg.dump())
            ImGui::TextUnformatted(line.c_str());

        ImGui::Spacing();
        ImGui::TextColored(ImGui::ColorConvertU32ToFloat4(kMuted), "Placeholder screen — controls are cosmetic only.");
    }
    ImGui::End();
}
