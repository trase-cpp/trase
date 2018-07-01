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

#include <memory>
#include <vector>

#include "frontend/Axis.hpp"
#include "frontend/Data.hpp"
#include "frontend/Drawable.hpp"
#include "frontend/Transform.hpp"
#include "util/BBox.hpp"
#include "util/Colors.hpp"
#include "util/Exception.hpp"

namespace trase {

class Plot1D : public Drawable {

  // available geometry types
  enum Geometry { Point, Line };

  /// dataset
  std::vector<std::shared_ptr<DataWithAesthetic>> m_data;

  /// label
  std::string m_label;

  RGBA m_color;

  /// min/max limits of m_data across all frames
  Limits m_limits;

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
    auto data = std::make_shared<DataWithAesthetic>();
    data->set(Aesthetic::x(), x);
    data->set(Aesthetic::y(), y);
    return add_frame(data, time);
  }

  void add_frame(const std::shared_ptr<DataWithAesthetic> &data, float time);

  const std::shared_ptr<DataWithAesthetic> &get_data(const int i) const {
    return m_data[i];
  }
  std::shared_ptr<DataWithAesthetic> &get_data(const int i) {
    return m_data[i];
  }

  void set_color(const RGBA &color) { m_color = color; }
  void set_label(const std::string &label) { m_label = label; }
  const std::string &get_label() const { return m_label; }
  const RGBA &get_color() const { return m_color; }

  template <typename Backend> void serialise(Backend &backend);
  template <typename Backend> void draw(Backend &backend, float time);
};

} // namespace trase

#endif // PLOT1D_H_
