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

#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

#include "util/BBox.hpp"
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

  void add_column() {
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
    static const int index = 0;
    const char *name = "x";
  };
  struct y {
    static const int index = 1;
    const char *name = "y";
  };
  struct color {
    static const int index = 2;
    const char *name = "color";
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

  template <typename Aesthetic> Column &get(const Aesthetic &unused) {
    (void)unused; // supress compiler warning

    auto search = m_map.find(Aesthetic::index);

    if (search == m_map.end()) {
      throw Exception(Aesthetic::name + std::string(" aestheic not provided"));
    }
    return (*m_data)[search->second];
  }

  template <typename Aesthetic>
  Column &set(const Aesthetic &unused, const Column &data) {
    (void)unused; // supress compiler warning

    auto search = m_map.find(Aesthetic::index);

    if (search == m_map.end()) {
      // if aesthetic is not in data then add a new column
      search = m_map.insert({Aesthetic::index, m_data->cols()}).first;
      m_data->add_column();
    }

    // copy data to column
    std::copy(data.begin(), data.end(), (*m_data)[search->second].begin());

    // set m_limits with new data
    auto min_max = std::minmax_element(data.begin(), data.end());
    m_limits.bmin[Aesthetic::index] = *min_max.first;
    m_limits.bmax[Aesthetic::index] = *min_max.second;

    return (*m_data)[search->second];
  }

  const Limits &limits() { return m_limits; }
};

} // namespace trase

#endif // DATA_H_
