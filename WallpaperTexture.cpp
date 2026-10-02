/*
 *  CSOPESY Semi-Major Output 2 / MO4 — Desktop-Style OS Mock-up
 *  WallpaperTexture — Implementation
 */

#include "WallpaperTexture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <GLFW/glfw3.h>
#include <GL/gl.h>        // for GL_CLAMP_TO_EDGE, glGenTextures, glTexImage2D
#include <cstdio>
#include <algorithm>

#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE 0x812F
#endif

// "Cover" fit math: scale the image so it fills the window, cropping overflow.
// Returns the top-left offset and size (in the source image's coordinate space)
// that the ImGui::AddImage call should sample from.
// NOTE: We do the fit math at draw time in Desktop.cpp; this file only stores
// the raw texture + native dimensions.

WallpaperTexture::~WallpaperTexture() {
    release();
}

bool WallpaperTexture::loadFromFile(const std::string& path) {
    release();  // clear any previous texture

    int w = 0, h = 0, channels = 0;
    stbi_set_flip_vertically_on_load(0); // OpenGL expects bottom-left origin
    unsigned char* pixels = stbi_load(path.c_str(), &w, &h, &channels, 4);
    if (!pixels) {
        std::fprintf(stderr, "[WallpaperTexture] Failed to load image: %s\n", path.c_str());
        return false;
    }

    GLuint tex = 0;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(pixels);

    m_texture = tex;
    m_width   = w;
    m_height  = h;

    std::fprintf(stdout, "[WallpaperTexture] Loaded %s (%dx%d)\n", path.c_str(), w, h);
    return true;
}

void WallpaperTexture::release() {
    if (m_texture != 0) {
        GLuint tex = m_texture;
        glDeleteTextures(1, &tex);
        m_texture = 0;
    }
    m_width  = 0;
    m_height = 0;
}

ImTextureID WallpaperTexture::handle() const {
    // ImTextureID is typically void*; convert from GLuint
    return (m_texture != 0) ? (ImTextureID)(intptr_t)m_texture : (ImTextureID)0;
}