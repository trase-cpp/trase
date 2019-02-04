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

/// \file Backend.hpp

#ifndef BACKEND_H_
#define BACKEND_H_

#include <cmath>
#include <sstream>
#include <vector>

#include "util/Vector.hpp"

namespace trase {

class Drawable;

/// a base class for all the backends that support drawing a single frame
class Backend {
public:
  // Declare overloads for each kind of a Drawable to dispatch
  virtual void accept(Drawable &drawable, float time) = 0;
};

#define TRASE_BACKEND_VISITABLE()                                              \
  void accept(Drawable &drawable, float time) override {                       \
    drawable.dispatch(*this, time);                                            \
  }

/// a base class for all the backends that support animation over time
class AnimatedBackend : public Backend {
public:
  // Can still accept a single time draw
  using Backend::accept;

  // Declare overloads for each kind of a Drawable to dispatch
  virtual void accept(Drawable &drawable) = 0;
};

#define TRASE_ANIMATED_BACKEND_VISITABLE()                                     \
  void accept(Drawable &drawable) override { drawable.dispatch(*this); }

// pi constant
const float pi =
    3.141592653589793238462643383279502884197169399375105820974944592307816406286f;

/// a transform matrix in the form
///
/// [a c e]
/// [b d f]
/// [0 0 1]
///
struct TransformMatrix {
  float a{1}, b{0}, c{0}, d{1}, e{0}, f{0};

  bool is_identity() {
    return a == 1.0f && b == 0.0f && c == 0.0f && d == 1.0f && e == 0.0f &&
           f == 0.0f;
  }
  void clear() {
    a = d = 1.0;
    c = e = b = f = 0.0;
  }
  void translate(const vfloat2_t &t) {
    /*
  /// [1 0 tx]   [a c e]    [a' c' e']
  /// [0 1 ty] * [b d f]  = [b' d' f']
  /// [0 0  1]   [0 0 1]    [0  0  1]

  // pre-mult
  e += t[0];
  f += t[1];
  */
    // post-mult
    /// [a c e]   [1 0 tx]   [a' c' e']
    /// [b d f] * [0 1 ty] = [b' d' f']
    /// [0 0 1]   [0 0  1]   [0  0  1]

    e += a * t[0] + c * t[1];
    f += b * t[0] + d * t[1];
  }
  void rotate(float angle) {
    const float cos_t = std::cos(angle);
    const float sin_t = std::sin(angle);

    /*
    // pre-mult
    ///  [cost -sint 0]   [a c e]   [a' c' e']
    ///  [sint cost  0] * [b d f] = [b' d' f']
    ///  [0     0    1]   [0 0 1]   [0  0   1]

    const float a0 = a * cos_t - b * sin_t;
    b = a * sin_t + b * cos_t;
    const float c0 = c * cos_t - d * sin_t;
    d = c * sin_t + d * cos_t;
    const float e0 = e * cos_t - f * sin_t;
    f = e * sin_t + f * cos_t;
    a = a0;
    c = c0;
    e = e0;
    */
    // post-mult
    /// [a c e]   [cost -sint 0]   [a' c' e']
    /// [b d f] * [sint cost  0] = [b' d' f']
    /// [0 0 1]   [0     0    1]   [0  0   1]

    const float a0 = a * cos_t + c * sin_t;
    c = -a * sin_t + c * cos_t;
    const float b0 = b * cos_t + d * sin_t;
    d = -b * sin_t + d * cos_t;
    a = a0;
    b = b0;
  }

  std::string to_string() {
    std::stringstream stream;
    stream << "transform=\"matrix(" << a << ' ' << b << ' ' << c << ' ' << d
           << ' ' << e << ' ' << f << ")\"";
    return stream.str();
  }
};

enum Align : unsigned int {
  // Horizontal align
  ALIGN_LEFT = 1u << 0u,   // Default, align text horizontally to left.
  ALIGN_CENTER = 1u << 1u, // Align text horizontally to center.
  ALIGN_RIGHT = 1u << 2u,  // Align text horizontally to right.
  // Vertical align
  ALIGN_TOP = 1u << 3u,      // Align text vertically to top.
  ALIGN_MIDDLE = 1u << 4u,   // Align text vertically to middle.
  ALIGN_BOTTOM = 1u << 5u,   // Align text vertically to bottom.
  ALIGN_BASELINE = 1u << 6u, // Default, align text vertically to baseline.
};

enum ArcDirection {
  CLOCKWISE = 1u << 0u,
  COUNTER_CLOCKWISE = 1u << 1u,
};

class FontManager {
public:
  std::vector<std::string> m_list_of_available_fonts;
  std::vector<std::string> m_font_dirs;
  FontManager();

  /// finds a font with name containing substring name1 (case sensitive), and
  /// optionally substring name2 (case insensitive) e.g.
  ///  find_font("Roboto","");
  ///  find_font("Roboto","bold");
  std::string find_font(const std::string &name1, const std::string &name2);

  void add_system_fonts();
  void add_font_dir(const std::string &path);
  void clear_font_dirs();

private:
  void list_fonts(const std::string &path);
};

} // namespace trase

#include "frontend/Drawable.hpp"

#endif // BACKEND_H_
