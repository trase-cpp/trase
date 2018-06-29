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

#ifndef PLOT1D_H_
#define PLOT1D_H_

// forward declare Plot1D so can be used in Axis
namespace trase {
class Plot1D;
}

#include <array>

#include "frontend/Axis.hpp"
#include "frontend/Drawable.hpp"
#include "util/Colors.hpp"
#include "util/Exception.hpp"

namespace trase {

class Plot1D : public Drawable {
  /// values
  std::vector<std::vector<vfloat2_t>> m_values;

  /// label
  std::string m_label;

  /// [xmin, ymin, xmax, ymax]
  bfloat2_t m_limits;

  RGBA m_color;

  /// parent axis
  Axis &m_axis;

public:
  explicit Plot1D(Axis &axis);

  template <typename T1, typename T2>
  void add_frame(const std::vector<T1> &x, const std::vector<T2> &y,
                 const float time) {
    if (x.size() != y.size()) {
      throw Exception("x and y vector sizes do not match");
    }
    std::vector<vfloat2_t> values(x.size());
    for (size_t i = 0; i < x.size(); ++i) {
      values[i][0] = x[i];
      values[i][1] = y[i];
    }
    return add_values(std::move(values), time);
  }

  void add_values(std::vector<vfloat2_t> &&values, float time);

  const std::vector<vfloat2_t> &get_values(const int i) const {
    return m_values[i];
  }
  std::vector<vfloat2_t> &get_values(const int i) { return m_values[i]; }

  void set_color(const RGBA &color) { m_color = color; }
  void set_label(const std::string &label) { m_label = label; }
  const std::string &get_label() const { return m_label; }
  const RGBA &get_color() const { return m_color; }

  template <typename Backend> void serialise(Backend &backend);
  template <typename Backend> void draw(Backend &backend, float time);

}; // namespace trase

} // namespace trase

#endif // PLOT1D_H_
