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
#include "Backend.hpp"
#include "Colors.hpp"
#include "Exception.hpp"
#include "Vector.hpp"

#include <ostream>

namespace trase {

class BackendSVG {
  std::ostream &m_out;
  std::string m_linewidth;
  std::string m_line_color;
  std::string m_fill_color;
  std::string m_path;
  std::string m_path_from;
  std::string m_font_face;
  std::string m_font_size;
  std::string m_font_align;
  float m_from_time;

public:
  BackendSVG(std::ostream &out) : m_out(out) {
    stroke_color(RGBA(0, 0, 0, 255));
    fill_color(RGBA(0, 0, 0, 255));
    stroke_width(1);
  }
  void init(const float width, const float height, const char *name);
  void finalise();

  inline bool is_interactive() { return false; }

  inline void scissor(const bfloat2_t &x) {}

  inline void reset_scissor() {}

  inline void begin_path() { m_path.clear(); }
  inline void begin_frames() {
    m_path.clear();
    m_path_from.clear();
    m_from_time = 0;
  }
  inline void add_frame(const float time) {
    if (m_path_from.empty()) {
      // write out start of path
      m_out << "<path d=\"" << m_path << "\" " << m_line_color << ' '
            << m_linewidth << ">\n ";
    } else {
      // write out animate element
      m_out << "<animate attributeName=\"d\" attributeType=\"XML\" begin=\""
            << m_from_time << "s\" dur=\"" << time - m_from_time
            << "s\" fill=\"freeze\" from=\"" << m_path_from << "\" to=\""
            << m_path << "\" />\n";
    }
    m_path_from = m_path;
    m_path.clear();
    m_from_time = time;
  }
  inline void end_frames(const float time) {
    add_frame(time);
    m_out << "</path>\n";
  }
  inline void rounded_rect(const bfloat2_t &x, const float r) {
    const auto &delta = x.delta();
    const auto &min = x.min();
    m_out << "<rect x=\"" << min[0] << "\" y=\"" << min[1] << "\" width=\""
          << delta[0] << "\" height=\"" << delta[1] << "\" rx=\"" << r << "\" "
          << m_fill_color << ' ' << m_line_color << ' ' << m_linewidth
          << "/>\n";
  }
  inline void rect(const bfloat2_t &x) {
    const auto &delta = x.delta();
    const auto &min = x.min();
    m_out << "<rect x=\"" << min[0] << "\" y=\"" << min[1] << "\" width=\""
          << delta[0] << "\" height=\"" << delta[1] << "\" " << m_fill_color
          << ' ' << m_line_color << ' ' << m_linewidth << "/>\n";
  }

  inline void circle(const vfloat2_t &centre, float radius) {
    m_out << "<circle cx=\"" << centre[0] << "\" cy=\"" << centre[1]
          << "\" r=\"" << radius << "\" " << m_fill_color << ' ' << m_line_color
          << ' ' << m_linewidth << "/>\n";
  }

  inline void move_to(const vfloat2_t &x) {
    m_path += " M " + std::to_string(x[0]) + ' ' + std::to_string(x[1]);
  }
  inline void line_to(const vfloat2_t &x) {
    m_path += " L " + std::to_string(x[0]) + ' ' + std::to_string(x[1]);
  }
  inline void stroke_color(const RGBA &color) {
    m_line_color =
        std::string("stroke=\"") + color.to_rgb_string() + std::string("\"");
  }
  inline void stroke_width(const float lw) {
    m_linewidth =
        std::string("stroke-width=\"") + std::to_string(lw) + std::string("\"");
  }
  inline void stroke() {
    m_out << "<path d=\"" << m_path << "\" " << m_line_color << ' '
          << m_linewidth << "/>\n";
  }
  inline void fill() {
    m_out << "<path d=\"" << m_path << "\" " << m_fill_color << ' '
          << m_line_color << ' ' << m_linewidth << "/>\n";
  }
  inline void font_size(float size) {
    m_font_size = "font-size=\"" + std::to_string(size) + '\"';
  }

  inline void font_face(const char *face) {
    m_font_face = "font-family=\"" + std::string(face) + '\"';
  }
  inline void font_blur(const float blur) {}
  inline void text_align(const int align) {
    std::string align_text;
    if (align & ALIGN_LEFT) {
      align_text = "start";
    } else if (align & ALIGN_CENTER) {
      align_text = "middle";
    } else if (align & ALIGN_RIGHT) {
      align_text = "end";
    }
    m_font_align = "text-anchor=\"" + align_text + '\"';
  }
  inline void fill_color(const RGBA &color) {
    m_fill_color = "fill=\"" + color.to_rgb_string() + std::string("\"");
  }

  inline void text(const vfloat2_t &x, const char *string, const char *end) {
    m_out << "<text x=\"" << x[0] << "\" y=\"" << x[1] << "\" " << m_font_face
          << ' ' << m_font_size << ' ' << m_font_align << ' ' << m_fill_color
          << '>' << string << "</text>\n";
  }
};

} // namespace trase

#endif // BACKENDSVG_H_
