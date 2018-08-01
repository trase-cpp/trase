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

/// \file Colors.hpp

#ifndef COLORS_H_
#define COLORS_H_

#include <array>
#include <string>
#include <vector>

#include "util/Vector.hpp"

namespace trase {

/// Colour class with red, green, blue and alpha in range 0 to 255
class RGBA {

  /// red value
  int m_r;

  /// green value
  int m_g;

  /// blue value
  int m_b;

  /// alpha value
  int m_a;

public:
  const static int default_alpha;
  const static std::array<const RGBA, 10> defaults;
  const static RGBA black;
  const static RGBA white;

  /// default constructor
  RGBA() = default;

  /// constructor taking red, green, blue and alpha
  RGBA(int r, int g, int b, int a = default_alpha) noexcept;

  /// constructor taking 4 float vector
  explicit RGBA(const Vector<float, 4> &v) noexcept;

  /// constructor taking 3 float vector
  explicit RGBA(const Vector<float, 3> &v) noexcept;

  /// convert to an float vector
  explicit operator Vector<float, 4>() const noexcept {
    return {static_cast<float>(m_r), static_cast<float>(m_g),
            static_cast<float>(m_b), static_cast<float>(m_a)};
  };

  /// convert to an rgb string of form #rrggbb
  std::string to_rgb_string() const noexcept;

  /// Get the current red value
  int r() const noexcept;

  /// Get the current green value
  int g() const noexcept;

  /// Get the current blue value
  int b() const noexcept;

  /// Get the current alpha value
  int a() const noexcept;

  /// Set a new red value
  RGBA &r(int r) noexcept;

  /// Set a new green value
  RGBA &g(int g) noexcept;

  /// Set a new blue value
  RGBA &b(int b) noexcept;

  /// Set a new alpha value
  RGBA &a(int a) noexcept;

  /// Equality comparison
  bool operator==(const RGBA &b) const noexcept;
  bool operator!=(const RGBA &b) const noexcept;
};

/// a linear segmented colormap
class Colormap {
  std::vector<Vector<float, 3>> m_colors;

public:
  /// constructs the colormap from a list of rgb values scaled from 0-1
  explicit Colormap(std::vector<Vector<float, 3>> list) noexcept;

  /// maps a float from 0->1 to a RGBA color
  RGBA to_color(float i) const;
};

struct Colormaps {
  static const Colormap viridis;
};

} // namespace trase

#endif // COLORS_H_
