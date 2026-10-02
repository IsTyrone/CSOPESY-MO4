cmake_minimum_required(VERSION 3.16)
project(CSOPESY_DesktopOS
    VERSION 1.0.0
    DESCRIPTION "CSOPESY Semi-Major Output 2 / MO4: Desktop-Style OS Mock-up"
    LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Optimization and Warning flags
if(MSVC)
    add_compile_options(/W4 /O2)
else()
    add_compile_options(-Wall -Wextra -O2)
    # Statically link runtime for standalone portability
    add_link_options(-static-libgcc -static-libstdc++)
endif()

# Include directories
set(IMGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui")
set(GLFW_DIR  "${CMAKE_CURRENT_SOURCE_DIR}/third_party/glfw")

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/stb
)

# Application Sources
set(APP_SOURCES
    main.cpp
    Compositor.cpp
    Desktop.cpp
    TaskBar.cpp
    TaskManager.cpp
    AppScreen.cpp
    ProcessSimulator.cpp
    Config.cpp
    RetroGfx.cpp
    WindowManager.cpp
    WallpaperTexture.cpp
    BootScreen.cpp
)

# Dear ImGui Vendored Sources
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

# Executable target
add_executable(csopesy
    ${APP_SOURCES}
    ${IMGUI_SOURCES}
)

# Library linking
if(WIN32)
    find_library(GLFW_LIB NAMES glfw3 libglfw3 HINTS "${GLFW_DIR}/lib")
    if(NOT GLFW_LIB)
        set(GLFW_LIB "${GLFW_DIR}/lib/libglfw3.a")
    endif()

    target_link_libraries(csopesy PRIVATE
        ${GLFW_LIB}
        opengl32
        gdi32
        user32
        shell32
    )
endif()

# Copy runtime config next to binary if building out-of-source
if(NOT CMAKE_CURRENT_BINARY_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    add_custom_command(TARGET csopesy POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/config.txt"
            "${CMAKE_CURRENT_BINARY_DIR}/config.txt"
        COMMENT "Copying config.txt to binary output directory"
    )
endif()
