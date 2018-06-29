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
#include <functional>
#include <memory>
#include <vector>

#include "frontend/Axis.hpp"
#include "frontend/Drawable.hpp"
#include "util/Colors.hpp"
#include "util/Exception.hpp"

namespace trase {

using Column = std::vector<float>;

class RawData {
  // raw data set, can have any number of columns
  std::vector<Column> m_matrix;

public:
  int cols() { return m_matrix.size(); };

  // assume that all columns are the same size
  int rows() { return m_matrix[0].size(); };

  template <typename T> void add_column(const std::vector<T> &data) {
    m_matrix.emplace_back();

    // if columns already exist then match the sizes
    if (cols() > 1) {
      m_matrix.back().resize(rows());
    }
  }

  Column &operator[](const int i) { return m_matrix[i]; }
  const Column &operator[](const int i) const { return m_matrix[i]; }
  Column &at(const int i) { return m_matrix.at(i); }
  const Column &at(const int i) const { return m_matrix.at(i); }
};

struct Aesthetic {
  // aesthetic indexes must be able to index a vector with size=SIZE
  static const int size = 3;
  struct x {
    const int index = 0;
    const char *name = "x";
  };
  struct y {
    const int index = 1;
    const char *name = "y";
  };
  struct color {
    const int index = 2;
    const char *name = "color";
  };
};

enum Geometry { Point, Line };

class DataWithAesthetic {
  /// matrix of raw data
  std::shared_ptr<RawData> m_data;

  /// the aethetics define the mapping from x,y,color,.. to column indices
  std::array<int, Aesthetic::size> m_aesthetics;

public:
  DataWithAesthetic() : m_data(std::make_shared<RawData>()) {
    std::fill(m_aesthetics.begin(), m_aesthetics.end(), -1);
  }
  DataWithAesthetic(std::shared_ptr<RawData> data) : m_data(data) {
    std::fill(m_aesthetics.begin(), m_aesthetics.end(), -1);
  }

  template <typename Aesthetic> Column &operator[](const Aesthetic) {
    const int index = m_aesthetics[Aesthetic::index];
    if (index < 0) {
      throw Exception(Aesthetic::name + std::string(" aestheic not provided"));
    }
    return (*m_data)[index];
  };

  template <typename Aesthetic>
  Column &set_aesthetic(const Aesthetic, const Column &data) {
    int index = m_aesthetics[Aesthetic::index];

    // if aesthetic is not in data then add a new column
    if (index < 0) {
      m_aesthetics[Aesthetic::index] = m_data->cols();
      index = m_aesthetics[Aesthetic::index];
      m_data->add_column();
    }

    // copy new column to data matrix
    std::copy(data.begin(), data.end(), m_data->at(index).begin());
    return (*m_data)[index];
  };
};

class Transform {
  std::function<DataWithAesthetic(const DataWithAesthetic &)> m_transform;

public:
  template <typename T>
  Transform(const T &transform) : m_transform(transform) {}
  DataWithAesthetic operator()(const DataWithAesthetic &data) {
    return m_transform(data);
  }
};

/// Identity transform, just pass through...
DataWithAesthetic identity(const DataWithAesthetic &data) { return data; }

class Plot1D : public Drawable {
  /// dataset
  std::vector<std::shared_ptr<DataWithAesthetic>> m_data;

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
    auto data = std::make_shared<DataWithAesthetic>();
    data->set_aesthetic(Aesthetic::x(), x);
    data->set_aesthetic(Aesthetic::y(), x);
    return add_values(data, time);
  }

  void add_values(std::shared_ptr<DataWithAesthetic> values, float time);

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
