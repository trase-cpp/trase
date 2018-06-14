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
#include <array>
#include <iostream>
#include <stdio.h>

#include "nanovg.h"

#include "BBox.hpp"
#include "Colors.hpp"
#include "Exception.hpp"
#include "Vector.hpp"

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

enum ArcDirection {
  CLOCKWISE = 1 << 0,
  COUNTER_CLOCKWISE = 1 << 1,
};

class BackendGL {
  GLFWwindow *m_window;
  NVGcontext *m_vg;

public:
  void init(int x_pixels, int y_pixels, const char *name);
  void finalise();
  vfloat2_t begin_frame();
  void end_frame();

  inline bool is_interactive() { return true; }

  inline float get_time() { return ImGui::GetTime(); }

  inline vfloat2_t get_mouse_pos() {
    auto pos = ImGui::GetMousePos();
    return vfloat2_t(pos[0], pos[1]);
  }

  inline bool mouse_dragging() { return ImGui::IsMouseDragging(); }

  vfloat2_t mouse_drag_delta() {
    ImVec2 delta = ImGui::GetMouseDragDelta();
    return vfloat2_t(delta[0], delta[1]);
  }

  inline void mouse_drag_reset_delta() { ImGui::ResetMouseDragDelta(); }

  inline void scissor(const bfloat2_t &x) {
    const auto &delta = x.delta();
    const auto &min = x.min();
    nvgScissor(m_vg, min[0], min[1], delta[0], delta[1]);
  }

  inline void reset_scissor() { nvgResetScissor(m_vg); }

  inline void begin_path() { nvgBeginPath(m_vg); }
  inline void rounded_rect(const bfloat2_t &x, const float r) {
    const auto &delta = x.delta();
    const auto &min = x.min();
    nvgRoundedRect(m_vg, min[0], min[1], delta[0], delta[1], r);
  }
  inline void rect(const bfloat2_t &x) {
    const auto &delta = x.delta();
    const auto &min = x.min();
    nvgRect(m_vg, min[0], min[1], delta[0], delta[1]);
  }

  inline void circle(const vfloat2_t &centre, float radius) {
    nvgArc(m_vg, centre[0], centre[1], radius, 0, 2 * M_PI, NVG_CW);
  }

  inline void move_to(const vfloat2_t &x) { nvgMoveTo(m_vg, x[0], x[1]); }
  inline void line_to(const vfloat2_t &x) { nvgLineTo(m_vg, x[0], x[1]); }
  inline void stroke_color(const RGBA &color) {
    nvgStrokeColor(m_vg, nvgRGBA(color.m_r, color.m_g, color.m_b, color.m_a));
  }
  inline void stroke_width(const float lw) { nvgStrokeWidth(m_vg, lw); }
  inline void stroke() { nvgStroke(m_vg); }
  inline void fill() { nvgFill(m_vg); }
  inline void font_size(float size) { nvgFontSize(m_vg, size); }
  inline void font_face(const char *face) { nvgFontFace(m_vg, face); }
  inline void font_blur(const float blur) { nvgFontBlur(m_vg, blur); }
  inline void text_align(const int align) { nvgTextAlign(m_vg, align); }
  inline void fill_color(const RGBA &color) {
    nvgFillColor(m_vg, nvgRGBA(color.m_r, color.m_g, color.m_b, color.m_a));
  }

  inline void text(const vfloat2_t &x, const char *string, const char *end) {
    nvgText(m_vg, x[0], x[1], string, end);
  }
  inline void text_bounds(const vfloat2_t &x, const char *string,
                          const char *end, float *bounds) {
    nvgTextBounds(m_vg, x[0], x[1], string, end, bounds);
  }

  inline bool should_close() { return glfwWindowShouldClose(m_window); }

private:
  NVGcontext *init_nanovg(int x_pixels, int y_pixels);
  void init_imgui(GLFWwindow *window);
  GLFWwindow *create_window(int x_pixels, int y_pixels, const char *name);
}; // namespace trase

} // namespace trase

#endif // BACKENDGL_H_
