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

/// \file Line.hpp

#ifndef LINE_H_
#define LINE_H_

#include "frontend/Geometry.hpp"

namespace trase {

/// A single line made up of one or more points connected by straight lines
///
/// Aesthetics:
///   - x (x-coordinates of the line points)
///   - y (y-coordinates of the line points)
///
/// Default Transform:
///   - Identity 
class Line : public Geometry {
public:
  /// create a new Line, connecting it to the @p parent
  explicit Line(Axis *parent) : Geometry(parent) {}
  virtual ~Line() = default;

  TRASE_GEOMETRY_DISPATCH_BACKENDS

  /// draw the full line animation using the AnimatedBackend
  ///
  /// @param backend the AnimatedBackend to use when drawing
  template <typename AnimatedBackend> void draw(AnimatedBackend &backend);

  /// draw the line at a snapshot in time using the Backend
  ///
  /// @param backend the Backend to use when drawing
  /// @param time draw the line at this time
  template <typename Backend> void draw(Backend &backend, float time);

  /// draw the full line legend animation
  ///
  /// @param backend the AnimatedBackend to use when drawing
  template <typename AnimatedBackend>
  void draw_legend(AnimatedBackend &backend, const bfloat2_t &box);

  /// draw the line legend at a snapshot in time
  ///
  /// @param backend the Backend to use when drawing
  /// @param time draw the legend at this time
  template <typename Backend>
  void draw_legend(Backend &backend, float time, const bfloat2_t &box);

private:
  template <typename AnimatedBackend>
  void draw_frames(AnimatedBackend &backend);
  template <typename AnimatedBackend>
  void draw_anim_highlights(AnimatedBackend &backend);
  template <typename Backend> void draw_plot(Backend &backend);
  template <typename Backend> void draw_highlights(Backend &backend);
};

} // namespace trase

#include "frontend/Line.tcc"

#endif // LINE_H_
