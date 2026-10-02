===============================================================================
CSOPESY Semi-Major Output 2 / MO4 — Desktop-Style OS Mock-up
De La Salle University — Operating Systems
===============================================================================

ENTRY CLASS / FILE
------------------
  Entry File : main.cpp
  Function   : int main()
  Role       : Boots the Compositor (GLFW + OpenGL 3.3 + Dear ImGui) and
               enters the main render loop.


DESCRIPTION
-----------
A compositor-driven, desktop-style operating-system mock-up built with C++17,
Dear ImGui v1.91.9b, GLFW 3.4, and an OpenGL 3.3 Core Profile.

The application emulates a modern desktop environment with:
  • Wallpaper background layer (gradient, pattern, or plain via ImGui commands)
  • Real-time digital clock updating every frame
  • Fixed taskbar (dockable to top or bottom via config)
  • Three application launcher icons:
      1. Files          - Two-pane file browser screen with placeholder data
      2. Settings       - Categorized system configuration mock screen
      3. Task Manager   - Interactive process table showing live CPU/RAM metrics
  • Active application indicators (green tint, status pip, and taskbar chip)
  • Toggle window controls (click launcher icon to toggle open/minimized)
  • Clean shutdown PWR button on the desktop (only exit path, exit code 0)
  • Dynamic runtime configuration via config.txt without recompilation


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

2. Run the build script:
     In PowerShell:
         .\build.bat

     In Command Prompt (CMD):
         build.bat

3. The script compiles all sources with:
     -std=c++17 -O2 -Wall -static-libgcc -static-libstdc++

   The static flags ensure csopesy.exe is completely standalone and will
   not fail due to missing MinGW DLLs (e.g., libstdc++-6-x64.dll).

To clean build artefacts:
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
  • Launch an app     : Click its icon on the taskbar.
  • Close/toggle app  : Click the active taskbar icon again, or click [X] on
                        the window title bar.
  • Reposition window : Click and drag the window title bar.
  • Shutdown Mock OS  : Click the red PWR button on the desktop.
                        This is the designated shutdown method.


CONFIGURATION (config.txt)
--------------------------
Edit config.txt in any text editor to modify parameters without recompiling:

  framebuffer-width    : Window width in pixels (min 800)
  framebuffer-height   : Window height in pixels (min 600)
  process-count        : Number of simulated processes (default 6)
  min-cpu / max-cpu    : CPU percentage range per process
  min-mem-kb           : Minimum simulated RAM in KB per process
  max-mem-kb           : Maximum simulated RAM in KB per process
  total-mem-kb         : Total system simulated memory in KB
  update-interval-ms   : Refresh cadence for simulated readings (in ms)
  rng-seed             : Seed for deterministic process readings
  taskbar-position     : "top" or "bottom"
  wallpaper-mode       : "gradient", "pattern", or "plain"


PROJECT STRUCTURE
-----------------
  main.cpp                 Program entry point (int main)
  Compositor.h / .cpp      Compositor, window loop, and layer management
  Desktop.h / .cpp         Wallpaper rendering, live clock, PWR shutdown button
  TaskBar.h / .cpp         Taskbar strip, launcher buttons, active app chips
  TaskManager.h / .cpp     Live process table and CPU/RAM summary display
  AppScreen.h / .cpp       Files explorer and Settings placeholder screens
  ProcessSimulator.h / .cpp Deterministic simulated process telemetry model
  Config.h / .cpp          Runtime parser for config.txt
  config.txt               Runtime parameters loaded at startup
  build.bat                One-step build and static verification script
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

  3. Process starts but no window appears:
     --> The binary was linked dynamically against missing MinGW DLLs.
         Rebuild using ".\build.bat", which enforces static runtime linking.

  4. Crash or black screen on launch:
     --> Verify that your graphics card driver supports OpenGL 3.3 Core Profile.


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
