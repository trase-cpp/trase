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

/// \file Drawable.hpp

#ifndef DRAWABLE_H_
#define DRAWABLE_H_

#include <array>
#include <ostream>
#include <vector>

#include "backend/Backend.hpp"
#include "util/BBox.hpp"

namespace trase {

/// A helper struct for Drawable that holds frame-related information
struct FrameInfo {

  int frame_above;
  float frame;
  float w1;
  float w2;

  void update(std::vector<float> &times, const float time_now) {
    frame_above = static_cast<int>(std::distance(
        times.begin(), std::lower_bound(times.begin(), times.end(), time_now)));

    if (frame_above == 0) {
      w1 = 1.f;
      w2 = 0.f;
      frame = 0.f;
    } else {
      const float delta_t = times[frame_above] - times[frame_above - 1];
      w1 = (time_now - times[frame_above - 1]) / delta_t;
      w2 = 1.f - w1;
      frame = static_cast<float>(frame_above - 1) + w1;
    }
  }
};

/// Base class for drawable objects in a figure
///
/// A figure consists of a tree structure of Drawable objects, with the
/// Figure object at the top of the tree. Each Drawable in the tree has a parent
/// Drawable and zero or more child Drawables.
///
/// Each Drawable is given a size as a ratio of its parents size, so that
/// resizing of the entire tree can be done easily.
///
/// Each Drawable also has a set of times at which animation frames are defined
/// and, given a time, can calculate where this time sits in the list of frames
///
/// Finally, each Drawable has functions for drawing itself, given a Backend as
/// a templated argument. These are overridden in derived classes (but can't be
/// virtual as they are templated on the Backend type). Instead, each draw
/// function is implemented in a header file ending in *Draw.hpp (e.g.
/// AxisDraw.hpp), and included when compiling each Backend
///
class Drawable {
protected:
  /// a list of Drawables that are children of this object
  std::vector<std::shared_ptr<Drawable>> m_children;

  /// parent of this object
  Drawable *m_parent;

  /// the area of this object as a ratio of its parent object
  bfloat2_t m_area;

  /// the area of this object in raw pixels (updated by resize())
  bfloat2_t m_pixels;

  /// the animation time span (see update_time_span())
  float m_time_span;

  /// the animation frame times (see add_frame_time())
  std::vector<float> m_times;

  /// stores information on the current draw time (see update_frame_info())
  FrameInfo m_frame_info;

public:
  /// constructs a Drawable under \p parent in the tree structure, and assigns
  /// it an drawable area given by \p area_of_parent
  ///
  /// \param parent the parent \Drawable object
  /// \param area_of_parent the drawable area assigned to this Drawable as a
  /// ratio of the parent size
  Drawable(Drawable &parent, const bfloat2_t &area_of_parent);

  /// resize the drawable area (in raw pixels) using the parents area (in raw
  /// pixels)
  void resize(const bfloat2_t &parent_pixels);

  /// if \p time is outside the current animation time span of this object then
  /// this time span is increased to include it
  void update_time_span(float time);

  /// adds a keyframe time to the animation
  void add_frame_time(float time);

  /// fills out m_frame_info using a given \p time
  ///
  /// \see get_frame_info()
  void update_frame_info(float time);

  /// returns the current FrameInfo
  ///
  /// \see update_frame_info()
  const FrameInfo &get_frame_info() const;

  /// returns this objects drawable area in raw pixels
  const bfloat2_t &pixels() { return m_pixels; }

  /// returns this objects drawable area as a ratio of the parents drawable area
  const bfloat2_t &area() { return m_pixels; }

  /// accept a visitor backend
  virtual void accept(Backend &backend) = 0;

  /// draw this object using the given AnimatedBackend
  template <typename AnimatedBackend> void draw(AnimatedBackend &backend);

  /// draw this object using the given Backend
  template <typename Backend> void draw(Backend &backend, float time);
};

#define DEFINE_VISITABLE()                                                     \
  void accept(Backend &backend) override { backend.dispatch(*this); }

} // namespace trase

#endif // DRAWABLE_H_
