@echo off
REM ============================================================
REM  CSOPESY Semi-Major Output 2 / MO4 — build script
REM  Desktop-Style OS Mock-up (Windows Classic Display Edition)
REM
REM  Requires:  MinGW g++ (C++17 capable)
REM             GLFW 3.4 + Dear ImGui v1.91.9b (vendored under third_party/)
REM
REM  Usage:     build.bat           (Direct fast build via g++)
REM             build.bat cmake     (Build via CMake, if CMake is installed)
REM             build.bat clean     (Clean all artefacts and build folder)
REM ============================================================

setlocal

REM ── Handle "clean" ─────────────────────────────────────────────
if /I "%1"=="clean" (
    echo Cleaning build artefacts...
    if exist csopesy.exe        del csopesy.exe
    if exist csopesy_static.exe del csopesy_static.exe
    if exist imgui.ini          del imgui.ini
    if exist build              rd /s /q build
    echo Clean complete.
    goto :eof
)

REM ── Handle "cmake" build option ────────────────────────────────
if /I "%1"=="cmake" (
    where cmake >nul 2>&1
    if errorlevel 1 (
        echo ERROR: 'cmake' was not found on PATH.
        echo        Please install CMake or run 'build.bat' to build directly with g++.
        exit /b 1
    )
    echo Building with CMake...
    cmake -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
    if errorlevel 1 exit /b 1
    cmake --build build --config Release
    if errorlevel 1 exit /b 1
    if exist build\csopesy.exe (
        copy /y build\csopesy.exe .\csopesy.exe >nul
    )
    echo CMake build succeeded: csopesy.exe
    goto :eof
)

REM ── Direct Toolchain ───────────────────────────────────────────
set CXX=g++
set CXXFLAGS=-std=c++17 -O2 -Wall -static-libgcc -static-libstdc++
set OUTPUT=csopesy.exe
set OBJdump=objdump

REM ── Paths ──────────────────────────────────────────────────────
set IMGUI=third_party\imgui
set GLFW=third_party\glfw

set INCLUDES=-I. -I%IMGUI% -I%IMGUI%\backends -I%GLFW%\include
set LIBDIRS=-L%GLFW%\lib
set LIBS=-lglfw3 -lopengl32 -lgdi32 -luser32 -lshell32

REM ── Preflight Checks ───────────────────────────────────────────
where %CXX% >nul 2>&1
if errorlevel 1 (
    echo ERROR: '%CXX%' not found on PATH.
    echo        Install MinGW-w64 ^(GCC 8+ with C++17^) and add its bin folder
    echo        to PATH.  See prerequisite.md for MSYS2 / WinLibs instructions.
    exit /b 1
)

if not exist "%GLFW%\lib\libglfw3.a" (
    echo ERROR: third_party\glfw\lib\libglfw3.a is missing.
    echo        Run:  git checkout -- third_party/glfw/lib/libglfw3.a
    exit /b 1
)

if not exist "%IMGUI%\imgui.cpp" (
    echo ERROR: vendored Dear ImGui sources not found under third_party\imgui.
    echo        Re-extract the project archive.
    exit /b 1
)

REM ── Source files ───────────────────────────────────────────────
set APP_SRC=main.cpp Compositor.cpp Desktop.cpp TaskBar.cpp TaskManager.cpp AppScreen.cpp ProcessSimulator.cpp Config.cpp RetroGfx.cpp WindowManager.cpp

set IMGUI_SRC=%IMGUI%\imgui.cpp %IMGUI%\imgui_draw.cpp %IMGUI%\imgui_tables.cpp %IMGUI%\imgui_widgets.cpp %IMGUI%\backends\imgui_impl_glfw.cpp %IMGUI%\backends\imgui_impl_opengl3.cpp

REM ── Build ──────────────────────────────────────────────────────
echo Building %OUTPUT% (C++17 + Windows Classic Engine) ...
%CXX% %CXXFLAGS% %INCLUDES% %APP_SRC% %IMGUI_SRC% %LIBDIRS% %LIBS% -o %OUTPUT%

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo BUILD FAILED — see errors above.
    exit /b 1
)

echo.
echo Build succeeded: %OUTPUT%
echo Run with:  .\%OUTPUT% (PowerShell) or %OUTPUT% (CMD)
echo.

REM ── Post-build verification ────────────────────────────────────
setlocal EnableDelayedExpansion
where %OBJdump% >nul 2>&1
if errorlevel 1 goto :skipverify

set MISSING=
for %%D in (libstdc++-6-x64.dll libgcc_s_seh-1.dll) do (
    %OBJdump% -p %OUTPUT% 2>nul | findstr /C:"DLL Name: %%D" >nul && set MISSING=%%D
)
if defined MISSING goto :verifyfail

echo VERIFY OK: no MinGW runtime DLL dependencies - the exe is self-contained.
goto :verifydone

:verifyfail
echo VERIFY FAILED: %OUTPUT% depends on !MISSING!, which may be absent on
echo                another machine.
echo                Ensure CXXFLAGS contains -static-libgcc -static-libstdc++.
exit /b 1

:skipverify
echo VERIFY SKIPPED: objdump unavailable, cannot inspect DLL dependencies.

:verifydone
endlocal
echo.
echo Next:  .\%OUTPUT%
