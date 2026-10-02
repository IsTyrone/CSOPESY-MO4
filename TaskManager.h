/*
 *  CSOPESY Semi-Major Output 2 / MO4 — Desktop-Style OS Mock-up
 *  Requirement C — Task Manager (Windows Classic Style)
 */

#pragma once
#include "IAppWindow.h"
#include <vector>

class Compositor;

class TaskManagerApp : public IAppWindow {
public:
    TaskManagerApp();
    virtual ~TaskManagerApp() = default;

    const std::string& id() const override { return m_id; }
    const std::string& title() const override { return m_title; }

    void render(Compositor& compositor, bool* keepOpen) override;
    void drawIcon(ImDrawList* dl, const ImVec2& center, float size) override;

private:
    void renderProcessesTab(Compositor& compositor);
    void renderPerformanceTab(Compositor& compositor);
    void renderApplicationsTab(Compositor& compositor);

    std::string m_id = "taskmanager";
    std::string m_title = "Task Manager";
    int m_currentTab = 1; // 0 = Applications, 1 = Processes, 2 = Performance

    // Rolling history for performance tab graph
    std::vector<float> m_cpuHistory;
    float m_historyTimer = 0.0f;
};
