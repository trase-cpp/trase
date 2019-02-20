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

/// \file Rectangle.hpp

#ifndef RECTANGLE_H_
#define RECTANGLE_H_

#include "frontend/Geometry.hpp"

namespace trase {

/// A collection of one or more filled rectangles 
///
/// Aesthetics:
///   - xmin (minimum x-coordinate of rectangles)
///   - ymin (minimum y-coordinate of rectangles)
///   - xmax (maximum x-coordinate of rectangles)
///   - ymax (maximum y-coordinate of rectangles)
///   - color (optional - stroke color of circles)
///   - fill (optional - fill color of circles)
///
/// Default Transform:
///   - Identity 
class Rectangle : public Geometry {
public:
  /// create a new Rectangle, connecting it to the @p parent
  explicit Rectangle(Axis *parent) : Geometry(parent) {}

  virtual ~Rectangle() = default;

  TRASE_GEOMETRY_DISPATCH_BACKENDS

  /// draw the full rectangles animation using the AnimatedBackend
  ///
  /// @param backend the AnimatedBackend to use when drawing
  template <typename AnimatedBackend> void draw(AnimatedBackend &backend);

  /// draw the rectangles at a snapshot in time using the Backend
  ///
  /// @param backend the Backend to use when drawing
  /// @param time draw the rectangles at this time
  template <typename Backend> void draw(Backend &backend, float time);

  /// draw the full rectangle legend animation
  ///
  /// @param backend the AnimatedBackend to use when drawing
  template <typename AnimatedBackend>
  void draw_legend(AnimatedBackend &backend, const bfloat2_t &box);

  /// draw the rectangles legend at a snapshot in time
  ///
  /// @param backend the Backend to use when drawing
  /// @param time draw the legend at this time
  template <typename Backend>
  void draw_legend(Backend &backend, float time, const bfloat2_t &box);

private:
  void validate_frames(const bool have_color, const bool have_fill,
                       const int n);
  template <typename AnimatedBackend>
  void draw_frames(AnimatedBackend &backend);
  template <typename Backend> void draw_plot(Backend &backend);
};

} // namespace trase

#include "frontend/Rectangle.tcc"

#endif // RECTANGLE_H_
