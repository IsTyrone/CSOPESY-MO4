/*
* CSOPESY Semi-Major Output 2 / MO4 — Desktop-Style OS Mock-up
* Bonus App - Calculator
*/

#include "CalculatorApp.h"
#include "Compositor.h"
#include "RetroGfx.h"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>

// ============================================================================
// Calculator Logic
// ============================================================================

void CalculatorApp::pressDigit(char digit) {
  if (m_clearOnNext || m_display == "0") {
    m_display = std::string(1, digit);
    m_clearOnNext = false;
    m_hasDecimal = false;
  } else {
    if (m_display.size() < 16) { // Limit display length
      m_display += digit;
    }
  }
  m_newAfterOp = false;
}

void CalculatorApp::pressOperator(char op) {
  if (!m_newAfterOp) {
    pressEquals();
  }
  m_accumulator = std::atof(m_display.c_str());
  m_pendingOp = op;
  m_clearOnNext = true;
  m_newAfterOp = true;
}

void CalculatorApp::pressEquals() {
  double current = std::atof(m_display.c_str());
  double result = current;

  switch (m_pendingOp) {
  case '+':
    result = m_accumulator + current;
    break;
  case '-':
    result = m_accumulator - current;
    break;
  case '*':
    result = m_accumulator * current;
    break;
  case '/':
    if (current != 0.0) {
      result = m_accumulator / current;
    } else {
      m_display = "Cannot divide by zero";
      m_clearOnNext = true;
      m_pendingOp = '\0';
      m_newAfterOp = true;
      return;
    }
    break;
  default:
    m_pendingOp = '\0';
    m_clearOnNext = true;
    m_newAfterOp = true;
    return;
  }

  // Format the result: use integer format when possible, otherwise
  // floating-point
  if (result == static_cast<double>(static_cast<long long>(result)) &&
      std::abs(result) < 1e15) {
    char buf[64];
    std::snprintf(buf, sizeof(buf), "%lld", static_cast<long long>(result));
    m_display = buf;
  } else {
    char buf[64];
    std::snprintf(buf, sizeof(buf), "%.10g", result);
    m_display = buf;
  }

  m_pendingOp = '\0';
  m_clearOnNext = true;
  m_hasDecimal = (m_display.find('.') != std::string::npos);
  m_newAfterOp = true;
}

void CalculatorApp::pressClear() {
  m_display = "0";
  m_accumulator = 0.0;
  m_pendingOp = '\0';
  m_clearOnNext = true;
  m_hasDecimal = false;
  m_newAfterOp = true;
}

void CalculatorApp::pressClearEntry() {
  m_display = "0";
  m_clearOnNext = true;
  m_hasDecimal = false;
}

void CalculatorApp::pressBackspace() {
  if (m_clearOnNext)
    return;
  if (m_display.size() > 1) {
    if (m_display.back() == '.')
      m_hasDecimal = false;
    m_display.pop_back();
  } else {
    m_display = "0";
  }
}

void CalculatorApp::pressSign() {
  if (m_display == "0" || m_display.find("Cannot") != std::string::npos)
    return;
  if (m_display[0] == '-') {
    m_display = m_display.substr(1);
  } else {
    m_display = "-" + m_display;
  }
}

void CalculatorApp::pressDot() {
  if (m_clearOnNext) {
    m_display = "0.";
    m_clearOnNext = false;
    m_hasDecimal = true;
    m_newAfterOp = false;
    return;
  }
  if (!m_hasDecimal) {
    m_display += '.';
    m_hasDecimal = true;
  }
}

// ============================================================================
// Icon
// ============================================================================

void CalculatorApp::drawIcon(ImDrawList *dl, const ImVec2 &center, float size) {
  RetroGfx::drawCalculatorIcon(dl, center, size);
}

// ============================================================================
// Render — Windows 95/98 Calculator UI
// ============================================================================

void CalculatorApp::render(Compositor &compositor, bool *keepOpen) {
  ImGuiWindowFlags flags =
      ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize;
  if (compositor.windowManager().isAppMaximized(m_id)) {
    flags |= ImGuiWindowFlags_NoMove;
    flags &= ~ImGuiWindowFlags_NoResize;
  }

  if (ImGui::Begin("Calculator", nullptr, flags)) {
    compositor.windowManager().renderTitleBarButtons(compositor, m_id,
                                                     keepOpen);

    const float scale = compositor.effectiveScale();

    // ── Sunken LCD Display ──
    const ImVec2 displayPos = ImGui::GetCursorScreenPos();
    const float displayW = ImGui::GetContentRegionAvail().x;
    const float displayH =
        std::max(30.0f * scale, ImGui::GetFontSize() + 14.0f * scale);
    ImDrawList *dl = ImGui::GetWindowDrawList();

    RetroGfx::drawSunkenBorder(
        dl, displayPos,
        ImVec2(displayPos.x + displayW, displayPos.y + displayH),
        IM_COL32(255, 255, 255, 255));

    // Right-align the display text inside the LCD
    const ImVec2 textSize = ImGui::CalcTextSize(m_display.c_str());
    const float textX = displayPos.x + displayW - textSize.x - 8.0f * scale;
    const float textY = displayPos.y + (displayH - textSize.y) * 0.5f;
    dl->AddText(ImVec2(textX, textY), RetroGfx::kBlack, m_display.c_str());

    ImGui::Dummy(ImVec2(0.0f, displayH + 4.0f * scale));

    // ── Button Grid ──
    // Layout (5 rows × 4 columns):
    //   [  C  ] [ CE  ] [ Bksp ] [  /  ]
    //   [  7  ] [  8  ] [  9   ] [  *  ]
    //   [  4  ] [  5  ] [  6   ] [  -  ]
    //   [  1  ] [  2  ] [  3   ] [  +  ]
    //   [ +/- ] [  0  ] [  .   ] [  =  ]

    const float spacing = ImGui::GetStyle().ItemSpacing.x;
    const float btnW = (displayW - 3.0f * spacing) / 4.0f;
    const float btnH =
        std::max(28.0f * scale, ImGui::GetFontSize() + 10.0f * scale);

    // Helper: styled calculator button
    auto calcBtn = [&](const char *label,
                       ImU32 textColor = RetroGfx::kBlack) -> bool {
      // Use a distinct red color for operator keys by pushing style
      (void)textColor; // Color differentiation handled by label convention
      return ImGui::Button(label, ImVec2(btnW, btnH));
    };

    // Row 1: C  CE  Bksp  /
    ImGui::PushStyleColor(ImGuiCol_Text,
                          ImVec4(0.60f, 0.0f, 0.0f, 1.0f)); // Red for top row
    if (calcBtn("C"))
      pressClear();
    ImGui::SameLine();
    if (calcBtn("CE"))
      pressClearEntry();
    ImGui::SameLine();
    if (calcBtn("<--"))
      pressBackspace();
    ImGui::PopStyleColor();
    ImGui::SameLine();
    ImGui::PushStyleColor(
        ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.60f, 1.0f)); // Blue for operators
    if (calcBtn("/"))
      pressOperator('/');
    ImGui::PopStyleColor();

    // Row 2: 7  8  9  *
    if (calcBtn("7"))
      pressDigit('7');
    ImGui::SameLine();
    if (calcBtn("8"))
      pressDigit('8');
    ImGui::SameLine();
    if (calcBtn("9"))
      pressDigit('9');
    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.60f, 1.0f));
    if (calcBtn("*"))
      pressOperator('*');
    ImGui::PopStyleColor();

    // Row 3: 4  5  6  -
    if (calcBtn("4"))
      pressDigit('4');
    ImGui::SameLine();
    if (calcBtn("5"))
      pressDigit('5');
    ImGui::SameLine();
    if (calcBtn("6"))
      pressDigit('6');
    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.60f, 1.0f));
    if (calcBtn("-"))
      pressOperator('-');
    ImGui::PopStyleColor();

    // Row 4: 1  2  3  +
    if (calcBtn("1"))
      pressDigit('1');
    ImGui::SameLine();
    if (calcBtn("2"))
      pressDigit('2');
    ImGui::SameLine();
    if (calcBtn("3"))
      pressDigit('3');
    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.60f, 1.0f));
    if (calcBtn("+"))
      pressOperator('+');
    ImGui::PopStyleColor();

    // Row 5: +/-  0  .  =
    if (calcBtn("+/-"))
      pressSign();
    ImGui::SameLine();
    if (calcBtn("0"))
      pressDigit('0');
    ImGui::SameLine();
    if (calcBtn("."))
      pressDot();
    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.60f, 1.0f));
    if (calcBtn("="))
      pressEquals();
    ImGui::PopStyleColor();

    // ── Exit Button ──
    // Explicit close control for the calculator. The window's title-bar X
    // accomplishes the same thing, this just makes it discoverable.
    ImGui::Separator();
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.60f, 0.0f, 0.0f, 1.0f));
    if (ImGui::Button("Exit", ImVec2(displayW, btnH))) {
      if (keepOpen)
        *keepOpen = false;
    }
    ImGui::PopStyleColor();
  }
  ImGui::End();
}
