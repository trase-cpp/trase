/*
Copyright (c) 2018, University of Oxford.
All rights reserved.

University of Oxford means the Chancellor, Masters and Scholars of the
University of Oxford, having an administrative office at Wellington
Square, Oxford OX1 2JD, UK.

This file is part of trase.

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

#include "BackendGL.hpp"

#define GLFW_INCLUDE_ES2
#define GLFW_INCLUDE_GLEXT
#include <GLFW/glfw3.h>

#include "nanovg.h"

// Needs to go in the cpp file (should only be included once)
//#define NANOVG_GL3_IMPLEMENTATION
#define NANOVG_GLES2_IMPLEMENTATION
#include "nanovg_gl.h"
#include "nanovg_gl_utils.h"

namespace trase {
static void glfw_error_callback(int error, const char *description) {
  fprintf(stderr, "Error %d: %s\n", error, description);
}

bool BackendGL::m_lbutton_down = false;
vfloat2_t BackendGL::m_lbutton_down_mouse_pos;

static void glfw_mouse_callback(GLFWwindow *window, int button, int action,
                                int mods) {
  if (button == GLFW_MOUSE_BUTTON_LEFT) {
    if (GLFW_PRESS == action) {
      double xpos, ypos;
      glfwGetCursorPos(window, &xpos, &ypos);
      BackendGL::set_mouse_down(
          {static_cast<float>(xpos), static_cast<float>(ypos)});
    } else if (GLFW_RELEASE == action) {
      BackendGL::set_mouse_up();
    }
  }
}

void BackendGL::init(const vfloat2_t &pixels, const char *name) {
  m_window = create_window(static_cast<vint2_t>(pixels), name);
  if (!m_window)
    throw Exception("trase: Cannot create OpenGL window");
  m_vg = init_nanovg(static_cast<vint2_t>(pixels));
}

bool BackendGL::is_interactive() { return true; }

float BackendGL::get_time() { return glfwGetTime(); }

void BackendGL::set_mouse_down(const vfloat2_t &mouse_pos) {
  m_lbutton_down = true;
  m_lbutton_down_mouse_pos = mouse_pos;
}
void BackendGL::set_mouse_up() { m_lbutton_down = false; }

vfloat2_t BackendGL::get_mouse_pos() {
  double xpos, ypos;
  glfwGetCursorPos(m_window, &xpos, &ypos);
  return {static_cast<float>(xpos), static_cast<float>(ypos)};
}

bool BackendGL::mouse_dragging() {
  return m_lbutton_down && (get_mouse_pos() != m_lbutton_down_mouse_pos).all();
}

vfloat2_t BackendGL::mouse_drag_delta() {
  if (!m_lbutton_down) {
    return {0.f, 0.f};
  }
  return get_mouse_pos() - m_lbutton_down_mouse_pos;
}

void BackendGL::mouse_drag_reset_delta() {
  m_lbutton_down_mouse_pos = get_mouse_pos();
}

void BackendGL::scissor(const bfloat2_t &x) {
  const auto &delta = x.delta();
  const auto &min = x.min();
  nvgScissor(m_vg, min[0], min[1], delta[0], delta[1]);
}

void BackendGL::reset_scissor() { nvgResetScissor(m_vg); }

void BackendGL::rotate(const float angle) { nvgRotate(m_vg, angle); }
void BackendGL::translate(const vfloat2_t &v) {
  nvgTranslate(m_vg, v[0], v[1]);
}
void BackendGL::reset_transform() { nvgResetTransform(m_vg); }

void BackendGL::begin_path() { nvgBeginPath(m_vg); }
void BackendGL::rounded_rect(const bfloat2_t &x, const float r) {
  const auto &delta = x.delta();
  const auto &min = x.min();
  begin_path();
  nvgRoundedRect(m_vg, min[0], min[1], delta[0], delta[1], r);
  fill();
}
void BackendGL::rect(const bfloat2_t &x) {
  const auto &delta = x.delta();
  const auto &min = x.min();
  begin_path();
  nvgRect(m_vg, min[0], min[1], delta[0], delta[1]);
  fill();
}

void BackendGL::circle(const vfloat2_t &centre, float radius) {
  begin_path();
  nvgArc(m_vg, centre[0], centre[1], radius, 0, 2 * M_PI, NVG_CW);
  fill();
}

void BackendGL::move_to(const vfloat2_t &x) { nvgMoveTo(m_vg, x[0], x[1]); }
void BackendGL::line_to(const vfloat2_t &x) { nvgLineTo(m_vg, x[0], x[1]); }
void BackendGL::stroke_color(const RGBA &color) {
  nvgStrokeColor(m_vg, nvgRGBA(color.r(), color.g(), color.b(), color.a()));
}

void BackendGL::stroke_width(const float lw) { nvgStrokeWidth(m_vg, lw); }
void BackendGL::stroke() { nvgStroke(m_vg); }
void BackendGL::fill() { nvgFill(m_vg); }
void BackendGL::font_size(float size) { nvgFontSize(m_vg, size); }
void BackendGL::font_face(const char *face) {
  if (nvgFindFont(m_vg, face) == -1) {
    auto filename = m_fm.find_font(face, "");
    if (filename.empty()) {
      throw Exception("Could not find font" + std::string(face));
    }
    int font_id = nvgCreateFont(m_vg, face, filename.c_str());
    if (font_id == -1) {
      throw Exception("Could not add font" + filename);
    }
  }
  nvgFontFace(m_vg, face);
}
void BackendGL::font_blur(const float blur) { nvgFontBlur(m_vg, blur); }
void BackendGL::text_align(const int align) { nvgTextAlign(m_vg, align); }
void BackendGL::fill_color(const RGBA &color) {
  nvgFillColor(m_vg, nvgRGBA(color.r(), color.g(), color.b(), color.a()));
}

void BackendGL::text(const vfloat2_t &x, const char *string, const char *end) {
  nvgText(m_vg, x[0], x[1], string, end);
}

bfloat2_t BackendGL::text_bounds(const vfloat2_t &x, const char *string) {
  bfloat2_t ret;
  float bounds[4];
  nvgTextBounds(m_vg, x[0], x[1], string, nullptr, bounds);
  ret.bmin[0] = bounds[0];
  ret.bmin[1] = bounds[1];
  ret.bmax[0] = bounds[2];
  ret.bmax[1] = bounds[3];
  return ret;
}

bool BackendGL::should_close() {
  return static_cast<bool>(glfwWindowShouldClose(m_window));
}

void BackendGL::finalise() {
  // Cleanup
  nvgDeleteGLES2(m_vg);

  glfwDestroyWindow(m_window);
  glfwTerminate();
}

vfloat2_t BackendGL::begin_frame() {
  glfwPollEvents();

  int winWidth, winHeight;
  int fbWidth, fbHeight;

  // glfwGetCursorPos(window, &mx, &my);
  glfwGetWindowSize(m_window, &winWidth, &winHeight);
  glfwGetFramebufferSize(m_window, &fbWidth, &fbHeight);
  // Calculate pixel ration for hi-dpi devices.
  float pxRatio = (float)fbWidth / (float)winWidth;

  glViewport(0, 0, fbWidth, fbHeight);
  // glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  // glClear(GL_COLOR_BUFFER_BIT);

  nvgBeginFrame(m_vg, winWidth, winHeight, pxRatio);

  // nvgResetTransform(m_vg);
  // nvgScale(m_vg, winWidth / 100.0, winHeight / 100.0);
  return vfloat2_t(winWidth, winHeight);
}

void BackendGL::end_frame() {
  nvgEndFrame(m_vg);
  // Rendering
  glfwSwapBuffers(m_window);
}

GLFWwindow *BackendGL::create_window(const vint2_t &pixels, const char *name) {
  // Setup window
  glfwSetErrorCallback(glfw_error_callback);
  // glfwSetWindowRefreshCallback(m_handle, glfw_window_refresh_callback);
  if (!glfwInit()) {
    throw Exception("Could not initialise GLFW");
  }
  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  GLFWwindow *window = glfwCreateWindow(pixels[0], pixels[1], name, NULL, NULL);

  if (!window)
    throw Exception("Could not create GLFW window");

  glfwSetMouseButtonCallback(window, glfw_mouse_callback);
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1); // Enable vsync
  return window;
}

NVGcontext *BackendGL::init_nanovg(const vint2_t &pixels) {
  NVGcontext *vg =
      nvgCreateGLES2(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);
  if (!vg)
    throw Exception("trase: Cannot init NanoVG");
  auto filename = m_fm.find_font("Roboto-Regular", "");
  int fontNormal = nvgCreateFont(vg, "Roboto", filename.c_str());
  if (fontNormal == -1) {
    throw Exception("Could not add font Roboto");
  }
  filename = m_fm.find_font("Roboto-Bold", "");
  int fontBold = nvgCreateFont(vg, "Roboto-Bold", filename.c_str());
  if (fontBold == -1) {
    throw Exception("Could not add font bold");
  }
  filename = m_fm.find_font("NotoEmoji-Regular", "");
  int fontEmoji = nvgCreateFont(vg, "NotoEmoji-Regular", filename.c_str());
  if (fontEmoji == -1) {
    throw Exception("Could not add font emoji");
  }
  nvgAddFallbackFontId(vg, fontNormal, fontEmoji);
  nvgAddFallbackFontId(vg, fontBold, fontEmoji);
  return vg;
}

} // namespace trase
