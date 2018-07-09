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

namespace trase {

template <typename T> void RawData::add_column(const std::vector<T> &new_col) {

  // if columns already exist then add the extra memory
  if (m_cols > 0) {

    // check number of rows in new column match
    if (static_cast<int>(new_col.size()) != m_rows) {
      throw Exception("columns in dataset must have identical number of rows");
    }

    // resize tmp vector
    m_tmp.resize(m_rows * (m_cols + 1));

    // copy orig data and new column to m_tmp
    for (int i = 0; i < m_rows; ++i) {
      for (int j = 0; j < m_cols; ++j) {
        m_tmp[i * (m_cols + 1) + j] = m_matrix[i * m_cols + j];
      }
      m_tmp[i * (m_cols + 1) + m_cols] = static_cast<float>(new_col[i]);
    }

    // swap data back to m_matrix
    m_matrix.swap(m_tmp);
  } else {
    // first column for matrix, set num rows and cols to match it
    m_rows = new_col.size();
    m_matrix.resize(m_rows);

    // copy data in (not using std::copy because visual studio complains if T
    // is not float)
    std::transform(new_col.begin(), new_col.end(), m_matrix.begin(),
                   [](auto i) { return static_cast<float>(i); });
  }
  ++m_cols;
}

template <typename Aesthetic>
ColumnIterator DataWithAesthetic::begin(const Aesthetic &a) {

  auto search = m_map.find(a.index);

  if (search == m_map.end()) {
    throw Exception(a.name + std::string(" aestheic not provided"));
  }
  return m_data->begin(search->second);
}

template <typename Aesthetic>
ColumnIterator DataWithAesthetic::end(const Aesthetic &a) {

  auto search = m_map.find(a.index);

  if (search == m_map.end()) {
    throw Exception(a.name + std::string(" aestheic not provided"));
  }
  return m_data->end(search->second);
}

template <typename Aesthetic, typename T>
void DataWithAesthetic::set(const Aesthetic &a, const std::vector<T> &data) {

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

template <typename T>
DataWithAesthetic &DataWithAesthetic::x(const std::vector<T> &data) {
  set(Aesthetic::x(), data);
  return *this;
}

template <typename T>
DataWithAesthetic &DataWithAesthetic::y(const std::vector<T> &data) {
  set(Aesthetic::y(), data);
  return *this;
}

template <typename T>
DataWithAesthetic &DataWithAesthetic::color(const std::vector<T> &data) {
  set(Aesthetic::color(), data);
  return *this;
}

template <typename T>
DataWithAesthetic &DataWithAesthetic::size(const std::vector<T> &data) {
  set(Aesthetic::size(), data);
  return *this;
}

} // namespace trase
