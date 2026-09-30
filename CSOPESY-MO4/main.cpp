/*
 *  CSOPESY Semi-Major Output 2  —  Desktop-Style OS Mock-up
 *  Entry point
 *
 *  Boots the compositor (GLFW + OpenGL + Dear ImGui) and hands control to it.
 *  The program owns no frame logic of its own: Compositor::run() drives the
 *  poll / new-frame / paint-layers / submit / swap sequence, and only the PWR
 *  button on the desktop layer can bring the loop down.
 *
 *  Build (MinGW g++) — or just run build.bat:
 *      build.bat
 */

#include <cstdio>
#include "Compositor.h"

int main() {
    Compositor compositor;

    if (!compositor.init()) {
        std::fprintf(stderr,
                     "CSOPESY: startup failed.  A machine with an OpenGL 3.3 capable\n"
                     "         display driver is required.\n");
        return 1;
    }

    compositor.run();
    compositor.shutdown();
    return 0;
}
