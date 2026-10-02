/*
 *  CSOPESY Semi-Major Output 2 / MO4 — Desktop-Style OS Mock-up
 *  Requirement B — Files (Explorer) & Settings (Control Panel) in Windows Classic Style
 */

#pragma once
#include "IAppWindow.h"

class Compositor;

class FilesApp : public IAppWindow {
public:
    const std::string& id() const override { return m_id; }
    const std::string& title() const override { return m_title; }

    void render(Compositor& compositor, bool* keepOpen) override;
    void drawIcon(ImDrawList* dl, const ImVec2& center, float size) override;

private:
    std::string m_id = "files";
    std::string m_title = "Exploring - C:\\CSOPESY";
};

class SettingsApp : public IAppWindow {
public:
    const std::string& id() const override { return m_id; }
    const std::string& title() const override { return m_title; }

    void render(Compositor& compositor, bool* keepOpen) override;
    void drawIcon(ImDrawList* dl, const ImVec2& center, float size) override;

private:
    std::string m_id = "settings";
    std::string m_title = "Control Panel";
    int m_activeTab = 0;
};
