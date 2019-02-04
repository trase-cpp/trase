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

// forward declare GL and nanovg stuff (included in BackendGL.cpp)
struct NVGcontext;
struct GLFWwindow;

#include <array>
#include <cstdio>
#include <iostream>

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
  static bool m_lbutton_down;
  static vfloat2_t m_lbutton_down_mouse_pos;

public:
  TRASE_BACKEND_VISITABLE()

  /// Initialise this backend. Call before any drawing
  /// @param pixels the size of the window in pixels
  /// @param name the name of the window
  void init(const vfloat2_t &pixels, const char *name);

  /// Finalise this backend. Call after all drawing finished
  void finalise();

  /// Begin a new frame. All drawing calls must be between begin_frame() and
  /// end_frame()
  vfloat2_t begin_frame();

  /// End the current frame. All drawing calls must be between begin_frame() and
  /// end_frame()
  void end_frame();

  /// Return true if this backend has mouse interaction
  bool is_interactive();

  /// Returns true if the current window should close (e.g. due to user
  /// interaction)
  bool should_close();

  /// Get the current time
  float get_time();

  static void set_mouse_down(const vfloat2_t &mouse_pos);
  static void set_mouse_up();

  /// Get the current position of the mouse in pixel units
  vfloat2_t get_mouse_pos();

  /// Returns true if the mouse button is clicked and the mouse is dragging
  bool mouse_dragging();

  /// Current delta position of the dragging mouse
  vfloat2_t mouse_drag_delta();

  /// Reset drag delta to zero
  void mouse_drag_reset_delta();

  /// All subsequent drawing calls will be masked to only show within the
  /// bounding box @p x
  void scissor(const bfloat2_t &x);

  /// Remove a previously call to scissor()
  void reset_scissor();

  /// Apply a rotation of @p angle to the current drawing transform
  void rotate(const float angle);

  /// Apply a translation of @p v to the current drawing transform
  void translate(const vfloat2_t &v);

  /// Reset the drawing transform to the identity matrix
  void reset_transform();

  /// Begin a path. Subsequent calls to move_to() or line_to() defines the path
  /// to be drawn. The current path is completed by a call to stroke() or fill()
  void begin_path();

  /// Extends the current path. Moves the current "pen" position to the position
  /// @p x
  /// @see begin_path()
  void move_to(const vfloat2_t &x);

  /// Extends the current path. Draws a straight line from the current "pen"
  /// position to @p x
  /// @see begin_path()
  void line_to(const vfloat2_t &x);

  /// Draw a line along the completed path
  /// @see begin_path()
  void stroke();

  /// Draw a line along the completed path and fill it in.
  /// @see begin_path()
  void fill();

  /// Set the current stroke width
  /// @see stroke()
  void stroke_width(const float lw);

  /// Set the current stroke color
  /// @see stroke()
  void stroke_color(const RGBA &color);

  /// Set the current fill color
  /// @see fill()
  void fill_color(const RGBA &color);

  /// Draw a rectangle with rounded corners
  /// @param x the bounding box describing the rectangle
  /// @param r the radius of the rounded corners
  void rounded_rect(const bfloat2_t &x, const float r);

  /// Draw a rectangle
  /// @param x the bounding box describing the rectangle
  void rect(const bfloat2_t &x);

  /// Draw a circle with a given @p centre and @p radius
  void circle(const vfloat2_t &centre, float radius);

  /// Draw the given text to the screen
  /// @param x the position to draw the text
  /// @param a pointer to the text
  /// @param end the end of the text to draw (use nullptr to draw entire string)
  void text(const vfloat2_t &x, const char *string, const char *end);

  /// Returns the bounding box covering the given text
  /// @param x the drawing position of the text
  /// @param string pointer to the text
  bfloat2_t text_bounds(const vfloat2_t &x, const char *string);

  /// Sets the current font height in pixels
  /// @see text()
  void font_size(float size);

  /// Sets the current font name
  /// @see text()
  void font_face(const char *face);

  /// Sets the amount of blur to apply to the drawn text
  /// @see text()
  void font_blur(const float blur);

  /// Sets the alignment of the text to its position
  /// @see text()
  void text_align(const int align);

private:
  NVGcontext *init_nanovg(const vint2_t &pixels);
  GLFWwindow *create_window(const vint2_t &pixels, const char *name);
};

} // namespace trase

#endif // BACKENDGL_H_
