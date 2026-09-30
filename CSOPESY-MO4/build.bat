@echo off
REM ============================================================
REM  CSOPESY Semi-Major Output 2 — build script
REM  Desktop-Style OS Mock-up
REM
REM  Requires:  MinGW g++ (C++17 capable)
REM             GLFW 3.4 + Dear ImGui v1.91.9b (vendored under third_party/)
REM
REM  Usage:     build.bat
REM             build.bat clean
REM ============================================================

setlocal

REM ── Toolchain ──────────────────────────────────────────────────
set CXX=g++
REM  -static-libgcc -static-libstdc++ are REQUIRED, not optional.  Without them
REM  the binary links libstdc++-6-x64.dll / libgcc_s_seh-1.dll dynamically and
REM  silently fails to start (no window) on any machine where the MinGW runtime
REM  DLLs are not on PATH.  Linking statically makes csopesy.exe self-contained.
set CXXFLAGS=-std=c++17 -O2 -Wall -static-libgcc -static-libstdc++
set OUTPUT=csopesy.exe

REM  Used only for the post-build dependency check at the end of this script.
set OBJdump=objdump

REM ── Paths ──────────────────────────────────────────────────────
set IMGUI=third_party\imgui
set GLFW=third_party\glfw

set INCLUDES=-I%IMGUI% -I%IMGUI%\backends -I%GLFW%\include
set LIBDIRS=-L%GLFW%\lib
set LIBS=-lglfw3 -lopengl32 -lgdi32 -luser32 -lshell32

REM ── Preflight: fail loudly, never with a silent bad artefact ────
where %CXX% >nul 2>&1
if errorlevel 1 (
    echo ERROR: '%CXX%' not found on PATH.
    echo        Install MinGW-w64 ^(GCC 8+ with C++17^) and add its bin folder
    echo        to PATH.  See prerequisite.md for MSYS2 / WinLibs instructions.
    exit /b 1
)

if not exist "%GLFW%\lib\libglfw3.a" (
    echo ERROR: third_party\glfw\lib\libglfw3.a is missing.
    echo        It is committed to the repository but was not checked out.
    echo        Run:  git checkout -- third_party/glfw/lib/libglfw3.a
    exit /b 1
)

if not exist "%IMGUI%\imgui.cpp" (
    echo ERROR: vendored Dear ImGui sources not found under third_party\imgui.
    echo        Re-extract the project archive.
    exit /b 1
)

REM ── Source files ───────────────────────────────────────────────
set APP_SRC=main.cpp Compositor.cpp Desktop.cpp TaskBar.cpp TaskManager.cpp AppScreen.cpp ProcessSimulator.cpp Config.cpp

set IMGUI_SRC=%IMGUI%\imgui.cpp %IMGUI%\imgui_draw.cpp %IMGUI%\imgui_tables.cpp %IMGUI%\imgui_widgets.cpp %IMGUI%\backends\imgui_impl_glfw.cpp %IMGUI%\backends\imgui_impl_opengl3.cpp

REM ── Handle "clean" ─────────────────────────────────────────────
if /I "%1"=="clean" (
    echo Cleaning...
    if exist %OUTPUT%        del %OUTPUT%
    if exist csopesy_static.exe del csopesy_static.exe
    if exist imgui.ini       del imgui.ini
    echo Done.
    goto :eof
)

REM ── Build ──────────────────────────────────────────────────────
echo Building %OUTPUT% ...
%CXX% %CXXFLAGS% %INCLUDES% %APP_SRC% %IMGUI_SRC% %LIBDIRS% %LIBS% -o %OUTPUT%

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo BUILD FAILED — see errors above.
    exit /b 1
)

echo.
echo Build succeeded: %OUTPUT%
echo Run with:  %OUTPUT%
echo.
echo ── Post-build verification ────────────────────────────────────
REM  Fail here rather than shipping a binary that cannot start.  A dynamic
REM  MinGW runtime link produces an exe that launches but never opens a window.
REM  Delayed expansion is required: MISSING is set inside a block and read
REM  after it, which %ERRORLEVEL%-style expansion would resolve too early.
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
echo                another machine.  It would start but never open a window.
echo                Ensure CXXFLAGS contains -static-libgcc -static-libstdc++.
exit /b 1

:skipverify
echo VERIFY SKIPPED: objdump unavailable, cannot inspect DLL dependencies.

:verifydone
endlocal
echo.
echo Next:  %OUTPUT%
