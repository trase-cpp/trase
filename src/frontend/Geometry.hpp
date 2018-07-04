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

#ifndef GEOMETRY_H_
#define GEOMETRY_H_

#include "frontend/Axis.hpp"
#include "frontend/Plot1D.hpp"

namespace trase {

// available geometry types
enum class Geometry { line, point };

class Points {
  template <typename Backend>
  static void serialise(Backend &backend, const Axis &axis, const Plot1D &plot);
  template <typename Backend>
  static void draw(Backend &backend, const Axis &axis, const Plot1D &plot);

private:
  template <typename Backend>
  static void serialise_frames(Backend &backend, const Axis &axis,
                               const Plot1D &plot);
  template <typename Backend>
  static void draw_plot(Backend &backend, const Axis &axis, const Plot1D &plot);
};

class Line {
  template <typename Backend>
  static void serialise(Backend &backend, const Axis &axis, const Plot1D &plot);
  template <typename Backend>
  static void draw(Backend &backend, const Axis &axis, const Plot1D &plot);

private:
  template <typename Backend>
  static void serialise_frames(Backend &backend, const Axis &axis,
                               const Plot1D &plot);
  template <typename Backend>
  static void serialise_highlights(Backend &backend, const Axis &axis,
                                   const Plot1D &plot);
  template <typename Backend>
  static void draw_plot(Backend &backend, const Axis &axis, const Plot1D &plot);
  template <typename Backend>
  static void draw_highlights(Backend &backend, const Axis &axis,
                              const Plot1D &plot);
};

template <typename Backend>
void serialise_geometry(const Geometry &geom, Backend &backend,
                        const Axis &axis, const Plot1D &plot) {
  switch (geom) {
  case Geometry::line:
    Line::serialise(backend, axis, plot);
    break;
  case Geometry::point:
    Points::serialise(backend, axis, plot);
    break;
  }
}

template <typename Backend>
void draw_geometry(const Geometry &geom, Backend &backend, const Axis &axis,
                   const Plot1D &plot) {
  switch (geom) {
  case Geometry::line:
    Line::draw(backend, axis, plot);
    break;
  case Geometry::point:
    Points::draw(backend, axis, plot);
    break;
  }
}

} // namespace trase

#endif // GEOMETRY_H_
