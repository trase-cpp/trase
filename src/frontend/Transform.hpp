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

#ifndef TRANSFORM_H_
#define TRANSFORM_H_

#include <algorithm>
#include <cmath>
#include <functional>
#include <numeric>
#include <vector>

#include "frontend/Data.hpp"

namespace trase {

/// holds a `std::function` that maps between two DataWithAesthetic classes
class Transform {
  std::function<DataWithAesthetic(const DataWithAesthetic &)> m_transform;

public:
  /// construct a Transform wrapping the given transform function T. The
  /// function T can be any function or function object that is compatible with
  /// `std::function`
  template <typename T>
  explicit Transform(const T &transform) : m_transform(transform) {}

  /// perform mapping on `data`, return result
  DataWithAesthetic operator()(const DataWithAesthetic &data) {
    return m_transform(data);
  }
};

/// Identity transform, just pass through...
struct Identity {
  DataWithAesthetic operator()(const DataWithAesthetic &data) { return data; }
};

/// bin x coordinates
///
/// Requires x aesthetic.
struct BinX {
  int m_number_of_bins{-1};
  BinX() {}
  BinX(const int number_of_bins) : m_number_of_bins(number_of_bins) {}
  DataWithAesthetic operator()(const DataWithAesthetic &data) {
    auto x_begin = data.begin<Aesthetic::x>();
    auto x_end = data.end<Aesthetic::x>();
    int n;
    if (m_number_of_bins == -1) {
      auto sum = std::accumulate(x_begin, x_end, 0.f);
      auto mean = sum / data.rows();

      // note: use std::transform_reduce after c++17
      std::vector<float> diff(data.size());
      std::transform(x_begin, x_end, diff.begin(),
                     [mean](double x) { return x - mean; });
      double sq_sum =
          std::inner_product(diff.begin(), diff.end(), diff.begin(), 0.0);
      double stdev = std::sqrt(sq_sum / data.rows());

      // Scott, D. 1979.
      // On optimal and data-based histograms.
      // Biometrika, 66:605-610.
      n = 3.49f * stdev * std::pow(data.rows(), -0.33f);
    } else {
      n = m_number_of_bins;
    }

    auto minmax = std::minmax_element(x_begin, x_end);
    const auto dx = *minmax.second - *minmax.first;
    std::vector<float> bin_x(n), bin_y(n);

    // fill x bin coordinates as centre of each bin
    for (int i = 0; i < n; ++i) {
      bin_x[i] = *minmax.first + (i + 0.5f) * dx;
    }

    //  accumulate data into histogram
    std::for_each(x_begin, x_end,
                  [min = *minmax.first, dx, &bin_y](const float x) {
                    const auto i = static_cast<int>((x - min) / dx);
                    ++(bin_y[i]);
                  });

    data.set<Aesthetic::x>(bin_x);
    data.set<Aesthetic::y>(bin_y);

    return data;
  }
};

} // namespace trase

#endif // TRANSFORM_H_
