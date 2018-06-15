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

#ifndef BACKENDSVG_H_
#define BACKENDSVG_H_

#include "BBox.hpp"
#include "Colors.hpp"
#include "Exception.hpp"
#include "Vector.hpp"

namespace trase {

class BackendSVG {
  std::ofstream m_out;
  std::string m_linewidth;
  std::string m_line_color;
  std::string m_fill_color; // rgb(255,255,255)
  std::string m_path;

public:
  void init(const char *filename, const char *name);
  void finalise();

  inline bool is_interactive() { return false; }

  inline void scissor(const bfloat2_t &x) {
    const auto &delta = x.delta();
    const auto &min = x.min();
    nvgScissor(m_vg, min[0], min[1], delta[0], delta[1]);
  }

  inline void reset_scissor() { nvgResetScissor(m_vg); }

  inline void begin_path() { m_path = "d=\""; }
  inline void rounded_rect(const bfloat2_t &x, const float r) {
    const auto &delta = x.delta();
    const auto &min = x.min();
    m_out << "<rect x=\"" << min[0] << "\" y=\"" << min[1] << "\" width=\""
          << delta[0] << "\" height=\"" << delta[1] << "\" rx=\"" << r << "\""
          << m_fill_color << m_line_color << m_linewidth << "/>\n";
  }
  inline void rect(const bfloat2_t &x) {
    const auto &delta = x.delta();
    const auto &min = x.min();
    m_out << "<rect x=\"" << min[0] << "\" y=\"" << min[1] << "\" width=\""
          << delta[0] << "\" height=\"" << delta[1] << "\"" << m_fill_color
          << m_line_color << m_linewidth << "/>\n";
  }

  inline void circle(const vfloat2_t &centre, float radius) {
    m_out << "<circle cx=\"" << centre[0] << "\" cy=\"" << centre[1]
          << "\" r=\"" << radius << "\"" << m_fill_color << m_line_color
          << m_linewidth << "/>\n";
  }

  inline void move_to(const vfloat2_t &x) {
    m_path += " M " << x[0] << ' ' << x[1];
  }
  inline void line_to(const vfloat2_t &x) {
    m_path += " L " << x[0] << ' ' << x[1];
  }
  inline void stroke_color(const RGBA &color) {
    m_linewidth =
        std::string("stroke=\"") + color.to_string() + str::string("\"");
  }
  inline void stroke_width(const float lw) {
    m_linewidth =
        std::string("stroke-width=\"") + std::to_string(lw) + str::string("\"");
  }
  inline void stroke() {
    m_path += '\"';
    m_out << "<path " << m_path << m_line_color << m_linewidth << "/>\n";
  }
  inline void fill() {
    m_path += '\"';
    m_out << "<path " << m_path << m_fill_color << m_line_color << m_linewidth
          << "/>\n";
  }
  inline void font_size(float size) { nvgFontSize(m_vg, size); }
  inline void font_face(const char *face) { nvgFontFace(m_vg, face); }
  inline void font_blur(const float blur) { nvgFontBlur(m_vg, blur); }
  inline void text_align(const int align) { nvgTextAlign(m_vg, align); }
  inline void fill_color(const RGBA &color) {
    m_fill_color =
        std::string("\"") + m_line_color.to_string() + str::string("\"");
  }

  inline void text(const vfloat2_t &x, const char *string, const char *end) {
    nvgText(m_vg, x[0], x[1], string, end);
  }
  inline void text_bounds(const vfloat2_t &x, const char *string,
                          const char *end, float *bounds) {
    nvgTextBounds(m_vg, x[0], x[1], string, end, bounds);
  }
};

} // namespace trase

#endif // BACKENDSVG_H_
