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
  /// the output file stream that the svg is written to
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
  std::string m_animate_stroke;
  std::string m_animate_stroke_opacity;
  std::string m_animate_fill;
  std::string m_animate_fill_opacity;
  std::string m_animate_times;
  float m_time_span;
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

  /// returns true if any of the mouseover features of this backend have been
  /// set
  bool mouseover() const noexcept;

public:
  /// create a new backend which will write out an animated SVG to the output
  /// stream @p out
  explicit BackendSVG(std::ostream &out);

  TRASE_BACKEND_VISITABLE()
  TRASE_ANIMATED_BACKEND_VISITABLE()

  /// Initialise the backend
  ///
  /// This function must be called before any other drawing calls
  ///
  /// @param pixels the image resolution in pixels
  /// @param name the name of the image
  /// @param time_span the total time span of the animated svg
  void init(const vfloat2_t &pixels, const char *name,
            float time_span = 0.f) noexcept;

  /// Finalise the backend.
  ///
  /// This function must be called after all drawing is complete
  void finalise() noexcept;

  /// returns false
  bool is_interactive();

  /// returns true
  bool should_close();

  /// not used (only used for interactive backends)
  vfloat2_t begin_frame();

  /// not used (only used for interactive backends)
  void end_frame();

  /// this backend is not interactive so return value is undefined
  vfloat2_t get_mouse_pos();

  /// not used (only used for interactive backends)
  float get_time();

  /// not used (only used for interactive backends)
  static void set_mouse_down(const vfloat2_t &mouse_pos);

  /// not used (only used for interactive backends)
  static void set_mouse_up();

  /// not used (only used for interactive backends)
  bool mouse_dragging();

  /// not used (only used for interactive backends)
  vfloat2_t mouse_drag_delta();

  /// not used (only used for interactive backends)
  void mouse_drag_reset_delta();

  /// all subsequent drawing calls will be cut to within this box
  void scissor(const bfloat2_t &x);

  /// resets any previous calls to @ref scissor()
  void reset_scissor();

  /// rotates the drawing transform by an angle given by @p angle
  void rotate(const float angle);

  /// resets the drawing transform to identity
  ///
  /// @see rotate(), translate()
  void reset_transform();

  /// translates the drawing transform by an amount given by @p v
  void translate(const vfloat2_t &v);

  /// begin a drawing path
  ///
  /// must be called before move_to(), line_to(), or close_path()
  void begin_path();

  /// move the "pen" to the point @p x without drawing a line
  ///
  /// @see begin_path()
  void move_to(const vfloat2_t &x);

  /// draw a line to the point @p x
  ///
  /// @see begin_path()
  void line_to(const vfloat2_t &x);

  /// draw a line to the first point of the path
  ///
  /// @see begin_path()
  void close_path();

  /// finalise a drawing path with a drawn line along the path
  ///
  /// @see begin_path()
  void stroke();

  /// finalise a drawing path with a drawn line and a filled enclosed domain
  ///
  /// @see begin_path()
  void fill();

  /// begin an animated path
  ///
  /// must be called before move_to(), line_to(), add_animated_path(), or
  /// end_animated_path()
  void begin_animated_path();

  /// finalise the current path and add a new path at time @p time
  ///
  /// Note that filled paths are not currently supported by the animated path
  ///
  /// @see begin_animated_path()
  void add_animated_path(const float time);

  /// finalise the current animated path
  ///
  /// Note that filled paths are not currently supported by the animated path
  ///
  /// @see begin_animated_path()
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

  /// start/continue an animated rectangle.
  /// subsequent calls to this method will add extra keyframe to the animation.
  ///
  /// @param x the bounding box of the rectangle
  /// @param stroke the stroke color of the circle
  /// @param fill the fill color of the circle
  /// @param time the time of the keyframe
  void add_animated_rect(const bfloat2_t &x, float time);

  /// Used in conjunction with add_animated_path, add_animated_rect,
  /// add_animated_circle, to provide animated stroke color/opacity
  ///
  /// Must by called once for each of the above methods
  ///
  /// @param color the stroke color of the current keyframe
  void add_animated_stroke(const RGBA &color);

  /// Used in conjunction with add_animated_path, add_animated_rect,
  /// add_animated_circle, to provide animated fill color/opacity
  ///
  /// Must by called once for each of the above methods
  ///
  /// @param color the fill color of the current keyframe
  void add_animated_fill(const RGBA &color);

  /// end an animated rectangle
  ///
  void end_animated_rect();

  /// draw a circle
  ///
  /// @param centre coordinates of the centre of the circle
  /// @param r the radius of the circle
  void circle(const vfloat2_t &centre, float r) noexcept;

  /// start/continue an animated circle
  /// subsequent calls to this method will add extra keyframe to the animation.
  ///
  /// @param centre the centre of the circle
  /// @param radius the radius of the circle
  /// @param color the color of the circle
  /// @param time the time of the keyframe
  void add_animated_circle(const vfloat2_t &centre, float radius, float time);

  /// end an animated circle
  ///
  void end_animated_circle();

  /// draw a circle with with a string label
  ///
  /// @param centre the centre of the circle
  /// @param radius the radius of the circle
  /// @param string the string text
  void circle_with_text(const vfloat2_t &centre, float radius,
                        const char *string);

  /// draw an arc
  ///
  /// @param centre the centre of the arc
  /// @parma radius the radius of the arc
  /// @param angle0 the start angle of the arc
  /// @param angle1 the end angle of the arc
  void arc(const vfloat2_t &centre, const float radius, const float angle0,
           const float angle1);

  /// set the current stroke color
  void stroke_color(const RGBA &color);

  /// set the current stroke color, with a different color to display on
  /// mouseover
  void stroke_color(const RGBA &color, const RGBA &color_mouseover);

  /// Set a tooltip
  ///
  /// This is used in prior to a call to rect() or circle(). Sets a tooltip text
  /// to display on mouseover on the rectangle or circle.
  ///
  /// @param x the position of the tooltip
  /// @param string the text of the tooltip
  void tooltip(const vfloat2_t &x, const char *string);

  /// Unset the tooltip
  ///
  /// @see tooltip()
  void clear_tooltip();

  /// sets the current stroke width
  void stroke_width(const float lw);

  /// set the current fill color
  void fill_color(const RGBA &color);

  /// set the current fill color, with a different color on mouseover
  void fill_color(const RGBA &color, const RGBA &color_mouseover);

  /// set the current font height
  void font_size(float size);

  /// sets the current font
  void font_face(const char *face);

  /// sets a web font
  ///
  /// must be called before init()
  void import_web_font(const std::string &url);

  /// set the amount of font blur on text
  void font_blur(const float blur);

  /// sets the current text alignment
  void text_align(const unsigned int align);

  /// draws text to a given position
  ///
  /// @param x the text position (see text_align())
  /// @param string a pointer to the text string
  /// @param end points to the end of the string. Use `nullptr` to write entire
  /// string
  void text(const vfloat2_t &x, const char *string, const char *end);

private:
  void end_animate(std::string &animate, const std::string &name);
  void end_animate_stroke();
  void end_animate_fill();
  void shape_styling();
};

} // namespace trase

#endif // BACKENDSVG_H_
