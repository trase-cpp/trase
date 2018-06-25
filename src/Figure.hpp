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

#ifndef FIGURE_H_
#define FIGURE_H_

// forward declare Figure so can be stored in Axis
namespace trase {
class Figure;
}

#include "Axis.hpp"
#include "Drawable.hpp"
#include <array>
#include <memory>

namespace trase {

class Figure : public Drawable {
  /// a unique id for this figure
  int m_id;

  /// the axis object for this figure
  std::vector<std::shared_ptr<Axis>> m_axes;

  /// total number of figures currentl created
  static int m_num_windows;

public:
  explicit Figure(const std::array<float, 2> &pixels);

  /// Create a new axis and return a shared pointer to it
  std::shared_ptr<Axis> axis() noexcept;

  /// Return a shared pointer to an existing axis.
  /// Throws std::out_of_range exception if out of range.
  /// \param n the axis to return
  std::shared_ptr<Axis> axis(int n);

  template <typename Backend> void serialise(Backend &backend);
  template <typename Backend> void show(Backend &backend);
  template <typename Backend> void draw(Backend &backend, float time);
};

inline std::shared_ptr<Figure> figure(std::array<float, 2> pixels = {
                                          {800.0, 600.0}}) {
  return std::make_shared<Figure>(pixels);
}

} // namespace trase

#endif // FIGURE_H_
