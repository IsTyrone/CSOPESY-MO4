===============================================================================
CSOPESY Semi-Major Output 2 / MO4 — Desktop-Style OS Mock-up
Windows Classic (95 / 98 / 2000) Display Edition
De La Salle University — Operating Systems
===============================================================================

ENTRY CLASS / FILE
------------------
  Entry File : main.cpp
  Function   : int main()
  Role       : Boots the Compositor subsystem, initializes GLFW and OpenGL 3.3
               Core Profile, registers polymorphic applications via WindowManager,
               and executes the main frame render loop.


DESCRIPTION
-----------
A compositor-driven, Windows Classic (95/98/2000) desktop operating-system
mock-up built with C++17, Dear ImGui v1.91.9b, GLFW 3.4, and OpenGL 3.3 Core.

Features:
  • Classic Windows Desktop:
      - Authentic #008080 teal wallpaper (or 2000 gradient, Bliss, pattern)
      - Large desktop shortcut icons (Files, Settings, Task Manager)
      - Retro beveled LCD/LED desktop clock (live updating time and date)
      - Dedicated 3D beveled PWR button (sole designated exit path, exit code 0)
  • Authentic Windows Classic Taskbar:
      - Raised 3D white highlight shelf docked at bottom (or top)
      - Classic "Start" button with Windows 4-color flag
      - Authentic Start Menu with blue vertical stripe and app shortcuts
      - Quick Launch bar with scalable launcher icons
      - Windows 95/98 active window tabs (sunken when focused, raised when idle)
      - System Tray (notification area) with speaker, network, and live clock
  • Three Authentic Applications:
      1. Files (Explorer)     : Two-pane file browser with address bar & tree
      2. Settings (Control)   : Property sheet dialog with Display, Taskbar, System
      3. Task Manager         : Classic tabbed Task Manager with Applications,
                                Processes table, and real-time CRT CPU graph!
  • Decoupled Architecture:
      - Polymorphic IAppWindow interface and WindowManager class
      - Centralized RetroGfx 3D bevel and vector icon engine
  • Scalable Display & UI:
      - Configurable "ui-scale" parameter to eliminate small icons on high-res screens


PREREQUISITES
-------------
  • OS: Windows 10 or 11 (64-bit)
  • Compiler: MinGW-w64 g++ (GCC 8+ with C++17 support) on system PATH
  • Graphics: GPU supporting OpenGL 3.3 Core Profile
  • Note: GLFW 3.4 and Dear ImGui are pre-packaged under third_party/;
          no additional external downloads are required.

To verify g++ is installed and accessible:
    g++ --version


HOW TO BUILD PROPERLY
---------------------
1. Open PowerShell or Command Prompt in the project folder:
     cd CSOPESY-MO4

2. Method A (One-step build script, no CMake required):
     In PowerShell:
         .\build.bat

     In Command Prompt (CMD):
         build.bat

   Method B (Using CMake, if CMake is installed):
     In PowerShell:
         cmake -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
         cmake --build build --config Release

     Or simply:
         .\build.bat cmake

3. To clean build artefacts:
     .\build.bat clean   (PowerShell)
     build.bat clean     (Command Prompt)


HOW TO RUN PROPERLY
-------------------
IMPORTANT (PowerShell Users):
In Windows PowerShell, executables in the current working directory MUST be
prefixed with ".\" or they will fail with a "term not recognized" error.

  In PowerShell:
      .\csopesy.exe

  In Command Prompt (CMD):
      csopesy.exe

A window titled "CSOPESY SMO2 — Desktop-Style OS Mock-up" will open at the
dimensions specified in config.txt (default: 1280x800).


USER CONTROLS
-------------
  • Start Button      : Click "Start" on the taskbar to toggle the Start Menu.
  • Desktop Shortcuts : Click any desktop icon to open or toggle an application.
  • Quick Launch      : Click launcher icons on the taskbar.
  • Window Tabs       : Click taskbar tabs to switch focus between windows.
  • Shutdown Mock OS  : Click the red PWR button on the desktop or via
                        Start -> Shut Down... (Clean exit with exit code 0).


CONFIGURATION & FIXING SMALL ICONS (config.txt)
-----------------------------------------------
Edit config.txt in any text editor to modify parameters without recompiling:

  framebuffer-width    : Window width in pixels (min 800)
  framebuffer-height   : Window height in pixels (min 600)
  process-count        : Number of simulated processes (default 6)
  min-cpu / max-cpu    : CPU percentage range per process
  min-mem-kb / max-mem : Memory range in KB per process
  total-mem-kb         : Total system simulated memory in KB
  update-interval-ms   : Refresh cadence for simulated readings (in ms)
  rng-seed             : Seed for deterministic process readings
  taskbar-position     : "bottom" (standard Windows) or "top"
  wallpaper-mode       : "classic-teal", "gradient", "bliss", "pattern", "plain"
  ui-scale             : Scaling factor (e.g. 1.25, 1.5 - fixes small icons)
  show-desktop-icons   : "true" to display desktop shortcut icons
  theme                : "classic"


PROJECT STRUCTURE
-----------------
  main.cpp                 Program entry point (int main)
  Compositor.h / .cpp      Compositor engine, window loop, and layer stack
  IAppWindow.h             Polymorphic application interface
  WindowManager.h / .cpp   Application registry, state, and decoupled rendering
  RetroGfx.h / .cpp        Windows Classic 3D bevels, palette, and vector icons
  Desktop.h / .cpp         Teal wallpaper, desktop shortcuts, LCD clock, PWR
  TaskBar.h / .cpp         Start button, Start menu, Quick launch, system tray
  TaskManager.h / .cpp     Classic Task Manager (Applications, Processes, CRT graph)
  AppScreen.h / .cpp       Files (Explorer) and Settings (Control Panel)
  ProcessSimulator.h / .cpp Deterministic simulated process telemetry model
  Config.h / .cpp          Runtime parser and validator for config.txt
  config.txt               User-editable runtime parameters
  build.bat                One-step compilation script (supports g++ and CMake)
  CMakeLists.txt           Modern CMake configuration for IDEs and CLI
  prerequisite.md          Setup instructions and prerequisite notes
  README.md                Full documentation with GitHub markdown formatting
  README.txt               Plain-text submission information and run instructions
  third_party/             Vendored GLFW 3.4 and Dear ImGui v1.91.9b sources


TROUBLESHOOTING
---------------
  1. "csopesy.exe : The term 'csopesy.exe' is not recognized..."
     --> You are using PowerShell. Prefix the command with ".\":
         .\csopesy.exe

  2. "'g++' is not recognized as an internal or external command..."
     --> MinGW bin directory is not in your system PATH. Add your MinGW
         bin folder (e.g. C:\msys64\ucrt64\bin) to PATH and restart terminal.

  3. Icons look small:
     --> Increase "ui-scale" in config.txt to 1.5 or 1.75, or adjust via
         Settings (Control Panel) -> Taskbar -> UI Scale Factor.

  4. Process starts but no window appears:
     --> Rebuild using ".\build.bat", which enforces -static-libgcc -static-libstdc++.


AUTHORS & SUBMISSION DETAILS
----------------------------
Course  : CSOPESY — Operating Systems
Project : Semi-Major Output 2 / MO4 — Desktop-Style OS Mock-up

Group Members:
  1. [Name] - [ID Number]
  2. [Name] - [ID Number]
  3. [Name] - [ID Number]
  4. [Name] - [ID Number]
===============================================================================
