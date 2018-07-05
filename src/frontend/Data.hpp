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

#ifndef DATA_H_
#define DATA_H_

#include <cassert>
#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

#include "util/BBox.hpp"
#include "util/Colors.hpp"
#include "util/ColumnIterator.hpp"
#include "util/Exception.hpp"

namespace trase {

/// Raw data class, impliments a matrix with row major order
class RawData {
  // raw data set, in row major order
  std::vector<float> m_matrix;

  /// temporary data
  std::vector<float> m_tmp;

  int m_rows{0};
  int m_cols{0};

public:
  /// return the number of columns
  int cols() { return m_cols; };

  /// return the number of rows
  int rows() { return m_rows; };

  /// add a new column to the matrix. the data in `new_col` is copied into the
  /// new column
  template <typename T> void add_column(const std::vector<T> &new_col) {
    ++m_cols;

    // if columns already exist then add the extra memory
    if (m_cols > 1) {

      // check number of rows in new column match
      assert(static_cast<int>(new_col.size()) == m_rows);

      // resize tmp vector
      m_tmp.resize(m_rows * m_cols);

      // copy orig data and new column to m_tmp
      for (int i = 0; i < m_rows; ++i) {
        for (int j = 0; j < m_cols - 1; ++j) {
          m_tmp[i * m_cols + j] = m_matrix[i * (m_cols - 1) + j];
        }
        m_tmp[i * m_cols + m_cols - 1] = static_cast<float>(new_col[i]);
      }

      // swap data back to m_matrix
      m_matrix.swap(m_tmp);
    } else {
      // first column for matrix, set num rows and cols to match it
      m_rows = new_col.size();
      m_cols = 1;
      m_matrix.resize(m_rows * m_cols);

      // copy data in (not using std::copy because visual studio complains if T
      // is not float)
      std::transform(new_col.begin(), new_col.end(), m_matrix.begin(),
                     [](auto i) { return static_cast<float>(i); });
    }
  }

  /// return a ColumnIterator to the beginning of column i
  ColumnIterator begin(const int i) { return {m_matrix.begin() + i, m_cols}; }

  /// return a ColumnIterator to the end of column i
  ColumnIterator end(const int i) { return {m_matrix.end() + i, m_cols}; }
};

/// Aesthetics are a collection of tag classes that represent each aesthetic
/// Each aesthetic has a name, and an index from 0 -> size, where size is the
/// total number of aesthetics
///
/// Each Aesthetic defines a mapping to and from a display type
struct Aesthetic {
  // aesthetic indexes must be able to index a vector with size=N
  static const int N = 4;

  using Limits = bbox<float, N>;

  /// the data to display on the x-axis of the plot
  struct x {
    static const int index = 0;
    static const char *name;
    static float to_display(const float data, const Limits &data_lim,
                            const bfloat2_t &display_lim) {
      float len_ratio = (display_lim.bmax[0] - display_lim.bmin[0]) /
                        (data_lim.bmax[index] - data_lim.bmin[index]);

      float rel_pos = data - data_lim.bmin[index];
      return display_lim.bmin[0] + rel_pos * len_ratio;
    }
    static float from_display(const float display, const Limits &data_lim,
                              const bfloat2_t &display_lim) {
      float len_ratio = (data_lim.bmax[index] - data_lim.bmin[index]) /
                        (display_lim.bmax[1] - display_lim.bmin[1]);

      float rel_pos = display - display_lim.bmin[1];
      return data_lim.bmin[index] + rel_pos * len_ratio;
    }
  };

  /// the data to display on the y-axis of the plot
  struct y {
    static const int index = 1;
    static const char *name;
    static float to_display(const float data, const Limits &data_lim,
                            const bfloat2_t &display_lim) {
      float len_ratio = (display_lim.bmax[1] - display_lim.bmin[1]) /
                        (data_lim.bmax[index] - data_lim.bmin[index]);

      // Get the relative position and invert y by default (e.g. limits->pixels)
      float rel_pos = data_lim.bmax[index] - data;
      return display_lim.bmin[1] + rel_pos * len_ratio;
    }
    static float from_display(const float display, const Limits &data_lim,
                              const bfloat2_t &display_lim) {
      float len_ratio = (data_lim.bmax[index] - data_lim.bmin[index]) /
                        (display_lim.bmax[1] - display_lim.bmin[1]);

      float rel_pos = display_lim.bmax[1] - display;
      return data_lim.bmin[index] + rel_pos * len_ratio;
    }
  };

  /// the color of each plotting element, scaled from 0 -> 1
  struct color {
    static const int index = 2;
    static const char *name;

    static float to_display(const float data, const Limits &data_lim,
                            const bfloat2_t &display_lim) {
      (void)display_lim;
      float len_ratio = 1.f / (data_lim.bmax[index] - data_lim.bmin[index]);

      float rel_pos = data - data_lim.bmin[index];
      return rel_pos * len_ratio;
    }
    static float from_display(const float display, const Limits &data_lim,
                              const bfloat2_t &display_lim) {
      (void)display_lim;
      float len_ratio = (data_lim.bmax[index] - data_lim.bmin[index]);

      float rel_pos = display;
      return data_lim.bmin[index] + rel_pos * len_ratio;
    }
  };

  /// the size of each plotting element, scaled from 1 pixel to 1/20 size of
  /// y-axis
  struct size {
    static const int index = 3;
    static const char *name;

    static float to_display(const float data, const Limits &data_lim,
                            const bfloat2_t &display_lim) {
      float len_ratio = 0.05 * (display_lim.bmax[1] - display_lim.bmin[1]) /
                        (data_lim.bmax[index] - data_lim.bmin[index]);

      float rel_pos = data - data_lim.bmin[index];
      return 1.f + rel_pos * len_ratio;
    }
    static float from_display(const float display, const Limits &data_lim,
                              const bfloat2_t &display_lim) {
      float len_ratio = 20.f * (data_lim.bmax[index] - data_lim.bmin[index]) /
                        (display_lim.bmax[1] - display_lim.bmin[1]);

      float rel_pos = display - 1.f;
      return data_lim.bmin[index] + rel_pos * len_ratio;
    }
  };
};

/// Each aesthetic has a set of min/max limits, or scales, that are used for
/// plotting
using Limits = Aesthetic::Limits;

/// Combination of the RawData class and Aesthetics, this class points to a
/// RawData object, and contains a mapping from aesthetics to RawData column
/// numbers
class DataWithAesthetic {
  /// matrix of raw data
  std::shared_ptr<RawData> m_data;

  /// the aethetics define the mapping from x,y,color,.. to column indices
  std::unordered_map<int, int> m_map;

  /// the min/max limits of m_data for each aesthetics
  Limits m_limits;

public:
  DataWithAesthetic() : m_data(std::make_shared<RawData>()) {}
  explicit DataWithAesthetic(std::shared_ptr<RawData> data)
      : m_data(std::move(data)) {}

  /// return a ColumnIterator to the beginning of the data column for aesthetic
  /// a, throws if a has not yet been set
  template <typename Aesthetic> ColumnIterator begin(const Aesthetic &a) {

    auto search = m_map.find(a.index);

    if (search == m_map.end()) {
      throw Exception(a.name + std::string(" aestheic not provided"));
    }
    return m_data->begin(search->second);
  }

  /// return a ColumnIterator to the end of the data column for aesthetic a,
  /// throws if a has not yet been set
  template <typename Aesthetic> ColumnIterator end(const Aesthetic &a) {

    auto search = m_map.find(a.index);

    if (search == m_map.end()) {
      throw Exception(a.name + std::string(" aestheic not provided"));
    }
    return m_data->end(search->second);
  }

  /// if aesthetic a is not yet been set, this creates a new data column and
  /// copies in `data` (throws if data does not have the correct number of
  /// rows). If aesthetic a has been previously set, its data column is
  /// overwritten with `data`
  template <typename Aesthetic, typename T>
  void set(const Aesthetic &a, const std::vector<T> &data) {

    auto search = m_map.find(a.index);

    if (search == m_map.end()) {
      // if aesthetic is not in data then add a new column
      search = m_map.insert({a.index, m_data->cols()}).first;
      m_data->add_column(data);
    } else {
      // copy data to column (TODO: move this into RawData class)
      std::transform(data.begin(), data.end(), m_data->begin(search->second),
                     [](auto i) { return static_cast<float>(i); });
    }

    // set m_limits with new data
    auto min_max = std::minmax_element(data.begin(), data.end());
    m_limits.bmin[a.index] = static_cast<float>(*min_max.first);
    m_limits.bmax[a.index] = static_cast<float>(*min_max.second);
  }

  /// returns number of rows in the data let
  int rows() const { return m_data->rows(); }

  /// returns the min/max limits of the data
  const Limits &limits() { return m_limits; }

  template <typename T> DataWithAesthetic &x(const std::vector<T> &data) {
    set(Aesthetic::x(), data);
    return *this;
  }

  template <typename T> DataWithAesthetic &y(const std::vector<T> &data) {
    set(Aesthetic::y(), data);
    return *this;
  }

  template <typename T> DataWithAesthetic &color(const std::vector<T> &data) {
    set(Aesthetic::color(), data);
    return *this;
  }

  template <typename T> DataWithAesthetic &size(const std::vector<T> &data) {
    set(Aesthetic::size(), data);
    return *this;
  }
};

} // namespace trase

#endif // DATA_H_
