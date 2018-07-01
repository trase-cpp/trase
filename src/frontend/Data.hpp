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
#include "util/Exception.hpp"

namespace trase {

class ColumnIterator {
public:
  using pointer = float *;
  using iterator_category = std::random_access_iterator_tag;
  using reference = float &;
  using value_type = float;
  using difference_type = std::ptrdiff_t;

  ColumnIterator(const std::vector<float>::iterator &p, const int stride)
      : m_p(&(*p)), m_stride(stride) {}

  reference operator*() const { return dereference(); }

  reference operator->() const { return dereference(); }

  ColumnIterator &operator++() {
    increment();
    return *this;
  }

  const ColumnIterator operator++(int) {
    ColumnIterator tmp(*this);
    operator++();
    return tmp;
  }

  ColumnIterator operator+(int n) const {
    ColumnIterator tmp(*this);
    tmp.increment(n);
    return tmp;
  }

  reference operator[](const int i) const { return operator+(i).dereference(); }

  size_t operator-(const ColumnIterator &start) const {
    return (m_p - start.m_p) / m_stride;
  }

  inline bool operator==(const ColumnIterator &rhs) const { return equal(rhs); }

  inline bool operator!=(const ColumnIterator &rhs) const {
    return !operator==(rhs);
  }

private:
  bool equal(ColumnIterator const &other) const { return m_p == other.m_p; }

  reference dereference() const { return *m_p; }

  void increment() { m_p += m_stride; }

  void increment(const int n) { m_p += n * m_stride; }

  float *m_p;
  int m_stride;
};

class RawData {
  // raw data set, in row major order
  std::vector<float> m_matrix;
  std::vector<float> m_tmp;
  int m_rows{0};
  int m_cols{0};

public:
  int cols() { return m_cols; };
  int rows() { return m_rows; };

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
        m_tmp[i * m_cols + m_cols - 1] = new_col[i];
      }

      // swap data back to m_matrix
      m_matrix.swap(m_tmp);
    } else {
      // first column for matrix, set num rows and cols to match it
      m_rows = new_col.size();
      m_cols = 1;
      m_matrix.resize(m_rows * m_cols);

      // copy data in
      std::copy(new_col.begin(), new_col.end(), m_matrix.begin());
    }
  }

  ColumnIterator begin(const int i) { return {m_matrix.begin() + i, m_cols}; }
  ColumnIterator end(const int i) { return {m_matrix.end() + i, m_cols}; }
};

struct Aesthetic {
  // aesthetic indexes must be able to index a vector with size=SIZE
  static const int size = 3;
  struct x {
    static const int index = 0;
    static const char *name;
  };
  struct y {
    static const int index = 1;
    static const char *name;
  };
  struct color {
    static const int index = 2;
    static const char *name;
  };
};

using Limits = bbox<float, Aesthetic::size>;

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

  template <typename Aesthetic> ColumnIterator begin(const Aesthetic &a) {

    auto search = m_map.find(a.index);

    if (search == m_map.end()) {
      throw Exception(a.name + std::string(" aestheic not provided"));
    }
    return m_data->begin(search->second);
  }

  template <typename Aesthetic> ColumnIterator end(const Aesthetic &a) {

    auto search = m_map.find(a.index);

    if (search == m_map.end()) {
      throw Exception(a.name + std::string(" aestheic not provided"));
    }
    return m_data->end(search->second);
  }

  template <typename Aesthetic, typename T>
  void set(const Aesthetic &a, const std::vector<T> &data) {

    auto search = m_map.find(a.index);

    if (search == m_map.end()) {
      // if aesthetic is not in data then add a new column
      search = m_map.insert({a.index, m_data->cols()}).first;
      m_data->add_column(data);
    } else {
      // copy data to column
      std::copy(data.begin(), data.end(), m_data->begin(search->second));
    }

    // set m_limits with new data
    auto min_max = std::minmax_element(data.begin(), data.end());
    m_limits.bmin[a.index] = *min_max.first;
    m_limits.bmax[a.index] = *min_max.second;
  }

  int rows() const { return m_data->rows(); }

  const Limits &limits() { return m_limits; }
};

} // namespace trase

#endif // DATA_H_
