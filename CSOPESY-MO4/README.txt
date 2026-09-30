CSOPESY Semi-Major Output 2 — Desktop-Style OS Mock-up
=======================================================

Description
-----------
A desktop-style operating-system mock-up built with C++17, GLFW 3.4, OpenGL 3.3,
and Dear ImGui v1.91.9b.  The application renders a compositor-driven desktop
environment with a taskbar, launcher icons, application windows, a real-time
clock, wallpaper, and a PWR (power) button to shut down the mock OS.

Features
--------
  • Wallpaper layer — full-window gradient/pattern, drawn first every frame
  • Real-time clock — updates every frame, displayed on the desktop
  • Taskbar — fixed bar (top or bottom per config) with three icon launchers:
      - Files — placeholder file-browser screen
      - Settings — placeholder settings screen
      - Task Manager — live process table driven by the process simulator
  • Running-app indicators — icon turns green + pip; chip appears in the bar
  • Toggle open/close — second icon click closes the window
  • PWR button — sole exit path; closes the application cleanly (exit code 0)
  • config.txt — all parameters (resolution, process count, taskbar position,
    wallpaper mode, etc.) can be changed without recompiling

Building
--------
Prerequisites:  see prerequisite.md

  1.  Open a terminal (Command Prompt or PowerShell) in this folder.
  2.  Run:
          build.bat
  3.  On success the output is csopesy.exe in the same folder.

  build.bat links the MinGW runtime statically, so csopesy.exe is
  self-contained and needs no additional DLLs.

To clean the build artefacts:
          build.bat clean

Running
-------
      csopesy.exe

The mock-up reads config.txt at startup.  Edit that file to change the window
size, process count, memory limits, taskbar position, wallpaper mode, etc.

A window always opens at the configured size, titled
"CSOPESY SMO2 - Desktop-Style OS Mock-up".  If the process starts but no
window appears, the exe was linked against the MinGW runtime DLLs and they are
missing from PATH -- see the troubleshooting table in prerequisite.md.

Controls
--------
  • Click a launcher icon to open its window; click the same icon again to close it.
  • Click the X on a window's title bar to close it.
  • Click the PWR button to shut the mock OS down (this is the only exit path).

Project Structure
-----------------
  main.cpp               Entry point
  Compositor.cpp / .h    Host window, ImGui backends, layer stack, app registry
  Desktop.cpp / .h       Wallpaper, clock, PWR button
  TaskBar.cpp / .h       Taskbar strip, launcher icons, running-app chips
  TaskManager.cpp / .h   Task Manager window (live process table)
  AppScreen.cpp / .h     Files and Settings placeholder screens
  ProcessSimulator.cpp / .h   Simulated process model (CPU, memory readings)
  Config.cpp / .h        Runtime parameters loaded from config.txt
  config.txt             User-editable runtime configuration
  build.bat              One-step build script (MinGW g++)
  prerequisite.md        Software prerequisites and setup guide
  third_party/           Vendored GLFW and Dear ImGui sources

Authors
-------
  CSOPESY — Operating Systems course project
