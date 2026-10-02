# CSOPESY — Desktop-Style OS Mock-up (MO4 / SMO2)

[![C++17](https://img.shields.io/badge/Language-C%2B%2B17-blue.svg)](https://en.cppreference.com/w/cpp/17)
[![Dear ImGui](https://img.shields.io/badge/GUI-Dear%20ImGui%20v1.91.9b-orange.svg)](https://github.com/ocornut/imgui)
[![GLFW](https://img.shields.io/badge/Library-GLFW%203.4-green.svg)](https://www.glfw.org/)
[![OpenGL](https://img.shields.io/badge/Graphics-OpenGL%203.3%20Core-red.svg)](https://www.khronos.org/opengl/)
[![CMake](https://img.shields.io/badge/Build-CMake%203.16%2B-brightgreen.svg)](https://cmake.org/)
[![Platform](https://img.shields.io/badge/Platform-Windows%2010%20%2F%2011-lightgrey.svg)](https://www.microsoft.com/windows)

A compositor-driven, **Windows Classic (95 / 98 / 2000)** desktop operating system mockup built in modern C++17 with Dear ImGui, GLFW 3.4, and OpenGL 3.3 Core Profile.

Developed for **CSOPESY (Operating Systems)** — Semi-Major Output 2 / MO4 at De La Salle University.

---

## Table of Contents

- [Entry Point](#entry-point)
- [Visual Highlights & Windows Classic Display](#visual-highlights--windows-classic-display)
- [Polymorphic Architecture](#polymorphic-architecture)
- [System Requirements](#system-requirements)
- [Prerequisites & Toolchain Setup](#prerequisites--toolchain-setup)
- [How to Build and Run Properly](#how-to-build-and-run-properly)
  - [Method A: One-Step Build Script (`build.bat`)](#method-a-one-step-build-script-buildbat)
  - [Method B: CMake Build (`CMakeLists.txt`)](#method-b-cmake-build-cmakeliststxt)
  - [Running the Executable](#running-the-executable)
  - [Cleaning Build Artifacts](#cleaning-build-artifacts)
- [User Controls & Navigation](#user-controls--navigation)
- [Configuration Reference (`config.txt`)](#configuration-reference-configtxt)
- [Fixing Small Icons / Scaling UI](#fixing-small-icons--scaling-ui)
- [Project Architecture & File Map](#project-architecture--file-map)
- [Troubleshooting](#troubleshooting)
- [Authors & Submission Details](#authors--submission-details)

---

## Entry Point

As required by the project specifications:
- **Entry File**: [`main.cpp`](file:///C:/Users/Gaibril%20Kyle/Documents/GitHub/CSOPESY-MO4/main.cpp)
- **Function**: `int main()`
- **Role**: Boots the [`Compositor`](file:///C:/Users/Gaibril%20Kyle/Documents/GitHub/CSOPESY-MO4/Compositor.h) subsystem, initializes GLFW/OpenGL, registers polymorphic applications into the [`WindowManager`](file:///C:/Users/Gaibril%20Kyle/Documents/GitHub/CSOPESY-MO4/WindowManager.h), executes the frame compositor loop, and performs clean shutdown upon exit.

---

## Visual Highlights & Windows Classic Display

The system faithfully recreates the authentic **Windows 95 / 98 / 2000** visual aesthetic:

1. **Classic Desktop Workspace**:
   - Authentic `#008080` teal desktop background (configurable to Windows 2000 setup gradient, Windows XP Bliss, grid pattern, or solid slate).
   - **Large Desktop Shortcut Icons**: Manila folder for **Files**, beige CRT monitor with wrench for **Settings**, and oscilloscope monitor for **Task Manager**. Clicking any icon opens the application.
   - **Desktop Digital Clock**: Retro beveled LCD/LED clock widget with day, date, and live time updating every frame.
   - **Prominent PWR Button**: 3D beveled red power button on the desktop providing the sole designated clean exit mechanism (`exit(0)`).
2. **Authentic Classic Taskbar**:
   - Raised 3D white highlight shelf docked at the bottom (or top via configuration).
   - **Classic "Start" Button**: Features the iconic Windows 4-color flag (Red, Green, Blue, Yellow) and bold "Start" label.
   - **Authentic Start Menu**: Toggling the Start button reveals the classic popup menu with a vertical blue gradient banner, app shortcuts, separator, and "Shut Down..." option.
   - **Quick Launch Bar**: Convenient launcher icons for rapid access.
   - **Active Window Tabs**: Windows 95/98 style taskbar buttons. Focused windows have sunken 3D bevels; inactive windows have raised bevels.
   - **System Tray (Notification Area)**: Recessed 3D tray with speaker volume icon, network connection monitors, and live digital clock.
3. **Three Full-Featured Applications**:
   - **Files (Explorer)**: Two-pane Windows Explorer with menu bar, Address input bar, hierarchical folder tree, detailed file table, and status bar.
   - **Settings (Control Panel)**: Property sheet tabbed dialog with Display settings, Taskbar placement, System details, and live `config.txt` parameter dump.
   - **Task Manager**: Windows Classic Task Manager with tabbed navigation:
     - `Applications`: Running processes with "Running" status.
     - `Processes`: Interactive table with Image Name, PID, CPU %, Memory, and Status.
     - `Performance`: **Real-time green CRT oscilloscope CPU graph** with rolling waveform, memory progress bar, and system totals.

---

## Polymorphic Architecture

The codebase utilizes an object-oriented, decoupled architecture adhering to software engineering best practices:

- [`IAppWindow`](file:///C:/Users/Gaibril%20Kyle/Documents/GitHub/CSOPESY-MO4/IAppWindow.h): Abstract interface defining `id()`, `title()`, `render()`, and `drawIcon()`.
- [`WindowManager`](file:///C:/Users/Gaibril%20Kyle/Documents/GitHub/CSOPESY-MO4/WindowManager.h): Manages registered application instances polymorphically. Adding new programs requires zero edits to compositor branching.
- [`RetroGfx`](file:///C:/Users/Gaibril%20Kyle/Documents/GitHub/CSOPESY-MO4/RetroGfx.h): Centralized graphics engine rendering 2-pixel 3D bevels, sunken borders, gradient title bars, and scalable vector icons without external image dependencies.
- [`Compositor`](file:///C:/Users/Gaibril%20Kyle/Documents/GitHub/CSOPESY-MO4/Compositor.h): Orchestrates the layered render pipeline:
  $$\text{Wallpaper / Desktop} \longrightarrow \text{App Windows} \longrightarrow \text{Taskbar} \longrightarrow \text{PWR / Dialogs}$$

---

## System Requirements

| Component | Minimum Specification |
|---|---|
| **Operating System** | Windows 10 or 11 (64-bit) |
| **Graphics Driver** | OpenGL 3.3 Core Profile capable GPU |
| **Resolution** | Minimum 800 &times; 600 (Default: 1280 &times; 800) |
| **Compiler** | MinGW-w64 `g++` (GCC 8.0+ with C++17 support) |
| **Build Tools** | Direct Batch Script (`build.bat`) or CMake 3.16+ |
| **Dependencies** | GLFW 3.4 & Dear ImGui v1.91.9b (**vendored under `third_party/`**, no download required) |

---

## Prerequisites & Toolchain Setup

The project includes pre-compiled static libraries and headers in [`third_party/`](file:///C:/Users/Gaibril%20Kyle/Documents/GitHub/CSOPESY-MO4/third_party/). You only need a C++17 capable **MinGW-w64 `g++`** compiler on your system `PATH`.

### Verify your Compiler

Open PowerShell or Command Prompt and test:

```powershell
g++ --version
```

If version `8.0` or higher is displayed, you are ready to compile.

### If `g++` is Not Installed

- **Via WinGet (Recommended)**:
  ```powershell
  winget install BrechtSanders.WinLibs.POSIX.UCRT
  ```
- **Via MSYS2**:
  ```bash
  pacman -S mingw-w64-ucrt-x86_64-gcc
  ```
  Then add `C:\msys64\ucrt64\bin` to your system `PATH`.

---

## How to Build and Run Properly

> [!IMPORTANT]
> **PowerShell Execution Rule**: In Windows PowerShell, programs in the current directory **must** be executed with `.\` (e.g. `.\csopesy.exe`). Running `csopesy.exe` without `.\` will fail with:
> `The term 'csopesy.exe' is not recognized as the name of a cmdlet...`

### Method A: One-Step Build Script (`build.bat`)

The included build script compiles all sources directly with GCC and verifies static runtime linking.

**In PowerShell:**
```powershell
.\build.bat
```

**In Command Prompt (CMD):**
```cmd
build.bat
```

#### What `build.bat` ensures:
1. Validates that `g++` and vendored libraries are present.
2. Compiles application and ImGui sources with `-std=c++17 -O2 -Wall`.
3. **Statically links MinGW runtimes** (`-static-libgcc -static-libstdc++`) to prevent missing DLL crashes (`libstdc++-6-x64.dll`).
4. Runs an automated `objdump` check to verify that the executable is 100% self-contained.

---

### Method B: CMake Build (`CMakeLists.txt`)

If you have CMake installed or use IDEs like CLion or VS Code:

**In PowerShell:**
```powershell
cmake -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

Or simply run:
```powershell
.\build.bat cmake
```

---

### Running the Executable

Once compiled, launch the program:

**In PowerShell:**
```powershell
.\csopesy.exe
```

**In Command Prompt (CMD):**
```cmd
csopesy.exe
```

---

### Cleaning Build Artifacts

To remove compiled binaries, temporary files, and CMake caches:

**In PowerShell:**
```powershell
.\build.bat clean
```

**In Command Prompt (CMD):**
```cmd
build.bat clean
```

---

## User Controls & Navigation

| Control | Action |
|---|---|
| **Start Button** | Click **Start** on the taskbar to open/close the Windows Start Menu. |
| **Desktop Icons** | Click any shortcut icon on the desktop (**Files**, **Settings**, **Task Manager**) to toggle its window. |
| **Taskbar Quick Launch** | Click launcher icons to toggle windows open or minimized. |
| **Window Tabs** | Click the active window tabs on the taskbar to focus or toggle windows. |
| **Shut Down (PWR)** | Click the **Shut Down** button on the desktop or via the Start Menu. This is the **designated clean exit path** (`exit(0)`). |

---

## Configuration Reference (`config.txt`)

Parameters are loaded on startup without requiring recompilation. Open [`config.txt`](file:///C:/Users/Gaibril%20Kyle/Documents/GitHub/CSOPESY-MO4/config.txt) in any text editor:

```ini
# Host window dimensions (min 800 x 600)
framebuffer-width  1280
framebuffer-height 800

# Simulated Process Model (Task Manager)
process-count      6
min-cpu            0.5
max-cpu            42
min-mem-kb         2048
max-mem-kb         65536
total-mem-kb       262144
update-interval-ms 180

# Deterministic Seed for Reproducible Tests
rng-seed           20260918

# Shell Layout: bottom (standard Windows) | top
taskbar-position   bottom

# Wallpaper: classic-teal | gradient | bliss | pattern | plain
wallpaper-mode     classic-teal

# UI and Icon Scaling Factor (Enlarges icons on high-res monitors)
ui-scale           1.25

# Desktop Shortcuts
show-desktop-icons true

# Theme: classic | modern
theme              classic
```

---

## Fixing Small Icons / Scaling UI

If icons appear small on your display (common on 1080p, 1440p, or 4K monitors):

1. Open [`config.txt`](file:///C:/Users/Gaibril%20Kyle/Documents/GitHub/CSOPESY-MO4/config.txt).
2. Increase `ui-scale` from `1.25` to `1.5` or `1.75`:
   ```ini
   ui-scale 1.5
   ```
3. Save the file and restart `.\csopesy.exe`.
   - Desktop icons, Start button, Quick Launch icons, system tray, and fonts will scale up smoothly.
4. You can also adjust the scale dynamically at runtime from **Settings (Control Panel) &rarr; Taskbar &rarr; UI Scale Factor**.

---

## Project Architecture & File Map

```
CSOPESY-MO4/
├── main.cpp                  # Program entry point (int main)
├── Compositor.h / .cpp       # Compositor engine, window lifecycle, layer rendering stack
├── IAppWindow.h              # Polymorphic interface for all application windows
├── WindowManager.h / .cpp    # App registry, focus management, and decoupled rendering
├── RetroGfx.h / .cpp         # Windows Classic 3D bevels, palette, and scalable vector icons
├── Desktop.h / .cpp          # Teal wallpaper, desktop shortcuts, LCD clock, PWR button
├── TaskBar.h / .cpp          # Start button, Start menu, Quick launch, window tabs, system tray
├── TaskManager.h / .cpp      # Windows Task Manager (Applications, Processes, Performance tabs)
├── AppScreen.h / .cpp        # Files (Explorer) and Settings (Control Panel) applications
├── ProcessSimulator.h / .cpp # Deterministic simulated process generator (CPU & RAM)
├── Config.h / .cpp           # Runtime parser and validator for config.txt
├── config.txt                # User-editable runtime parameters
├── build.bat                 # One-step compilation and verification script (g++ and CMake)
├── CMakeLists.txt            # Modern CMake build configuration
├── prerequisite.md           # Setup instructions and prerequisite notes
├── README.md                 # Full markdown documentation and guide
├── README.txt                # Plain-text submission instructions and run guide
└── third_party/
    ├── glfw/                 # GLFW 3.4 headers and prebuilt static library (libglfw3.a)
    └── imgui/                # Dear ImGui v1.91.9b source code and backends
```

---

## Troubleshooting

| Problem | Cause | Solution |
|---|---|---|
| `'csopesy.exe' is not recognized...` | Executing directly in PowerShell without path prefix. | Run with `.\csopesy.exe` instead of `csopesy.exe`. |
| `'g++' is not recognized` | MinGW compiler is not in your system `PATH`. | Install MinGW-w64 and add its `bin` folder (e.g. `C:\msys64\ucrt64\bin`) to your system `PATH`. |
| `cannot find -lglfw3` | Vendored static library was not checked out. | Verify that [`third_party/glfw/lib/libglfw3.a`](file:///C:/Users/Gaibril%20Kyle/Documents/GitHub/CSOPESY-MO4/third_party/glfw/lib/libglfw3.a) is present. Run `git checkout -- third_party/glfw/lib/libglfw3.a`. |
| Process starts but no window appears | Binary was compiled without static runtime linking. | Recompile using `.\build.bat`. The script enforces `-static-libgcc -static-libstdc++`. |
| Crash / Black screen on startup | GPU driver does not support OpenGL 3.3 Core Profile. | Update your graphics drivers or ensure hardware acceleration is enabled. |
| Icons look too small | High-DPI display resolution. | Set `ui-scale 1.5` in `config.txt` or adjust via **Settings &rarr; Taskbar**. |

---

## Authors & Submission Details

- **Course**: CSOPESY — Operating Systems
- **Output**: Semi-Major Output 2 / MO4 (Desktop-Style OS Mock-up)
- **Institution**: De La Salle University
- **Entry File**: `main.cpp`
- **Group Members**:
  - `[Member 1 Name]` - `[ID Number]`
  - `[Member 2 Name]` - `[ID Number]`
  - `[Member 3 Name]` - `[ID Number]`
  - `[Member 4 Name]` - `[ID Number]`
