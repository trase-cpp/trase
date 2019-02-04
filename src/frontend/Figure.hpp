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

/// \file Figure.hpp

#ifndef FIGURE_H_
#define FIGURE_H_

#include <array>
#include <memory>
#include <unordered_map>

#include "frontend/Axis.hpp"
#include "frontend/Drawable.hpp"

namespace trase {

/// The primary Drawable for each figure
///
/// Each Figure points to one or more Axis objects that are drawn within the
/// Figure.
class Figure : public Drawable {
  /// a unique id for this figure
  int m_id;

  /// number of axis subplots in each direction
  vint2_t m_axis_layout;

  /// axis subplots
  std::unordered_map<vint2_t, std::shared_ptr<Axis>, vint2_hash, vint2_equal>
      m_axis_subplots;

  /// total number of figures currentl created
  static int m_num_windows;

public:
  /// create a new figure with the given number of pixels
  ///
  /// \param pixels the number of pixels along the {width, height} of the figure
  explicit Figure(const std::array<float, 2> &pixels);

  virtual ~Figure() = default;

  TRASE_DISPATCH_BACKENDS

  /// Create a new axis at position (0,0) and return a shared pointer to it
  /// If the axis already exists returns a pointer to the existing axis
  /// \return a shared pointer to the new axis
  std::shared_ptr<Axis> axis();

  /// Create a new axis at position (\p i, \p j) and return a shared pointer to
  /// it If the axis already exists returns a pointer to the existing axis
  /// \return a shared pointer to the new axis
  std::shared_ptr<Axis> axis(int i, int j);

  /// Draw the Figure using the Backend provided
  ///
  /// This function takes control of the render loop and animates the
  /// Figure until the window is closed.
  ///
  /// TODO: issue #86 - only works for the BackendGL backend
  ///
  /// \param backend the Backend used to draw the figure.
  template <typename Backend> void show(Backend &backend);

  /// Draw the Figure using the AnimatedBackend provided
  ///
  /// \param backend the AnimatedBackend used to draw the figure.
  template <typename AnimatedBackend> void draw(AnimatedBackend &backend);

  /// Draw the Figure at a given time using the Backend provided
  ///
  /// \param backend the Backend used to draw the figure.
  /// \param time the Figure is drawn at this time
  template <typename Backend> void draw(Backend &backend, float time);

private:
  /// creates a new Axis at position \p new_position of the subplot array, or
  /// returns existing Axis if one exists at this position. If \p new_position
  /// is larger than the maximum extent of the current subplot array (see
  /// m_axis_layout), then the subplot array is extended to cover the new
  /// position, and all existing Axis children are resized accordingly
  std::shared_ptr<Axis> update_layout(const vint2_t &new_position);

  template <typename Backend> void render_interactive_frame(Backend &backend);
#ifdef __EMSCRIPTEN__
  template <typename Backend> static void emscripten_callback(void *data);
#endif
};

/// create a new Figure
///
/// \param pixels (optional) the number of pixels along the {width, height} of
/// the figure
inline std::shared_ptr<Figure> figure(std::array<float, 2> pixels = {
                                          {800.0, 600.0}}) {
  return std::make_shared<Figure>(pixels);
}

} // namespace trase

#include "frontend/Figure.tcc"

#endif // FIGURE_H_
