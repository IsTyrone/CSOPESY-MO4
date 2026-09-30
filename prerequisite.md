# Prerequisites

Software and hardware required to build and run the CSOPESY SMO2 desktop-style OS mock-up.

---

## Hardware

| Component | Minimum |
|---|---|
| GPU / display driver | OpenGL 3.3 core-profile capable |
| Display resolution | 800 × 600 or higher |
| RAM | 256 MB free (the application itself is lightweight) |

---

## Software

| Dependency | Version | Notes |
|---|---|---|
| **Windows** | 10 or later | Tested on Windows 10/11 |
| **MinGW-w64 (g++)** | GCC 8+ with C++17 support | Must be on `PATH`. Recommended: [MSYS2](https://www.msys2.org/) or [WinLibs](https://winlibs.com/) |
| **GLFW 3.4** | 3.4 | Vendored under `third_party/glfw/` — no separate install needed |
| **Dear ImGui** | v1.91.9b | Vendored under `third_party/imgui/` — no separate install needed |

> **Note:** GLFW and Dear ImGui are already included in the `third_party/` directory.
> You only need to install a C++17-capable MinGW g++ compiler.

---

## Setup Instructions

### 1. Install MinGW-w64 (if not already installed)

**Option A — via MSYS2 (recommended):**
1. Download and install MSYS2 from <https://www.msys2.org/>.
2. Open the MSYS2 UCRT64 terminal and run:
   ```
   pacman -S mingw-w64-ucrt-x86_64-gcc
   ```
3. Add `C:\msys64\ucrt64\bin` (or your install path) to the system `PATH`.

**Option B — via WinLibs:**
1. Download a release from <https://winlibs.com/> (GCC 13+ recommended).
2. Extract to a folder (e.g., `C:\mingw64`).
3. Add the `bin` subfolder to the system `PATH`.

### 2. Verify the compiler

Open a new Command Prompt or PowerShell window and run:

```
g++ --version
```

You should see GCC 8.0 or later with C++17 support.

### 3. Build the project

```
build.bat
```

### 4. Run

```
csopesy.exe
```

> **`build.bat` links the MinGW runtime statically** (`-static-libgcc
> -static-libstdc++`), so `csopesy.exe` runs on any machine with a compatible
> `g++` and needs no extra DLLs.

---

## Verifying your build

The mock-up always opens a `1280x800` window titled
`CSOPESY SMO2 - Desktop-Style OS Mock-up`. If the process starts but **no window
appears**, the binary was linked against the MinGW runtime DLLs dynamically and
those DLLs are missing from `PATH`. Check that `build.bat` contains:

```
set CXXFLAGS=-std=c++17 -O2 -Wall -static-libgcc -static-libstdc++
```

and rebuild.

## Troubleshooting

| Problem | Solution |
|---|---|
| `'g++' is not recognized` | MinGW `bin` folder is not on `PATH`. Add it and reopen the terminal. |
| `cannot find -lglfw3` | The `third_party/glfw/lib/` directory is missing or incomplete (`libglfw3.a` is required). Re-extract the project archive. |
| **Process starts but no window appears** | The exe was linked against `libstdc++-6-x64.dll` dynamically and the MinGW runtime is not on `PATH`. Confirm `build.bat` passes `-static-libgcc -static-libstdc++`, then rebuild. |
| Black window / crash on start | Your GPU driver may not support OpenGL 3.3. Update your display driver. |
| Build warnings about `snprintf` | Safe to ignore — the code explicitly includes `<cstdio>`. |
