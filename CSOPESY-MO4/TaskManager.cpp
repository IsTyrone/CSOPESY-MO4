#include "TaskManager.h"
#include "Compositor.h"

#include <algorithm>
#include <cstdio>

namespace {

const ImU32 kStatLabel = IM_COL32(142, 156, 182, 255);
const ImU32 kStatValue = IM_COL32(236, 242, 252, 255);

void statTile(ImDrawList* dl, float x, float y, float w,
              const char* label, const char* value, ImU32 accent) {
    const float h = 46.0f;
    dl->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), IM_COL32(28, 33, 44, 255), 5.0f);
    dl->AddRectFilled(ImVec2(x, y), ImVec2(x + 3.0f, y + h), accent, 2.0f);
    dl->AddText(ImVec2(x + 12.0f, y + 8.0f), kStatLabel, label);
    dl->AddText(ImVec2(x + 12.0f, y + 24.0f), kStatValue, value);
}

ImVec4 statusColor(ProcStatus s) {
    switch (s) {
        case ProcStatus::Running:   return ImVec4(0.38f, 0.86f, 0.67f, 1.00f);
        case ProcStatus::Idle:      return ImVec4(0.91f, 0.77f, 0.41f, 1.00f);
        case ProcStatus::Suspended: return ImVec4(0.59f, 0.62f, 0.69f, 1.00f);
    }
    return ImVec4(0.78f, 0.78f, 0.78f, 1.00f);
}

} // namespace

void TaskManager::draw(Compositor& compositor, bool* keepOpen) {
    const ImVec2 view = compositor.viewportSize();

    ImGui::SetNextWindowSize(ImVec2(720.0f, 470.0f), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(view.x * 0.5f - 360.0f, view.y * 0.5f - 235.0f),
                            ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowBgAlpha(0.97f);

    const ProcessSimulator& sim = compositor.simulator();

    if (ImGui::Begin("Task Manager", keepOpen, ImGuiWindowFlags_NoCollapse)) {

        // ── Summary tiles ──
        const int   count = static_cast<int>(sim.processes().size());
        const float cpu   = sim.totalCpuPercent();
        const int   usedMb = sim.usedMemoryKb() / 1024;
        const int   totalMb = sim.totalMemoryKb() / 1024;

        char bufProcesses[32], bufCpu[48], bufMem[64], bufUptime[48];
        std::snprintf(bufProcesses, sizeof(bufProcesses), "%d", count);
        std::snprintf(bufCpu,       sizeof(bufCpu),       "%.1f %%", static_cast<double>(cpu));
        std::snprintf(bufMem,       sizeof(bufMem),       "%d / %d MB", usedMb, totalMb);
        std::snprintf(bufUptime,    sizeof(bufUptime),    "%d running", sim.runningCount());

        ImDrawList* dl = ImGui::GetWindowDrawList();
        const ImVec2 org = ImGui::GetCursorScreenPos();
        const float  gap = 8.0f;
        const float  tw  = std::max(120.0f, (ImGui::GetContentRegionAvail().x - gap * 3.0f) / 4.0f);

        statTile(dl, org.x,                        org.y, tw, "Processes", bufProcesses, IM_COL32(96, 152, 232, 255));
        statTile(dl, org.x + (tw + gap),           org.y, tw, "CPU Usage",  bufCpu,       IM_COL32(96, 220, 170, 255));
        statTile(dl, org.x + (tw + gap) * 2.0f,    org.y, tw, "Memory",     bufMem,       IM_COL32(232, 176, 104, 255));
        statTile(dl, org.x + (tw + gap) * 3.0f,    org.y, tw, "Scheduler",  bufUptime,    IM_COL32(178, 150, 232, 255));
        ImGui::Dummy(ImVec2(0.0f, kHeaderH + 6.0f));

        // ── The process table ──
        const ImGuiTableFlags flags =
            ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable |
            ImGuiTableFlags_ScrollY | ImGuiTableFlags_SizingStretchProp;

        if (ImGui::BeginTable("##processes", 5, flags, ImVec2(0.0f, 0.0f))) {
            ImGui::TableSetupScrollFreeze(0, 1);
            ImGui::TableSetupColumn("PID",     ImGuiTableColumnFlags_WidthFixed, 70.0f);
            ImGui::TableSetupColumn("Process", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Status",  ImGuiTableColumnFlags_WidthFixed, 110.0f);
            ImGui::TableSetupColumn("CPU",     ImGuiTableColumnFlags_WidthFixed, 90.0f);
            ImGui::TableSetupColumn("Memory",  ImGuiTableColumnFlags_WidthFixed, 110.0f);
            ImGui::TableHeadersRow();

            for (const Process& p : sim.processes()) {
                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%d", p.pid);

                ImGui::TableSetColumnIndex(1);
                ImGui::TextUnformatted(p.name.c_str());

                ImGui::TableSetColumnIndex(2);
                ImGui::TextColored(statusColor(p.status), "%s", ProcessSimulator::statusName(p.status));

                ImGui::TableSetColumnIndex(3);
                ImGui::Text("%.1f %%", static_cast<double>(p.cpu));

                ImGui::TableSetColumnIndex(4);
                ImGui::Text("%d MB", p.memKb / 1024);
            }
            ImGui::EndTable();
        }

        // ── Recent activity ──
        ImGui::Separator();
        ImGui::TextColored(ImGui::ColorConvertU32ToFloat4(kStatLabel), "Recent activity");
        for (const std::string& e : sim.eventLog())
            ImGui::BulletText("%s", e.c_str());

        ImGui::Separator();
        if (ImGui::Button("End Task")) {
            // Placeholder: the spec only requires the table itself.
            ImGui::TextColored(ImGui::ColorConvertU32ToFloat4(kStatLabel), "  (placeholder — no process is actually terminated)");
        }
        ImGui::SameLine();
        if (ImGui::Button("Refresh"))
            ImGui::TextColored(ImGui::ColorConvertU32ToFloat4(kStatLabel), "  (values refresh automatically every frame)");
    }
    ImGui::End();
}
