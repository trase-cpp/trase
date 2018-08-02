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

/// \file BackendSVG.hpp

#ifndef BACKENDSVG_H_
#define BACKENDSVG_H_

#include "backend/Backend.hpp"
#include "util/BBox.hpp"
#include "util/Colors.hpp"
#include "util/Exception.hpp"
#include "util/Vector.hpp"

#include <iomanip>
#include <ostream>
#include <sstream>

namespace trase {

/// Helper class for BackendSVG to format attributes as 'name="val" '
class AttributeFormatter {
  std::stringstream m_ss;
  int m_precision = 4;

public:
  AttributeFormatter() { m_ss << std::setprecision(m_precision); }

  /// Format as 'name="val" ' (inc quotes and space), and return a string
  ///
  /// @tparam T type of val
  /// @param name name of attribute
  /// @param val value of attribute
  /// @return string of 'name="val" '
  template <typename T>
  std::string operator()(const std::string &name, const T val) {
    m_ss.str("");
    m_ss << name << "=\"" << val << "\" ";
    return m_ss.str();
  }
};

class BackendSVG : public AnimatedBackend {
  std::ostream &m_out;
  std::string m_linewidth;
  std::string m_line_color;
  std::string m_fill_color;
  std::string m_path;
  std::string m_font_face;
  std::string m_font_size;
  std::string m_font_align;
  std::string m_web_font;
  std::string m_onmouseover_stroke;
  std::string m_onmouseout_stroke;
  std::string m_onmouseover_fill;
  std::string m_onmouseout_fill;
  std::string m_onmouseover_tooltip;
  std::string m_onmouseout_tooltip;
  std::vector<std::string> m_animate_values;
  std::string m_animate_times;
  float m_time_span;
  float m_old_time;
  std::string m_font_size_base;
  std::string m_font_face_base;
  TransformMatrix m_transform;
  AttributeFormatter m_att;

  /// Add the opening circle tag to m_out
  /// @param centre coordinates of the centre of the circle
  /// @param r radius of the circle
  void circle_begin(const vfloat2_t &centre, float r) noexcept;

  /// Add the closing circle tag to m_out
  void circle_end() noexcept;

  /// Add the opening rect tag to m_out
  ///
  /// @param x the bounding box of the rectangle
  /// @param r the radius of the circle used to round the corners
  void rect_begin(const bfloat2_t &x, float r) noexcept;

  /// Add the closing rect tag to m_out
  void rect_end() noexcept;

public:
  explicit BackendSVG(std::ostream &out) : m_out(out) {
    stroke_color({0, 0, 0, 255});
    fill_color({0, 0, 0, 255});
    stroke_width(1);
  }

  void init(float width, float height, float time_span,
            const char *name) noexcept;

  void finalise() noexcept;

  inline bool is_interactive() { return false; }

  inline void scissor(const bfloat2_t &x) {}

  inline void reset_scissor() {}

  inline void rotate(const float angle) { m_transform.rotate(angle); }
  inline void reset_transform() { m_transform.clear(); }
  inline void translate(const vfloat2_t &v) { m_transform.translate(v); }

  inline void begin_path() { m_path.clear(); }

  bool mouseover() const noexcept;

  inline void begin_animated_path() {
    if (m_animate_values.empty()) {
      m_animate_values.resize(1);
    }
    m_animate_times = "keyTimes=\"";
    m_animate_values[0] = "values=\"";
    begin_path();
  }
  inline void add_animated_path(const float time) {

    // if its the first frame need to write the path element
    // TODO: not sure why need to add d attribute here, but neccessary for
    // single frame animations. Figure out how to remove this as it doubles the
    // required space
    if (m_animate_times.back() == '\"') {
      m_out << "<path " << m_line_color << ' ' << m_linewidth
            << " fill-opacity=\"0\" d=\"" << m_path << "\">\n ";
    }

    // all times are scaled by total time span (all times start at 0)
    m_animate_times += std::to_string(time / m_time_span) + ';';
    m_animate_values[0] += m_path + ';';
    m_path.clear();
  }

  inline void end_animated_path(const float time) {
    add_animated_path(time);
    m_animate_times.back() = '\"';
    m_animate_values[0].back() = '\"';
    m_out << "<animate attributeName=\"d\" "
             //"calcMode=\"discrete\" "
             "repeatCount=\"indefinite\" "
             "begin =\"0s\" "
             "dur=\""
          << m_time_span << "s\" " << m_animate_values[0] << ' '
          << m_animate_times << "/>\n";
    m_out << "</path>\n";
    m_animate_times.clear();
    m_animate_values[0].clear();
  }

  /// draw a rectangle with rounded corners
  ///
  /// @param x the bounding box of the rectangle
  /// @param r the radius of the circle used to round the corners
  void rounded_rect(const bfloat2_t &x, float r) noexcept;

  /// draw a rectangle, optionally with rounded corners
  ///
  /// @param x the bounding box of the rectangle
  /// @param r the radius of the circle used to round the corners, default 0.f
  void rect(const bfloat2_t &x, float r = 0.f) noexcept;

  /// start/continue an animated rectangle, optimally with rounded corners.
  /// subsequent calls to this method will add extra keyframe to the animation.
  ///
  /// @param x the bounding box of the rectangle
  /// @param r the radius of the circle used to round the corners, default 0.f
  /// @param time the time of the keyframe
  void add_animated_rect(const bfloat2_t &x, float time);

  /// end an animated rectangle
  ///
  void end_animated_rect();

  /// draw a circle
  ///
  /// @param centre coordinates of the centre of the circle
  /// @param r the radius of the circle
  void circle(const vfloat2_t &centre, float r) noexcept;

  inline void add_animated_circle(const vfloat2_t &centre, float radius,
                                  float time) {
    // check if first circle
    if (m_animate_times.empty()) {
      circle_begin(centre, radius);

      if (m_animate_values.size() < 3) {
        m_animate_values.resize(3);
      }
      m_animate_times =
          "keyTimes=\"" + std::to_string(time / m_time_span) + ';';
      m_animate_values[0] = "values=\"" + std::to_string(centre[0]) + ';';
      m_animate_values[1] = "values=\"" + std::to_string(centre[1]) + ';';
      m_animate_values[2] = "values=\"" + std::to_string(radius) + ';';
    } else {
      m_animate_times += std::to_string(time / m_time_span) + ';';
      m_animate_values[0] += std::to_string(centre[0]) + ';';
      m_animate_values[1] += std::to_string(centre[1]) + ';';
      m_animate_values[2] += std::to_string(radius) + ';';
    }
  }

  inline void end_animated_circle() {

    m_animate_times.back() = '\"';
    for (int i = 0; i < 3; ++i) {
      m_animate_values[i].back() = '\"';
    }
    const std::string names[3] = {"cx", "cy", "r"};
    for (int i = 0; i < 3; ++i) {
      m_out << "<animate attributeName=\"" + names[i] +
                   "\" repeatCount=\"indefinite\" begin =\"0s\" dur=\""
            << m_time_span << "s\" " << m_animate_values[i] << ' '
            << m_animate_times << "/>\n";
    }
    circle_end();
    m_animate_times.clear();

    for (int i = 0; i < 3; ++i) {
      m_animate_values[i].clear();
    }
  }

  inline void circle_with_text(const vfloat2_t &centre, float radius,
                               const char *string) {
    m_out << "<circle cx=\"" << centre[0] << "\" cy=\"" << centre[1]
          << "\" r=\"" << radius << "\" " << m_fill_color << ' ' << m_line_color
          << ' ' << m_linewidth
          << " onmouseover=\"evt.target.setAttribute('stroke-opacity','1.0');"
             "\" onmouseout=\"bob.setAttribute('stroke-opacity', '0.0');"
             "\"/>\n";
    auto text_pos = centre + 2.f * vfloat2_t(radius, -radius);
    m_out << "<text id=\"bob\" x=\"" << text_pos[0] << "\" y=\"" << text_pos[1]
          << "\" " << m_font_face << ' ' << m_font_size << ' ' << m_font_align
          << ' ' << m_fill_color << '>' << string << "</text>\n";
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
                   "\" stroke-opacity=\"" + std::to_string(color.a() / 255.0) +
                   '\"';
    m_onmouseover_stroke.clear();
    m_onmouseout_stroke.clear();
  };

  inline void stroke_color(const RGBA &color, const RGBA &color_mouseover) {
    stroke_color(color);
    m_onmouseover_stroke = "evt.target.setAttribute('stroke', '" +
                           color_mouseover.to_rgb_string() +
                           "'); evt.target.setAttribute('stroke-opacity','" +
                           std::to_string(color_mouseover.a() / 255.0) + "');";
    m_onmouseout_stroke = "evt.target.setAttribute('stroke', '" +
                          color.to_rgb_string() +
                          "'); evt.target.setAttribute('stroke-opacity','" +
                          std::to_string(color.a() / 255.0) + "');";
  }

  inline void tooltip(const vfloat2_t &x, const char *string) {
    m_onmouseover_tooltip = "tooltip(" + std::to_string(x[0]) + ',' +
                            std::to_string(x[1]) + ",'" + string + "'," +
                            m_font_size_base + ",'" + m_font_face_base + "');";
    m_onmouseout_tooltip = "remove_tooltip();";
  }

  inline void clear_tooltip() {
    m_onmouseover_tooltip.clear();
    m_onmouseout_tooltip.clear();
  }

  inline void stroke_width(const float lw) {
    m_linewidth =
        std::string("stroke-width=\"") + std::to_string(lw) + std::string("\"");
  }
  inline void fill_color(const RGBA &color) {
    m_fill_color = "fill=\"" + color.to_rgb_string() + "\" fill-opacity=\"" +
                   std::to_string(color.a() / 255.0) + '\"';
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
    m_onmouseover_fill.clear();
    m_onmouseout_fill.clear();
  }

  inline void fill_color(const RGBA &color, const RGBA &color_mouseover) {
    fill_color(color);
    m_onmouseover_fill = "evt.target.setAttribute('fill', '" +
                         color_mouseover.to_rgb_string() +
                         "'); evt.target.setAttribute('fill-opacity','" +
                         std::to_string(color_mouseover.a() / 255.0) + "');";
    m_onmouseout_fill = "evt.target.setAttribute('fill', '" +
                        color.to_rgb_string() +
                        "'); evt.target.setAttribute('fill-opacity','" +
                        std::to_string(color.a() / 255.0) + "');";
  }

  inline void font_size(float size) {
    m_font_size_base = std::to_string(size);
    m_font_size = "font-size=\"" + std::to_string(size) + '\"';
  }

  inline void font_face(const char *face) {
    m_font_face_base = std::string(face);
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
