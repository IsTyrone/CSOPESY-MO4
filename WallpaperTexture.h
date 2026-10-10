/*
 *  CSOPESY Semi-Major Output 2 / MO4 — Desktop-Style OS Mock-up
 *  WallpaperTexture — Loads an image file and uploads it as an OpenGL texture
 *                     for use as a full-screen desktop wallpaper.
 */

#pragma once
#include "imgui.h"
#include <string>

class WallpaperTexture {
public:
    WallpaperTexture() = default;
    ~WallpaperTexture();

    // Load an image file (JPG/PNG/BMP/TGA) and upload it as a GL texture.
    // Safe to call multiple times; releases the previous texture first.
    // Returns true on success.
    bool loadFromFile(const std::string& path);

    // Release the GL texture (call on shutdown).
    void release();

    // ImGui-usable texture handle (nullptr if not loaded).
    ImTextureID handle() const;

    // Original image dimensions in pixels (0 if not loaded).
    int width()  const { return m_width;  }
    int height() const { return m_height; }

    bool valid() const { return m_texture != 0; }

private:
    unsigned int m_texture = 0;   // GLuint
    int          m_width   = 0;
    int          m_height  = 0;
};