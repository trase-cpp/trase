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

  TRASE_BACKEND_VISITABLE()
  TRASE_ANIMATED_BACKEND_VISITABLE()

  void init(const vfloat2_t &pixels, const char *name,
            float time_span = 0.f) noexcept;

  void finalise() noexcept;

  bool is_interactive();

  vfloat2_t get_mouse_pos();

  void scissor(const bfloat2_t &x);

  void reset_scissor();

  void rotate(const float angle);
  void reset_transform();
  void translate(const vfloat2_t &v);

  void begin_path();

  bool mouseover() const noexcept;

  void begin_animated_path();
  void add_animated_path(const float time);

  void end_animated_path(const float time);

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

  void add_animated_circle(const vfloat2_t &centre, float radius, float time);

  void end_animated_circle();

  void circle_with_text(const vfloat2_t &centre, float radius,
                        const char *string);

  void arc(const vfloat2_t &centre, const float radius, const float angle0,
           const float angle1);

  void move_to(const vfloat2_t &x);
  void line_to(const vfloat2_t &x);
  void close_path();

  void stroke_color(const RGBA &color);

  void stroke_color(const RGBA &color, const RGBA &color_mouseover);

  void tooltip(const vfloat2_t &x, const char *string);

  void clear_tooltip();
  void stroke_width(const float lw);
  void fill_color(const RGBA &color);
  void stroke();
  void fill();

  void fill_color(const RGBA &color, const RGBA &color_mouseover);

  void font_size(float size);
  void font_face(const char *face);

  void import_web_font(const std::string &url);

  void font_blur(const float blur);
  void text_align(const unsigned int align);
  void text(const vfloat2_t &x, const char *string, const char *end);
};

} // namespace trase

#endif // BACKENDSVG_H_
