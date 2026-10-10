/*
* CSOPESY Semi-Major Output 2 / MO4 — Desktop-Style OS Mock-up
* Bonus App - Calculator
*/
#pragma once
#include "IAppWindow.h"
#include <string>

class Compositor;

class CalculatorApp : public IAppWindow {
public:
    const std::string& id() const override { return m_id; }
    const std::string& title() const override { return m_title; }

    void render(Compositor& compositor, bool* keepOpen) override;
    void drawIcon(ImDrawList* dl, const ImVec2& center, float size) override;

private:
    void pressDigit(char digit);
    void pressOperator(char op);
    void pressEquals();
    void pressClear();
    void pressClearEntry();
    void pressBackspace();
    void pressSign();
    void pressDot();

    std::string m_id    = "calculator";
    std::string m_title = "Calculator";

    std::string m_display    = "0";
    double      m_accumulator = 0.0;
    char        m_pendingOp   = '\0';
    bool        m_clearOnNext = true;
    bool        m_hasDecimal  = false;
    bool        m_newAfterOp  = true;
};
