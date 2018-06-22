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
  std::string m_web_font;
  float m_from_time;
  int m_nanimate;
  int m_nframe;
  bool m_end_of_frames;
  Transform m_transform;

public:
  BackendSVG(std::ostream &out)
      : m_out(out), m_nanimate(0), m_nframe(0), m_end_of_frames(false) {
    stroke_color(RGBA(0, 0, 0, 255));
    fill_color(RGBA(0, 0, 0, 255));
    stroke_width(1);
  }
  void init(const float width, const float height, const char *name);
  void finalise();

  inline bool is_interactive() { return false; }

  inline void scissor(const bfloat2_t &x) {}

  inline void reset_scissor() {}

  inline void rotate(const float angle) { m_transform.rotate(angle); }
  inline void reset_transform() { m_transform.clear(); }
  inline void translate(const vfloat2_t &v) { m_transform.translate(v); }

  inline void begin_path() { m_path.clear(); }
  inline void begin_frames() {
    m_path.clear();
    m_path_from.clear();
    m_from_time = 0;
    m_nframe = 0;
    m_nanimate++;
  }
  inline void add_frame(const float time) {
    std::string frame_id =
        "id" + std::to_string(m_nanimate) + '_' + std::to_string(m_nframe);
    std::string prev_frame_id =
        "id" + std::to_string(m_nanimate) + '_' + std::to_string(m_nframe - 1);
    std::string last_frame_id = "id" + std::to_string(m_nanimate) + "_end";
    if (m_end_of_frames) {
      frame_id = last_frame_id;
    }

    if (m_nframe == 0 || m_path_from.empty()) {
      // write out start of path
      m_out << "<path d=\"" << m_path << "\" " << m_line_color << ' '
            << m_linewidth << " fill-opacity=\"0\">\n ";
    } else if (m_nframe == 1) {
      // write out first animate element
      m_out << "<animate id=\"" << frame_id
            << "\" attributeName=\"d\" attributeType=\"XML\" begin=\"1s;"
            << last_frame_id << ".end\" dur=\"" << time - m_from_time
            << "s\" fill=\"freeze\" from=\"" << m_path_from << "\" to=\""
            << m_path << "\" />\n";
    } else {
      // write out other animate element
      m_out << "<animate id=\"" << frame_id
            << "\" attributeName=\"d\" attributeType=\"XML\" begin=\""
            << prev_frame_id << ".end\" dur=\"" << time - m_from_time
            << "s\" fill=\"freeze\" from=\"" << m_path_from << "\" to=\""
            << m_path << "\" />\n";
    }
    m_path_from = m_path;
    m_path.clear();
    m_from_time = time;
    m_nframe++;
  }
  inline void end_frames(const float time) {
    m_end_of_frames = true;
    add_frame(time);
    m_end_of_frames = false;
    m_out << "</path>\n";
  }
  inline void rounded_rect(const bfloat2_t &x, const float r) { rect(x); }
  inline void rect(const bfloat2_t &x) {
    const auto &delta = x.delta();
    vfloat2_t point = x.min();
    begin_path();
    move_to(point);
    point[0] += delta[0];
    line_to(point);
    point[1] += delta[1];
    line_to(point);
    point[0] -= delta[0];
    line_to(point);
    close_path();
  }

  inline void circle(const vfloat2_t &centre, float radius) {
    m_out << "<circle cx=\"" << centre[0] << "\" cy=\"" << centre[1]
          << "\" r=\"" << radius << "\" " << m_fill_color << ' ' << m_line_color
          << ' ' << m_linewidth << "/>\n";

    /*
      arc(centre, radius, 0, pi);
      arc(centre, radius, pi, 2 * pi);
      */
  }

  inline void arc(const vfloat2_t &centre, const float radius,
                  const float angle0, const float angle1) {
    const vfloat2_t p0 =
        centre + radius * vfloat2_t(std::cos(angle0), std::sin(angle0));
    const vfloat2_t p1 =
        centre + radius * vfloat2_t(std::cos(angle1), std::sin(angle1));
    move_to(p0);
    m_path += " A " + std::to_string(radius) + ' ' + std::to_string(radius) +
              " 0 0 1 " + std::to_string(p1[0]) + ' ' + std::to_string(p1[1]);
  }

  inline void move_to(const vfloat2_t &x) {
    m_path += " M " + std::to_string(x[0]) + ' ' + std::to_string(x[1]);
  }
  inline void line_to(const vfloat2_t &x) {
    m_path += " L " + std::to_string(x[0]) + ' ' + std::to_string(x[1]);
  }
  inline void close_path() { m_path += " Z"; }

  inline void stroke_color(const RGBA &color) {
    m_line_color = std::string("stroke=\"") + color.to_rgb_string() +
                   "\" stroke-opacity=\"" + std::to_string(color.m_a / 255.0) +
                   '\"';
  };
  inline void stroke_width(const float lw) {
    m_linewidth =
        std::string("stroke-width=\"") + std::to_string(lw) + std::string("\"");
  }
  inline void stroke() {
    m_out << "<path d=\"" << m_path << "\" " << m_line_color << ' '
          << m_linewidth << " fill-opacity=\"0\"";
    if (!m_transform.is_identity()) {
      m_out << ' ' << m_transform.to_string();
    }
    m_out << "/>\n";
  }
  inline void fill() {
    m_out << "<path d=\"" << m_path << "\" " << m_fill_color << ' '
          << m_line_color << ' ' << m_linewidth;
    if (!m_transform.is_identity()) {
      m_out << ' ' << m_transform.to_string();
    }
    m_out << "/>\n";
  }
  inline void font_size(float size) {
    m_font_size = "font-size=\"" + std::to_string(size) + '\"';
  }

  inline void font_face(const char *face) {
    m_font_face = "font-family=\"" + std::string(face) + '\"';
  }

  inline void import_web_font(const std::string &url) { m_web_font = url; }

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
    std::string vert_align_text;
    if (align & ALIGN_TOP) {
      vert_align_text = "hanging";
    } else if (align & ALIGN_MIDDLE) {
      vert_align_text = "middle";
    } else if (align & ALIGN_BOTTOM) {
      vert_align_text = "baseline";
    }
    m_font_align = "text-anchor=\"" + align_text + "\" alignment-baseline=\"" +
                   vert_align_text + '\"';
  }
  inline void fill_color(const RGBA &color) {
    m_fill_color = "fill=\"" + color.to_rgb_string() + "\" fill-opacity=\"" +
                   std::to_string(color.m_a / 255.0) + '\"';
  }

  inline void text(const vfloat2_t &x, const char *string, const char *end) {
    m_out << "<text x=\"" << x[0] << "\" y=\"" << x[1] << "\" " << m_font_face
          << ' ' << m_font_size << ' ' << m_font_align << ' ' << m_fill_color;
    if (!m_transform.is_identity()) {
      m_out << ' ' << m_transform.to_string();
    }
    m_out << '>' << string << "</text>\n";
  }
};

} // namespace trase

#endif // BACKENDSVG_H_
