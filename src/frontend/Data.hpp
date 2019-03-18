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

/// \file Data.hpp
/// Contains classes and functions for handling data and aesthetics

#ifndef DATA_H_
#define DATA_H_

#include <cassert>
#include <functional>
#include <map>
#include <memory>
#include <set>
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

  // sets for non-numeric string data
  std::vector<std::set<std::string>> m_string_data;

  /// temporary data
  std::vector<float> m_tmp;

  int m_rows{0};
  int m_cols{0};

public:
  /// return the number of columns
  int cols() const { return m_cols; };

  /// return the number of rows
  int rows() const { return m_rows; };

  /// add a new column to the matrix using begin/end iterators. the data is
  /// copied into the new column
  template <typename T> void add_column(T new_col_begin, T new_col_end);

  /// add a new row to the matrix using begin/end iterators. the data is
  /// copied into the new column
  template <typename T> void add_row(T new_row_begin, T new_row_end);

  /// add a new column to the matrix. the data in `new_col` is copied into the
  /// new column
  template <typename T> void add_column(const std::vector<T> &new_col);

  /// add a new row to the matrix. the data in `new_row` is copied into the
  /// new row
  template <typename T> void add_row(const std::vector<T> &new_row);

  /// set a column in the matrix. the data in `new_col` is copied into column
  /// i
  template <typename T> void set_column(int i, const std::vector<T> &new_col);

  /// return a ColumnIterator to the beginning of column i
  ColumnIterator begin(int i) const;

  /// return a ColumnIterator to the end of column i
  ColumnIterator end(int i) const;

  /// return the set of strings for column i
  ///
  /// the returned set will be empty if column i contains numeric data
  const std::set<std::string> &string_data(int i) const;

  /// facets the data based on the input data column
  ///
  /// The input data column (of the same number of rows as this dataset)
  /// contains N unique values. This function returns a map of each of these N
  /// values to a dataset containing all the rows that have this value
  template <typename T>
  std::map<T, std::shared_ptr<RawData>> facet(const std::vector<T> &data) const;

  /// facets the data based on the dual input data columns
  ///
  /// The input data columns (of the same number of rows as this dataset)
  /// contains NxM unique value pairs. This function returns a map of each of
  /// these NxM values to a dataset containing all the rows that have this value
  template <typename T1, typename T2>
  std::map<std::pair<T1, T2>, std::shared_ptr<RawData>>
  facet(const std::vector<T1> &data1, const std::vector<T2> &data2) const;
};

/// Aesthetics are a collection of tag classes that represent each aesthetic
/// Each aesthetic has a name, and an index from 0 -> size, where size is the
/// total number of aesthetics
///
/// Each Aesthetic defines a mapping to and from a display type
struct Aesthetic {
  // total number of Aesthetics
  static const int N = 9;

  /// all aethetics except for xmin,ymin,xmax,ymax have their own min/max
  /// bounds
  using Limits = bbox<float, N - 4>;

  /// the data to display on the x-axis of the plot
  struct x {
    static const int index = 0;
    static const char *name;
    static float to_display(float data, const Limits &data_lim,
                            const bfloat2_t &display_lim);
    static float from_display(float display, const Limits &data_lim,
                              const bfloat2_t &display_lim);
  };

  /// the data to display on the y-axis of the plot
  struct y {
    static const int index = 1;
    static const char *name;
    static float to_display(float data, const Limits &data_lim,
                            const bfloat2_t &display_lim);
    static float from_display(float display, const Limits &data_lim,
                              const bfloat2_t &display_lim);
  };

  /// the color of each plotting element, scaled from 0 -> 1
  struct color {
    static const int index = 2;
    static const char *name;

    static float to_display(float data, const Limits &data_lim,
                            const bfloat2_t &display_lim);
    static float from_display(float display, const Limits &data_lim,
                              const bfloat2_t &display_lim);
  };

  /// the size of each plotting element, scaled from 1 pixel to 1/20 size of
  /// y-axis
  struct size {
    static const int index = 3;
    static const char *name;

    static float to_display(float data, const Limits &data_lim,
                            const bfloat2_t &display_lim);
    static float from_display(float display, const Limits &data_lim,
                              const bfloat2_t &display_lim);
  };

  /// the fill color of each plotting element, scaled from 0 -> 1
  struct fill {
    static const int index = 4;
    static const char *name;

    static float to_display(float data, const Limits &data_lim,
                            const bfloat2_t &display_lim);
    static float from_display(float display, const Limits &data_lim,
                              const bfloat2_t &display_lim);
  };

  // NOTE: xmin,ymin,xmax,ymax need to go at end so that the indices for
  // Limits work out

  /// the minimum x coordinate of the data
  struct xmin {
    static const int index = 5;
    static const char *name;
    static float to_display(float data, const Limits &data_lim,
                            const bfloat2_t &display_lim);
    static float from_display(float display, const Limits &data_lim,
                              const bfloat2_t &display_lim);
  };

  /// the minimum y coordinate of the data
  struct ymin {
    static const int index = 6;
    static const char *name;
    static float to_display(float data, const Limits &data_lim,
                            const bfloat2_t &display_lim);
    static float from_display(float display, const Limits &data_lim,
                              const bfloat2_t &display_lim);
  };

  /// the maximum x coordinate of the data
  struct xmax {
    static const int index = 7;
    static const char *name;
    static float to_display(float data, const Limits &data_lim,
                            const bfloat2_t &display_lim);
    static float from_display(float display, const Limits &data_lim,
                              const bfloat2_t &display_lim);
  };

  /// the maximum y coordinate of the data
  struct ymax {
    static const int index = 8;
    static const char *name;
    static float to_display(float data, const Limits &data_lim,
                            const bfloat2_t &display_lim);
    static float from_display(float display, const Limits &data_lim,
                              const bfloat2_t &display_lim);
  };
};

/// Each aesthetic (except for xmin/ymin/xmax/ymax) has a set of min/max
/// limits, or scales, that are used for plotting
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

  DataWithAesthetic(std::shared_ptr<RawData> data,
                    const std::unordered_map<int, int> &map,
                    const Limits &limits)
      : m_data(std::move(data)), m_map(map), m_limits(limits) {}

  /// return a ColumnIterator to the beginning of the data column for
  /// aesthetic a, throws if a has not yet been set
  template <typename Aesthetic> ColumnIterator begin() const;

  /// return a ColumnIterator to the end of the data column for aesthetic a,
  /// throws if a has not yet been set
  template <typename Aesthetic> ColumnIterator end() const;

  /// if aesthetic a is not yet been set, this creates a new data column and
  /// copies in `data` (throws if data does not have the correct number of
  /// rows). If aesthetic a has been previously set, its data column is
  /// overwritten with `data`
  template <typename Aesthetic, typename T>
  void set(const std::vector<T> &data);

  /// rather than adding new data, this allows the limits of a given aesthetic
  /// to be manually set. This is used, for example, with geometries where the
  /// data is implicitly defined over a range (e.g. histograms with regular
  /// bin widths)
  template <typename Aesthetic> void set(float min, float max);

  /// returns true if Aesthetic has been set
  template <typename Aesthetic> bool has() const;

  /// returns number of rows in the data set
  int rows() const;

  /// returns number of cols in the data set
  int cols() const;

  /// returns the min/max limits of the data
  const Limits &limits() const;

  template <typename T> DataWithAesthetic &x(const std::vector<T> &data);
  DataWithAesthetic &x(float min, float max);

  template <typename T> DataWithAesthetic &y(const std::vector<T> &data);
  DataWithAesthetic &y(float min, float max);

  template <typename T> DataWithAesthetic &color(const std::vector<T> &data);
  DataWithAesthetic &color(float min, float max);

  template <typename T> DataWithAesthetic &size(const std::vector<T> &data);
  DataWithAesthetic &size(float min, float max);

  template <typename T> DataWithAesthetic &fill(const std::vector<T> &data);
  DataWithAesthetic &fill(float min, float max);

  template <typename T> DataWithAesthetic &xmin(const std::vector<T> &data);
  DataWithAesthetic &xmin(float min, float max);

  template <typename T> DataWithAesthetic &ymin(const std::vector<T> &data);
  DataWithAesthetic &ymin(float min, float max);

  template <typename T> DataWithAesthetic &xmax(const std::vector<T> &data);
  DataWithAesthetic &xmax(float min, float max);

  template <typename T> DataWithAesthetic &ymax(const std::vector<T> &data);
  DataWithAesthetic &ymax(float min, float max);

  /// facets the data based on the input data column
  ///
  /// The input data column (of the same number of rows as this dataset)
  /// contains N unique values. This function returns a map of each of these N
  /// values to a dataset containing all the rows that have this value
  template <typename T>
  std::map<T, DataWithAesthetic> facet(const std::vector<T> &data) const;

  /// facets the data based on the dual input data columns
  ///
  /// The input data columns (of the same number of rows as this dataset)
  /// contains NxM unique value pairs. This function returns a map of each of
  /// these NxM values to a dataset containing all the rows that have this value
  template <typename T1, typename T2>
  std::map<std::pair<T1, T2>, DataWithAesthetic>
  facet(const std::vector<T1> &data1, const std::vector<T2> &data2) const;

private:
  template <typename Aesthetic, typename T>
  void calculate_limits(T begin, T end);
};

/// creates a new, empty dataset
/// \return an empty DataWithAesthetic
DataWithAesthetic create_data();

} // namespace trase

#include "Data.tcc"

#endif // DATA_H_
