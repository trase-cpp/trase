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

/// \file BackendGL.hpp

#ifndef BACKENDGL_H_
#define BACKENDGL_H_

#include <glad.h>
// works for you.
#define GLFW_INCLUDE_GLEXT
#include <GLFW/glfw3.h>

#include <array>
#include <cstdio>
#include <iostream>

#include "imgui.h"
#include "nanovg.h"

#include "backend/Backend.hpp"
#include "util/BBox.hpp"
#include "util/Colors.hpp"
#include "util/Exception.hpp"
#include "util/Vector.hpp"

namespace trase {

class BackendGL : public Backend {
  GLFWwindow *m_window;
  NVGcontext *m_vg;
  FontManager m_fm;
  RGBA m_stroke_color_mouseover;
  RGBA m_fill_color_mouseover;

public:
  TRASE_BACKEND_VISITABLE()

  void init(const vfloat2_t &pixels, const char *name);
  void finalise();
  vfloat2_t begin_frame();
  void end_frame();

  inline bool is_interactive() { return true; }

  inline float get_time() { return ImGui::GetTime(); }

  inline vfloat2_t get_mouse_pos() {
    auto pos = ImGui::GetMousePos();
    return {pos[0], pos[1]};
  }

  inline bool mouse_dragging() { return ImGui::IsMouseDragging(); }

  vfloat2_t mouse_drag_delta() {
    ImVec2 delta = ImGui::GetMouseDragDelta();
    return {delta[0], delta[1]};
  }

  inline void mouse_drag_reset_delta() { ImGui::ResetMouseDragDelta(); }

  void scissor(const bfloat2_t &x);

  void reset_scissor();

  void rotate(const float angle);
  void translate(const vfloat2_t &v);
  void reset_transform();

  void begin_path();
  void rounded_rect(const bfloat2_t &x, const float r);
  void rect(const bfloat2_t &x);

  void circle(const vfloat2_t &centre, float radius);

  void move_to(const vfloat2_t &x);
  void line_to(const vfloat2_t &x);
  void stroke_color(const RGBA &color);

  void stroke_width(const float lw);
  void stroke();
  void fill();
  void font_size(float size);
  void font_face(const char *face);
  void font_blur(const float blur);
  void text_align(const int align);
  void fill_color(const RGBA &color);

  void text(const vfloat2_t &x, const char *string, const char *end);

  bfloat2_t text_bounds(const vfloat2_t &x, const char *string);

  bool should_close();

private:
  NVGcontext *init_nanovg(int x_pixels, int y_pixels);
  void init_imgui(GLFWwindow *window);
  GLFWwindow *create_window(int x_pixels, int y_pixels, const char *name);
};

} // namespace trase

#endif // BACKENDGL_H_
