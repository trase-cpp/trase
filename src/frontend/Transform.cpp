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

#include <algorithm>
#include <cmath>
#include <limits>
#include <numeric>
#include <vector>

#include "frontend/Transform.hpp"

namespace trase {

BinX::BinX(const int number_of_bins) : m_number_of_bins(number_of_bins) {}
BinX::BinX(const int number_of_bins, const float min, const float max)
    : m_number_of_bins(number_of_bins),
      m_span(Vector<float, 1>(min), Vector<float, 1>({max})) {}

DataWithAesthetic BinX::operator()(const DataWithAesthetic &data) {
  auto x_begin = data.begin<Aesthetic::x>();
  auto x_end = data.end<Aesthetic::x>();

  // if input data is empty then create an empty y aesthetic
  if (std::distance(x_begin, x_end) == 0) {
    std::vector<float> y;
    return create_data().y(y);
  }

  auto minmax = std::minmax_element(x_begin, x_end);

  if (m_span.is_empty()) {
    // increase the span slightly so round-off doesn't cause points to fall
    // outside the domain
    m_span.bmin[0] =
        *minmax.first - 1e4f * std::numeric_limits<float>::epsilon();
    m_span.bmin[1] =
        *minmax.second + 1e4f * std::numeric_limits<float>::epsilon();
  }

  if (m_number_of_bins == -1) {
    auto sum = std::accumulate(x_begin, x_end, 0.f);
    auto mean = sum / data.rows();

    // note: use std::transform_reduce after c++17
    std::vector<float> diff(data.rows());
    std::transform(x_begin, x_end, diff.begin(),
                   [mean](float x) { return x - mean; });
    auto sq_sum =
        std::inner_product(diff.begin(), diff.end(), diff.begin(), 0.f);
    auto stdev = std::sqrt(sq_sum / data.rows());

    // Scott, D. 1979.
    // On optimal and data-based histograms.
    // Biometrika, 66:605-610.
    const float dx =
        3.49f * stdev * std::pow(static_cast<float>(data.rows()), -0.33f);

    // if calculated dx is too small then set number of bins to pre-determined
    // number
    if (dx > m_span.delta()[0] / 200.f) {
      m_number_of_bins = static_cast<int>(std::round(m_span.delta()[0] / dx));
    } else {
      m_number_of_bins = 200;
    }
  }

  const float dx = m_span.delta()[0] / m_number_of_bins;

  std::vector<float> bin_y(m_number_of_bins);

  // zero y bin values
  std::fill(bin_y.begin(), bin_y.end(), 0.f);

  //  accumulate data into histogram
  std::for_each(x_begin, x_end, [&](const float x) {
    const auto i = static_cast<int>(std::floor((x - m_span.bmin[0]) / dx));
    if (i >= 0 && i < m_number_of_bins) {
      ++(bin_y[i]);
    }
  });

  // return new data set, making sure to set ymin to zero
  DataWithAesthetic ret;
  ret.x(m_span.bmin[0], m_span.bmax[0]).y(bin_y);
  ret.y(0.f, ret.limits().bmax[Aesthetic::y::index]);
  return ret;
}

} // namespace trase
