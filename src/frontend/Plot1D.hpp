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

/// \file Plot1D.hpp

#ifndef PLOT1D_H_
#define PLOT1D_H_

// forward declare Axis so it can be stored in plot1d
namespace trase {
class Axis;
} // namespace trase

#include <memory>
#include <vector>

#include "frontend/Data.hpp"
#include "frontend/Drawable.hpp"
#include "frontend/Transform.hpp"
#include "util/BBox.hpp"
#include "util/Colors.hpp"
#include "util/Exception.hpp"

namespace trase {

class Plot1D : public Drawable {
protected:
  /// dataset
  std::vector<DataWithAesthetic> m_data;

  /// label
  std::string m_label;

  /// colormap
  const Colormap *m_colormap;

  float m_line_width;

  RGBA m_color;

  /// min/max limits of m_data across all frames
  Limits m_limits;

  /// parent axis
  Axis &m_axis;

  /// transform
  Transform m_transform;

public:
  explicit Plot1D(Axis &axis);

  // make it polymorphic
  virtual ~Plot1D() = default;

  template <typename T1, typename T2>
  void add_frame(const std::vector<T1> &x, const std::vector<T2> &y,
                 const float time) {
    if (x.size() != y.size()) {
      throw Exception("x and y vector sizes do not match");
    }
    return add_frame(DataWithAesthetic().x(x).y(y), time);
  }

  /// Adds a new data frame to this plot
  ///
  /// The limits of the new data frame will be added to the limits of this
  /// plot, and the parent axis
  ///
  /// \param data the new data frame
  /// \param time the timestamp for this frame. This must be greater than the
  /// time for all previously added frames
  void add_frame(const DataWithAesthetic &data, float time);

  float get_time(const int i) const { return m_times[i]; }

  const DataWithAesthetic &get_data(const int i) const { return m_data[i]; }
  DataWithAesthetic &get_data(const int i) { return m_data[i]; }
  size_t data_size() const { return m_data.size(); }

  /// Sets the transform
  ///
  /// \param transform the new transform
  ///
  /// All new data frames added to the plot will have this transform applied
  /// before the data is stored internally
  void set_transform(const Transform &transform) { m_transform = transform; }

  void set_color(const RGBA &color) { m_color = color; }

  /// Set the label
  ///
  /// This label describes the plot and is shown on the axis legend
  ///
  /// \param label a string description of the plot
  void set_label(const std::string &label) { m_label = label; }

  const std::string &get_label() const { return m_label; }
  const RGBA &get_color() const { return m_color; }
  const float get_line_width() const { return m_line_width; }
  const Colormap &get_colormap() const { return *m_colormap; }

  template <typename Backend> void serialise(Backend &backend);
  template <typename Backend> void draw(Backend &backend, float time);
};

} // namespace trase

#endif // PLOT1D_H_
