/*
Copyright (c) 2018, University of Oxford.
All rights reserved.

University of Oxford means the Chancellor, Masters and Scholars of the
University of Oxford, having an administrative office at Wellington
Square, Oxford OX1 2JD, UK.

This file is part of the Oxford RSE C++ Template project.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.
* Neither the name of the copyright holder nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef BACKENDGL_H_
#define BACKENDGL_H_

#include <glad/glad.h>
// works for you.
#define GLFW_INCLUDE_GLEXT
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw_gl3.h"
#include <array>
#include <iostream>
#include <stdio.h>

#include "nanovg.h"
#define NANOVG_GL3_IMPLEMENTATION
#include "nanovg_gl.h"

namespace trase {

enum Align {
  // Horizontal align
  ALIGN_LEFT = 1 << 0,   // Default, align text horizontally to left.
  ALIGN_CENTER = 1 << 1, // Align text horizontally to center.
  ALIGN_RIGHT = 1 << 2,  // Align text horizontally to right.
  // Vertical align
  ALIGN_TOP = 1 << 3,      // Align text vertically to top.
  ALIGN_MIDDLE = 1 << 4,   // Align text vertically to middle.
  ALIGN_BOTTOM = 1 << 5,   // Align text vertically to bottom.
  ALIGN_BASELINE = 1 << 6, // Default, align text vertically to baseline.
};

struct RGBA {
  RGBA(int r, int g, int b, int a) : m_r(r), m_g(g), m_b(b), m_a(a) {}
  int m_r, m_g, m_b, m_a;
};

class BackendGL {
  GLFWwindow *m_window;
  NVGcontext *m_vg;

  enum Align {
    // Horizontal align
    ALIGN_LEFT = 1 << 0,   // Default, align text horizontally to left.
    ALIGN_CENTER = 1 << 1, // Align text horizontally to center.
    ALIGN_RIGHT = 1 << 2,  // Align text horizontally to right.
    // Vertical align
    ALIGN_TOP = 1 << 3,      // Align text vertically to top.
    ALIGN_MIDDLE = 1 << 4,   // Align text vertically to middle.
    ALIGN_BOTTOM = 1 << 5,   // Align text vertically to bottom.
    ALIGN_BASELINE = 1 << 6, // Default, align text vertically to baseline.
  };

  inline void begin_path() { nvgBeginPath(m_vg); }
  inline void move_to(const float x, const float y) { nvgMoveTo(m_vg, x, y); }
  inline void line_to(const float x, const float y) {
    nvgLineTo(m_vg, x + w, y + h);
  }
  inline void stroke_color(const RGBA &color) {
    nvgStrokeColor(m_vg, nvgRGBA(color.r, color.g, color.b, color.a));
  }
  inline void stroke_width(const float lw){nvgStrokeWidth(m_vg, lw)};
  inline void stroke() { nvgStroke(m_vg); }
  inline void font_size(float size) { vgFontSize(m_vg, size); }
  inline void font_face(const char *face) { nvgFontFace(m_vg, face); }
  inline void text_align(const Align &align) { nvgTextAlign(m_vg, align); }
  inline void fill_color(const int r, const int g, const int b, const int a) {
    nvgFillColor(m_vg, nvgRGBA(r, g, b, a));
  }

  inline void text(const float x, const float y, const char *string,
                   const char *end) {
    nvgText(m_vg, x, y, string, end);
  }

  void init(int x_pixels, int y_pixels, const char *name) {
    m_window = create_window(x_pixels, y_pixels, name);
    if (!window)
      throw XX;
    init_imgui(m_window);
    m_vg = init_nanovg();
    if (!m_vg)
      throw XX;
  }

  NVGcontext *init_nanovg() {
    NVGcontext *vg =
        nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);
    int fontNormal = nvgCreateFont(vg, "sans", "../font/Roboto-Regular.ttf");
    if (fontNormal == -1) {
      printf("Could not add font italic.\n");
      return nullptr;
    }
    int fontBold = nvgCreateFont(vg, "sans-bold", "../font/Roboto-Bold.ttf");
    if (fontBold == -1) {
      printf("Could not add font bold.\n");
      return nullptr;
    }
    int fontEmoji = nvgCreateFont(vg, "emoji", "../font/NotoEmoji-Regular.ttf");
    if (fontEmoji == -1) {
      printf("Could not add font emoji.\n");
      return nullptr;
    }
    nvgAddFallbackFontId(vg, fontNormal, fontEmoji);
    nvgAddFallbackFontId(vg, fontBold, fontEmoji);
    return vg;
  }

  void init_imgui(GLFWwindow *window) {
    // Setup Dear ImGui binding
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard
    // Controls io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable
    // Gamepad Controls
    ImGui_ImplGlfwGL3_Init(window, true);

    // Setup style
    ImGui::StyleColorsDark();
  }

  GLFWwindow *create_window(int x_pixels, int y_pixels, const char *name) {
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
      return nullptr;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    GLFWwindow *window =
        glfwCreateWindow(1280, 720, "C++ Plotting Test", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync
    // gl3wInit();
    // gladLoadGL();
    if (!gladLoadGL()) {
      exit(-1);
    }
    std::cout << "OpenGL Version " << GLVersion.major << '.' << GLVersion.minor
              << std::endl;

    return window;
  }
}

} // namespace trase

#endif // FIGURE_H_
