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

#include "Data.hpp"

namespace trase {

// helper function to cast type T to a float
//
// by default, will use static_cast to cast T to float
// if T is a std::string, then:
//   if string_data is not empty: returns a float equal to the index of arg in
//   the set if string_data is empty: returns stof(arg)
template <typename T>
float cast_to_float(const T &arg, const std::set<std::string> &string_data) {
  return static_cast<float>(arg);
}

template <>
float cast_to_float<std::string>(const std::string &arg,
                                 const std::set<std::string> &string_data);

// helper function to parse a column of data given by the two iterators
// new_col_begin and new_col_end
//
// if the value_type of the data is a std::string, and the first element cannot
// be converted to a float by stof, then the strings in the data column are
// copied to the set string_data
template <typename T>
void store_non_numeric_strings(T new_col_begin, T new_col_end,
                               std::set<std::string> &string_data);

// the function above uses tag dispatching based on the value_type of the data
// column. This function is chosen if the value_type is a std::string
template <typename T>
void store_non_numeric_strings(T new_col_begin, T new_col_end,
                               std::set<std::string> &string_data,
                               std::true_type) {

  if (new_col_begin == new_col_end)
    return;

  size_t pos;
  bool failed = false;
  try {
    std::stof(*new_col_begin, &pos);
  } catch (std::invalid_argument) {
    failed = true;
  }

  // if exception caught or if not all characters convered, then the first
  // element is a non-numeric string.  if the first element is a non-numeric
  // string, assume the rest are and process them into string_data
  if (failed || pos < new_col_begin->size()) {
    std::copy(new_col_begin, new_col_end,
              std::inserter(string_data, string_data.begin()));
  }
}

// the function above uses tag dispatching based on the value_type of the data
// column. This function is chosen if the value_type is not a std::string
template <typename T>
void store_non_numeric_strings(T new_col_begin, T new_col_end,
                               std::set<std::string> &string_data,
                               std::false_type) {}

// see declaration above
template <typename T>
void store_non_numeric_strings(T new_col_begin, T new_col_end,
                               std::set<std::string> &string_data) {
  store_non_numeric_strings(
      new_col_begin, new_col_end, string_data,
      std::is_same<typename std::iterator_traits<T>::value_type,
                   std::string>());
}

template <typename T> void RawData::add_column(T new_col_begin, T new_col_end) {
  const size_t n = std::distance(new_col_begin, new_col_end);

  // check number of rows in new column match
  if (m_cols > 0 && static_cast<int>(n) != m_rows) {
    throw Exception("columns in dataset must have identical number of rows");
  }

  m_string_data.emplace_back();
  store_non_numeric_strings(new_col_begin, new_col_end, m_string_data.back());

  // if columns already exist then add the extra memory
  if (m_cols > 0) {

    // resize tmp vector
    m_tmp.resize(m_rows * (m_cols + 1));

    // copy orig data and new column to m_tmp
    for (int i = 0; i < m_rows; ++i) {
      for (int j = 0; j < m_cols; ++j) {
        m_tmp[i * (m_cols + 1) + j] = m_matrix[i * m_cols + j];
      }
      m_tmp[i * (m_cols + 1) + m_cols] =
          cast_to_float(*new_col_begin++, m_string_data.back());
    }

    // swap data back to m_matrix
    m_matrix.swap(m_tmp);
  } else {
    // first column for matrix, set num rows and cols to match it
    m_rows = static_cast<int>(n);
    m_matrix.resize(m_rows);

    // copy data in (not using std::copy because visual studio complains if T
    // is not float)
    std::transform(
        new_col_begin, new_col_end, m_matrix.begin(),
        [this](auto i) { return cast_to_float(i, m_string_data.back()); });
  }
  ++m_cols;
}

template <typename T> void RawData::add_row(T new_row_begin, T new_row_end) {
  const size_t n = std::distance(new_row_begin, new_row_end);
  // check number of cols in new row match
  if (m_rows > 0 && static_cast<int>(n) != m_cols) {
    throw Exception("rows in dataset must have identical number of columns");
  }
  m_cols = static_cast<int>(n);
  ++m_rows;
  const size_t oldn = m_matrix.size();
  m_matrix.resize(oldn + n);
  // copy data in (not using std::copy because visual studio complains if T
  // is not float)
  std::transform(new_row_begin, new_row_end, m_matrix.begin() + oldn,
                 [this](auto i) { return static_cast<float>(i); });
}

template <typename T> void RawData::add_column(const std::vector<T> &new_col) {
  add_column(new_col.begin(), new_col.end());
}

template <typename T> void RawData::add_row(const std::vector<T> &new_row) {
  add_row(new_row.begin(), new_row.end());
}

template <typename T>
void RawData::set_column(const int i, const std::vector<T> &new_col) {

  // check column exists
  if (i < 0 || i >= cols()) {
    throw std::out_of_range("column index out of range");
  }

  // check number of rows in new column match
  if (static_cast<int>(new_col.size()) != m_rows) {
    throw Exception("columns in dataset must have identical number of rows");
  }

  m_string_data[i].clear();
  store_non_numeric_strings(new_col.begin(), new_col.end(), m_string_data[i]);

  // copy column
  for (int j = 0; j < m_rows; ++j) {
    m_matrix[j * m_cols + i] = cast_to_float(new_col[j], m_string_data[i]);
  }
}

template <typename T>
std::map<T, std::shared_ptr<RawData>>
RawData::facet(const std::vector<T> &data) const {
  // check number of rows in new column match
  if (m_cols > 0 && static_cast<int>(data.size()) != m_rows) {
    throw Exception(
        "facet column must have an identical number of rows to the dataset");
  }

  std::map<T, std::shared_ptr<RawData>> fdata;

  std::vector<std::size_t> row_indices(rows());
  std::iota(row_indices.begin(), row_indices.end(), 0);
  std::stable_sort(
      row_indices.begin(), row_indices.end(),
      [&](std::size_t i, std::size_t j) { return data[i] < data[j]; });

  auto i = row_indices.begin();
  for (auto j = i; i != row_indices.end(); i = j) {
    while (j != row_indices.end() && data[*j] <= data[*i]) {
      ++j;
    }

    const auto ji_diff = static_cast<size_t>(j - i);
    std::vector<float> tmp(ji_diff);
    auto &facet = fdata[data[*i]];
    facet = std::make_shared<RawData>();
    for (int k = 0; k < cols(); ++k) {
      std::transform(i, j, tmp.begin(), [row = begin(k)](size_t i) {
        return row[static_cast<int>(i)];
      });
      facet->add_column(tmp);
    }
  }
  return fdata;
}

template <typename T1, typename T2>
std::map<std::pair<T1, T2>, std::shared_ptr<RawData>>
RawData::facet(const std::vector<T1> &data1,
               const std::vector<T2> &data2) const {
  // check number of rows in new column match
  if (m_cols > 0 && static_cast<int>(data1.size()) != m_rows) {
    throw Exception(
        "facet column 1 must have an identical number of rows to the dataset");
  }
  // check number of rows in new column match
  if (m_cols > 0 && static_cast<int>(data2.size()) != m_rows) {
    throw Exception(
        "facet column 2 must have an identical number of rows to the dataset");
  }

  std::map<std::pair<T1, T2>, std::shared_ptr<RawData>> fdata;

  // insert all rows into map, with key given by pair(data1,data2)
  for (size_t i = 0; i < data1.size(); ++i) {
    auto &facet = fdata[std::make_pair(data1[i], data2[i])];
    if (!facet) {
      facet = std::make_shared<RawData>();
    }
    std::vector<float> row(cols());
    for (int k = 0; k < cols(); ++k) {
      row[k] = begin(k)[static_cast<int>(i)];
    }
    facet->add_row(row);
  }

  return fdata;
}

template <typename T>
std::map<T, DataWithAesthetic>
DataWithAesthetic::facet(const std::vector<T> &data) const {
  std::map<T, DataWithAesthetic> faceted_data;

  for (auto raw_data : m_data->facet(data)) {
    faceted_data[raw_data.first] =
        DataWithAesthetic(raw_data.second, m_map, m_limits);
  }

  return faceted_data;
}

template <typename T1, typename T2>
std::map<std::pair<T1, T2>, DataWithAesthetic>
DataWithAesthetic::facet(const std::vector<T1> &data1,
                         const std::vector<T2> &data2) const {
  std::map<std::pair<T1, T2>, DataWithAesthetic> faceted_data;

  for (auto raw_data : m_data->facet(data1, data2)) {
    faceted_data[raw_data.first] =
        DataWithAesthetic(raw_data.second, m_map, m_limits);
  }

  return faceted_data;
}

template <typename Aesthetic, typename T>
void DataWithAesthetic::calculate_limits(T begin, T end) {
  if (begin != end) {
    // set m_limits with new data
    auto min_max = std::minmax_element(begin, end);
    float min = *min_max.first;
    float max = *min_max.second;

    // if limits are equal spread them out by 2*1e4*eps to stop zeros later on
    if (min == max) {
      min -= 1e4f * std::numeric_limits<float>::epsilon();
      max += 1e4f * std::numeric_limits<float>::epsilon();
    }

    set<Aesthetic>(min, max);
  }
}

template <typename Aesthetic, typename T>
void DataWithAesthetic::set(const std::vector<T> &data) {

  auto search = m_map.find(Aesthetic::index);

  if (search == m_map.end()) {
    // if aesthetic is not in data then add a new column
    search = m_map.insert({Aesthetic::index, m_data->cols()}).first;
    m_data->add_column(data);
  } else {
    // copy data to column (TODO: move this into RawData class)
    m_data->set_column(search->second, data);
  }

  calculate_limits<Aesthetic>(m_data->begin(search->second),
                              m_data->end(search->second));
}

/// returns true if Aesthetic has been set
template <typename Aesthetic> bool DataWithAesthetic::has() const {
  auto search = m_map.find(Aesthetic::index);
  return search != m_map.end();
}

template <typename T>
DataWithAesthetic &DataWithAesthetic::x(const std::vector<T> &data) {
  set<Aesthetic::x>(data);
  return *this;
}

template <typename T>
DataWithAesthetic &DataWithAesthetic::y(const std::vector<T> &data) {
  set<Aesthetic::y>(data);
  return *this;
}

template <typename T>
DataWithAesthetic &DataWithAesthetic::color(const std::vector<T> &data) {
  set<Aesthetic::color>(data);
  return *this;
}

template <typename T>
DataWithAesthetic &DataWithAesthetic::size(const std::vector<T> &data) {
  set<Aesthetic::size>(data);
  return *this;
}

template <typename T>
DataWithAesthetic &DataWithAesthetic::fill(const std::vector<T> &data) {
  set<Aesthetic::fill>(data);
  return *this;
}

template <typename T>
DataWithAesthetic &DataWithAesthetic::xmin(const std::vector<T> &data) {
  set<Aesthetic::xmin>(data);
  return *this;
}

template <typename T>
DataWithAesthetic &DataWithAesthetic::ymin(const std::vector<T> &data) {
  set<Aesthetic::ymin>(data);
  return *this;
}

template <typename T>
DataWithAesthetic &DataWithAesthetic::xmax(const std::vector<T> &data) {
  set<Aesthetic::xmax>(data);
  return *this;
}

template <typename T>
DataWithAesthetic &DataWithAesthetic::ymax(const std::vector<T> &data) {
  set<Aesthetic::ymax>(data);
  return *this;
}

template <typename Aesthetic>
void DataWithAesthetic::set(const float min, const float max) {
  m_limits.bmin[Aesthetic::index] = min;
  m_limits.bmax[Aesthetic::index] = max;
}

// specialisations of set here for xmin,xmax,ymin,ymax (these set the x/y
// aesthetic bounds accordingly)
template <>
void DataWithAesthetic::set<Aesthetic::xmin>(const float min, const float max);

template <>
void DataWithAesthetic::set<Aesthetic::xmax>(const float min, const float max);

template <>
void DataWithAesthetic::set<Aesthetic::ymin>(const float min, const float max);

template <>
void DataWithAesthetic::set<Aesthetic::ymax>(const float min, const float max);

} // namespace trase
