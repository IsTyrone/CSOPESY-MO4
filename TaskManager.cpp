/*
 *  CSOPESY Semi-Major Output 2 / MO4 — Desktop-Style OS Mock-up
 *  Requirement C — Task Manager Implementation (Windows Classic Style)
 */

#include "TaskManager.h"
#include "Compositor.h"
#include "RetroGfx.h"
#include <cstdio>
#include <algorithm>

namespace {

ImVec4 statusColor(ProcStatus s) {
    switch (s) {
        case ProcStatus::Running:   return ImVec4(0.00f, 0.50f, 0.15f, 1.00f);
        case ProcStatus::Idle:      return ImVec4(0.60f, 0.45f, 0.00f, 1.00f);
        case ProcStatus::Suspended: return ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
    }
    return ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
}

} // namespace

TaskManagerApp::TaskManagerApp() {
    m_cpuHistory.resize(60, 0.0f);
}

void TaskManagerApp::drawIcon(ImDrawList* dl, const ImVec2& center, float size) {
    RetroGfx::drawTaskManagerIcon(dl, center, size);
}

void TaskManagerApp::render(Compositor& compositor, bool* keepOpen) {
    const ImVec2 view = compositor.viewportSize();

    ImGui::SetNextWindowSize(ImVec2(680.0f, 480.0f), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(view.x * 0.5f - 340.0f, view.y * 0.5f - 240.0f), ImGuiCond_FirstUseEver);

    // Update CPU history
    const ProcessSimulator& sim = compositor.simulator();
    const float currentCpu = sim.totalCpuPercent();
    m_historyTimer += ImGui::GetIO().DeltaTime;
    if (m_historyTimer >= 0.1f) {
        m_historyTimer = 0.0f;
        m_cpuHistory.erase(m_cpuHistory.begin());
        m_cpuHistory.push_back(currentCpu);
    }

    if (ImGui::Begin("Windows Task Manager", keepOpen, ImGuiWindowFlags_NoCollapse)) {

        // Classic Menu Bar / Tabs
        if (ImGui::BeginTabBar("##TaskMgrTabs", ImGuiTabBarFlags_None)) {
            if (ImGui::BeginTabItem("Applications")) {
                m_currentTab = 0;
                renderApplicationsTab(compositor);
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Processes")) {
                m_currentTab = 1;
                renderProcessesTab(compositor);
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Performance")) {
                m_currentTab = 2;
                renderPerformanceTab(compositor);
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }

        // Sunken Status Bar at Bottom
        ImGui::Separator();
        const float sbH = 22.0f;
        const ImVec2 sbPos = ImGui::GetCursorScreenPos();
        const float availW = ImGui::GetContentRegionAvail().x;
        ImDrawList* dl = ImGui::GetWindowDrawList();

        char sb1[48], sb2[48], sb3[64];
        std::snprintf(sb1, sizeof(sb1), "Processes: %d", static_cast<int>(sim.processes().size()));
        std::snprintf(sb2, sizeof(sb2), "CPU Usage: %.1f%%", static_cast<double>(currentCpu));
        std::snprintf(sb3, sizeof(sb3), "Mem: %dMB / %dMB", sim.usedMemoryKb() / 1024, sim.totalMemoryKb() / 1024);

        const float partW = (availW - 12.0f) / 3.0f;
        RetroGfx::drawSunkenBorder(dl, ImVec2(sbPos.x, sbPos.y), ImVec2(sbPos.x + partW, sbPos.y + sbH), RetroGfx::kGrayFace);
        RetroGfx::drawSunkenBorder(dl, ImVec2(sbPos.x + partW + 6.0f, sbPos.y), ImVec2(sbPos.x + partW * 2.0f + 6.0f, sbPos.y + sbH), RetroGfx::kGrayFace);
        RetroGfx::drawSunkenBorder(dl, ImVec2(sbPos.x + (partW + 6.0f) * 2.0f, sbPos.y), ImVec2(sbPos.x + availW, sbPos.y + sbH), RetroGfx::kGrayFace);

        const float ty = sbPos.y + (sbH - ImGui::GetFontSize()) * 0.5f;
        dl->AddText(ImVec2(sbPos.x + 8.0f, ty), RetroGfx::kBlack, sb1);
        dl->AddText(ImVec2(sbPos.x + partW + 14.0f, ty), RetroGfx::kBlack, sb2);
        dl->AddText(ImVec2(sbPos.x + (partW + 6.0f) * 2.0f + 8.0f, ty), RetroGfx::kBlack, sb3);

        ImGui::Dummy(ImVec2(0.0f, sbH + 2.0f));
    }
    ImGui::End();
}

void TaskManagerApp::renderProcessesTab(Compositor& compositor) {
    const ProcessSimulator& sim = compositor.simulator();

    const ImGuiTableFlags flags =
        ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable |
        ImGuiTableFlags_ScrollY | ImGuiTableFlags_SizingStretchProp;

    const float tableHeight = ImGui::GetContentRegionAvail().y - 42.0f;
    if (ImGui::BeginTable("##proc_table", 5, flags, ImVec2(0.0f, tableHeight))) {
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableSetupColumn("Image Name", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("PID",        ImGuiTableColumnFlags_WidthFixed, 65.0f);
        ImGui::TableSetupColumn("CPU",        ImGuiTableColumnFlags_WidthFixed, 75.0f);
        ImGui::TableSetupColumn("Mem Usage",  ImGuiTableColumnFlags_WidthFixed, 100.0f);
        ImGui::TableSetupColumn("Status",     ImGuiTableColumnFlags_WidthFixed, 95.0f);
        ImGui::TableHeadersRow();

        for (const Process& p : sim.processes()) {
            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);
            ImGui::TextUnformatted(p.name.c_str());

            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%d", p.pid);

            ImGui::TableSetColumnIndex(2);
            ImGui::Text("%.1f%%", static_cast<double>(p.cpu));

            ImGui::TableSetColumnIndex(3);
            ImGui::Text("%d K", p.memKb);

            ImGui::TableSetColumnIndex(4);
            ImGui::TextColored(statusColor(p.status), "%s", ProcessSimulator::statusName(p.status));
        }
        ImGui::EndTable();
    }

    ImGui::Spacing();
    if (ImGui::Button("End Process", ImVec2(110.0f, 26.0f))) {
        // Safe UI feedback
    }
    ImGui::SameLine();
    ImGui::TextDisabled("(Select a process to inspect or terminate)");
}

void TaskManagerApp::renderPerformanceTab(Compositor& compositor) {
    const ProcessSimulator& sim = compositor.simulator();
    const float currentCpu = sim.totalCpuPercent();
    const int usedMb = sim.usedMemoryKb() / 1024;
    const int totalMb = sim.totalMemoryKb() / 1024;

    const float availW = ImGui::GetContentRegionAvail().x;
    const float availH = ImGui::GetContentRegionAvail().y - 36.0f;

    // CPU Usage History Box
    ImGui::Text("CPU Usage History (%.1f%%)", static_cast<double>(currentCpu));
    const ImVec2 graphPos = ImGui::GetCursorScreenPos();
    const ImVec2 graphSize(availW, std::min(160.0f, availH * 0.55f));

    ImDrawList* dl = ImGui::GetWindowDrawList();
    RetroGfx::drawSunkenBorder(dl, graphPos, ImVec2(graphPos.x + graphSize.x, graphPos.y + graphSize.y),
                              IM_COL32(0, 24, 8, 255));

    // Green CRT Grid lines
    const float stepX = graphSize.x / 10.0f;
    const float stepY = graphSize.y / 5.0f;
    for (int i = 1; i < 10; ++i) {
        dl->AddLine(ImVec2(graphPos.x + stepX * i, graphPos.y),
                    ImVec2(graphPos.x + stepX * i, graphPos.y + graphSize.y),
                    IM_COL32(0, 70, 20, 255));
    }
    for (int i = 1; i < 5; ++i) {
        dl->AddLine(ImVec2(graphPos.x, graphPos.y + stepY * i),
                    ImVec2(graphPos.x + graphSize.x, graphPos.y + stepY * i),
                    IM_COL32(0, 70, 20, 255));
    }

    // Rolling waveform
    if (m_cpuHistory.size() >= 2) {
        const float dx = graphSize.x / static_cast<float>(m_cpuHistory.size() - 1);
        for (size_t i = 0; i < m_cpuHistory.size() - 1; ++i) {
            const float y0 = graphPos.y + graphSize.y - (m_cpuHistory[i] / 100.0f) * (graphSize.y - 6.0f) - 3.0f;
            const float y1 = graphPos.y + graphSize.y - (m_cpuHistory[i + 1] / 100.0f) * (graphSize.y - 6.0f) - 3.0f;
            dl->AddLine(ImVec2(graphPos.x + dx * i, y0),
                        ImVec2(graphPos.x + dx * (i + 1), y1),
                        IM_COL32(0, 255, 70, 255), 2.0f);
        }
    }

    ImGui::Dummy(graphSize);
    ImGui::Spacing();

    // Memory Usage Bar
    ImGui::Text("MEM Usage History (%d MB of %d MB)", usedMb, totalMb);
    const float memFrac = static_cast<float>(sim.usedMemoryKb()) / static_cast<float>(sim.totalMemoryKb());
    ImGui::ProgressBar(memFrac, ImVec2(availW, 24.0f));

    ImGui::Spacing();
    ImGui::Columns(2, "##perf_stats", false);
    ImGui::Text("Totals:");
    ImGui::BulletText("Handles: 4120");
    ImGui::BulletText("Threads: %d", sim.runningCount() * 3 + 12);
    ImGui::BulletText("Processes: %d", static_cast<int>(sim.processes().size()));
    ImGui::NextColumn();
    ImGui::Text("Physical Memory (K):");
    ImGui::BulletText("Total: %d", sim.totalMemoryKb());
    ImGui::BulletText("Available: %d", sim.totalMemoryKb() - sim.usedMemoryKb());
    ImGui::BulletText("System Cache: 38400");
    ImGui::Columns(1);
}

void TaskManagerApp::renderApplicationsTab(Compositor& compositor) {
    ImGui::Text("Running Applications:");
    const ImGuiTableFlags flags =
        ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY | ImGuiTableFlags_SizingStretchProp;

    const float tableHeight = ImGui::GetContentRegionAvail().y - 42.0f;
    if (ImGui::BeginTable("##app_table", 2, flags, ImVec2(0.0f, tableHeight))) {
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableSetupColumn("Task", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Status", ImGuiTableColumnFlags_WidthFixed, 120.0f);
        ImGui::TableHeadersRow();

        for (const std::string& title : compositor.windowManager().openAppTitles()) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("%s", title.c_str());

            ImGui::TableSetColumnIndex(1);
            ImGui::TextColored(ImVec4(0.0f, 0.6f, 0.2f, 1.0f), "Running");
        }
        ImGui::EndTable();
    }

    ImGui::Spacing();
    if (ImGui::Button("Switch To", ImVec2(100.0f, 26.0f))) {}
    ImGui::SameLine();
    if (ImGui::Button("New Task...", ImVec2(100.0f, 26.0f))) {}
    ImGui::SameLine();
    if (ImGui::Button("End Task", ImVec2(100.0f, 26.0f))) {}
}
