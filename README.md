# CSOPESY — Desktop-Style OS Mock-up (MO4 / SMO2)

[![C++17](https://img.shields.io/badge/Language-C%2B%2B17-blue.svg)](https://en.cppreference.com/w/cpp/17)
[![Dear ImGui](https://img.shields.io/badge/GUI-Dear%20ImGui%20v1.91.9b-orange.svg)](https://github.com/ocornut/imgui)
[![GLFW](https://img.shields.io/badge/Library-GLFW%203.4-green.svg)](https://www.glfw.org/)
[![OpenGL](https://img.shields.io/badge/Graphics-OpenGL%203.3%20Core-red.svg)](https://www.khronos.org/opengl/)
[![Platform](https://img.shields.io/badge/Platform-Windows%2010%20%2F%2011-lightgrey.svg)](https://www.microsoft.com/windows)

A compositor-driven, desktop-style operating system mockup built in C++17 with Dear ImGui, GLFW 3.4, and OpenGL 3.3 core profile.

Developed for **CSOPESY (Operating Systems)** — Semi-Major Output 2 / MO4 at De La Salle University.

---

## Table of Contents

- [Entry Point](#entry-point)
- [Key Features](#key-features)
- [System Requirements](#system-requirements)
- [Prerequisites & Toolchain Setup](#prerequisites--toolchain-setup)
- [How to Build and Run Properly](#how-to-build-and-run-properly)
  - [1. Building the Project](#1-building-the-project)
  - [2. Running the Application](#2-running-the-application)
  - [3. Cleaning Build Artifacts](#3-cleaning-build-artifacts)
- [User Controls & Navigation](#user-controls--navigation)
- [Configuration (`config.txt`)](#configuration-configtxt)
- [Project Architecture](#project-architecture)
- [Troubleshooting](#troubleshooting)
- [Authors & Submission Details](#authors--submission-details)

---

## Entry Point

As required by the project submission specifications:
- **Entry File**: [`main.cpp`](file:///C:/Users/Gaibril%20Kyle/Documents/GitHub/CSOPESY-MO4/main.cpp)
- **Function**: `int main()`
- **Role**: Initializes the [`Compositor`](file:///C:/Users/Gaibril%20Kyle/Documents/GitHub/CSOPESY-MO4/Compositor.h) class, starts the primary render loop, and handles clean teardown upon exit.

---

## Key Features

1. **Layer-Based Compositor**:
   - Renders each frame back-to-front: Wallpaper & Desktop &rarr; Open Application Windows &rarr; Taskbar &rarr; PWR Button.
2. **Desktop Background**:
   - Full-window background supporting gradient, patterned, or plain modes without relying on external image files.
   - **Real-Time Clock**: Live clock updating continuously every frame in the desktop workspace.
   - **PWR Button**: Prominent shutdown button providing the sole designated exit path (cleanly exits with code `0`).
3. **Taskbar & Launcher**:
   - Fixed-position panel (configurable to top or bottom).
   - Vector icon launchers for three applications:
     - **Files**: Explorer-style file browser screen with breadcrumbs and file metadata.
     - **Settings**: System settings panel with grouped controls (display, audio, performance).
     - **Task Manager**: Live process table modeling CPU and memory utilization.
   - **Active Process Indicators**: Open apps highlight in green, display a status pip, and add an active chip on the taskbar.
   - **Toggle Navigation**: Clicking an open app's launcher icon or its window close button minimizes/closes it.
4. **Live Process Simulator**:
   - Generates simulated process states using a seeded pseudo-random number generator for 100% reproducible test runs.
5. **Zero-Recompile Runtime Configuration**:
   - Window dimensions, process counts, memory limits, update cadence, wallpaper styles, and taskbar placement are loaded dynamically from [`config.txt`](file:///C:/Users/Gaibril%20Kyle/Documents/GitHub/CSOPESY-MO4/config.txt).

---

## System Requirements

| Component | Minimum Specification |
|---|---|
| **Operating System** | Windows 10 or 11 (64-bit) |
| **Graphics Driver** | OpenGL 3.3 Core Profile capable GPU |
| **Resolution** | Minimum 800 &times; 600 (Default: 1280 &times; 800) |
| **Compiler** | MinGW-w64 `g++` (GCC 8.0+ with C++17 support) |
| **Dependencies** | GLFW 3.4 and Dear ImGui v1.91.9b (**vendored under `third_party/`**, no external download required) |

---

## Prerequisites & Toolchain Setup

The project uses vendored libraries under [`third_party/`](file:///C:/Users/Gaibril%20Kyle/Documents/GitHub/CSOPESY-MO4/third_party/), so you only need a C++17 compatible **MinGW-w64 `g++`** compiler installed and available in your system `PATH`.

### Verify your Compiler

Open PowerShell or Command Prompt and run:

```powershell
g++ --version
```

If version `8.0` or higher is displayed, you are ready to build.

### If `g++` is Not Installed

Choose one of the following methods to install MinGW-w64:

- **Via winget (Easiest)**:
  ```powershell
  winget install BrechtSanders.WinLibs.POSIX.UCRT
  ```
- **Via MSYS2**:
  1. Download and install MSYS2 from [msys2.org](https://www.msys2.org/).
  2. Open MSYS2 UCRT64 terminal and install GCC:
     ```bash
     pacman -S mingw-w64-ucrt-x86_64-gcc
     ```
  3. Add `C:\msys64\ucrt64\bin` to your Windows Environment `PATH`.
- **Via WinLibs**:
  1. Download the standalone archive from [winlibs.com](https://winlibs.com/).
  2. Extract to `C:\mingw64` and add `C:\mingw64\bin` to your `PATH`.

---

## How to Build and Run Properly

> [!IMPORTANT]
> **PowerShell vs Command Prompt**: In Windows PowerShell, executables in the current directory must be prefixed with `.\` (e.g. `.\csopesy.exe`). Simply typing `csopesy.exe` will result in a *"term not recognized"* error.

### 1. Building the Project

Open your terminal in the project root directory and execute the build script:

**In PowerShell:**
```powershell
.\build.bat
```

**In Command Prompt (CMD):**
```cmd
build.bat
```

#### What `build.bat` does:
1. Validates that `g++` and required library files (`libglfw3.a`, ImGui sources) are present.
2. Compiles application and ImGui sources with `-std=c++17 -O2 -Wall`.
3. **Statically links MinGW runtimes** (`-static-libgcc -static-libstdc++`) so the resulting binary is completely portable and requires no extra DLLs.
4. Executes an automated `objdump` verification check to ensure no dynamic runtime DLL dependencies remain.

---

### 2. Running the Application

Once built successfully, launch the executable:

**In PowerShell:**
```powershell
.\csopesy.exe
```

**In Command Prompt (CMD):**
```cmd
csopesy.exe
```

A window titled **"CSOPESY SMO2 — Desktop-Style OS Mock-up"** will appear at the dimensions configured in `config.txt`.

---

### 3. Cleaning Build Artifacts

To remove compiled executables and temporary files:

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

| Action | Control / Interaction |
|---|---|
| **Launch Application** | Click an icon on the taskbar (**Files**, **Settings**, or **Task Manager**). |
| **Close Application Window** | Click the active icon in the taskbar again, or click the **[X]** in the window's title bar. |
| **Move / Resize Windows** | Drag windows by their title bars; resize by dragging bottom-right corners. |
| **Shut Down Mock OS** | Click the red **PWR** button on the desktop. This is the **designated clean exit path** (exits with code `0`). |

---

## Configuration (`config.txt`)

You can modify runtime parameters without recompiling the program. Simply edit [`config.txt`](file:///C:/Users/Gaibril%20Kyle/Documents/GitHub/CSOPESY-MO4/config.txt) in any text editor and restart the application:

```ini
# Host window size (minimum 800 x 600)
framebuffer-width  1280
framebuffer-height 800

# Process model feeding the Task Manager table
process-count      6
min-cpu            0.5
max-cpu            42
min-mem-kb         2048
max-mem-kb         65536
total-mem-kb       262144
update-interval-ms 180

# Seed for reproducible process readings
rng-seed           20260918

# Shell layout: top | bottom
taskbar-position   top

# Wallpaper mode: gradient | pattern | plain
wallpaper-mode     gradient
```

| Setting | Type | Description |
|---|---|---|
| `framebuffer-width` | Integer | Initial window width in pixels (minimum `800`). |
| `framebuffer-height` | Integer | Initial window height in pixels (minimum `600`). |
| `process-count` | Integer | Number of simulated processes in the Task Manager table. |
| `min-cpu` / `max-cpu` | Float | Simulated CPU usage range per process. |
| `min-mem-kb` / `max-mem-kb` | Integer | Simulated memory range per process in KB. |
| `total-mem-kb` | Integer | Simulated total system RAM in KB. |
| `update-interval-ms` | Integer | Refresh interval for simulated CPU/RAM telemetry (in ms). |
| `rng-seed` | Integer | PRNG seed ensuring reproducible black-box test runs. |
| `taskbar-position` | String | `top` or `bottom`. Sets the docking position of the taskbar. |
| `wallpaper-mode` | String | `gradient` (default blue gradient), `pattern` (grid pattern), or `plain` (solid dark). |

---

## Project Architecture

```
CSOPESY-MO4/
├── main.cpp                  # Program entry point (int main)
├── Compositor.h / .cpp       # Compositor engine, window lifecycle, layer rendering stack
├── Desktop.h / .cpp          # Background wallpaper, real-time clock, PWR button
├── TaskBar.h / .cpp          # Taskbar strip, launcher icons, active-app chips
├── TaskManager.h / .cpp      # Windows-style Task Manager table and system telemetry
├── AppScreen.h / .cpp        # Files explorer and Settings mock screens
├── ProcessSimulator.h / .cpp # Deterministic simulated process generator (CPU & RAM)
├── Config.h / .cpp           # Runtime parser for config.txt
├── config.txt                # User-editable runtime parameters
├── build.bat                 # One-step compilation and verification script
├── prerequisite.md           # Detailed prerequisites documentation
├── specs-mo4.docx            # Project specifications document
└── third_party/
    ├── glfw/                 # GLFW 3.4 headers and prebuilt static library (libglfw3.a)
    └── imgui/                # Dear ImGui v1.91.9b source code and backends
```

---

## Troubleshooting

| Problem | Cause | Solution |
|---|---|---|
| `'csopesy.exe' is not recognized...` | Executing directly in PowerShell without path prefix. | Run using `.\csopesy.exe` instead of `csopesy.exe`. |
| `'g++' is not recognized` | MinGW compiler is not in your system `PATH`. | Install MinGW-w64 and add its `bin` folder to your system `PATH`, then restart your terminal. |
| `cannot find -lglfw3` | Vendored static library is missing. | Verify that [`third_party/glfw/lib/libglfw3.a`](file:///C:/Users/Gaibril%20Kyle/Documents/GitHub/CSOPESY-MO4/third_party/glfw/lib/libglfw3.a) is present. Run `git checkout -- third_party/glfw/lib/libglfw3.a` if needed. |
| Process launches but no window appears | Binary was compiled without static runtime linking. | Recompile using `.\build.bat`. The script ensures `-static-libgcc -static-libstdc++` flags are applied. |
| Immediate crash / Black screen | GPU driver does not support OpenGL 3.3 Core Profile. | Update your graphics drivers or ensure hardware acceleration is enabled. |

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
