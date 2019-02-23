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

#include "Facets.hpp"
#include <vector>

namespace trase {

template <Aesthetic>
std::vector<DataWithAesthetics> facet(const DataWithAesthetic &data) {
  std::vector<DataWithAesthetic> fdata;

  auto aes_data = data.begin<Aesthetic>();
  std::vector<std::size_t> row_indices(data.rows());
  std::iota(row_indices.begin(), row_indices.end(), 0);
  std::sort(
      row_indices.begin(), row_indices.end(),
      [&](std::size_t i, std::size_t j) { return aes_data[i] < aes_data[j]; });

  auto i = row_indices.begin();
  while (i != row_indices.end()) {
    auto j = i;
    while (aes_data[*(j + 1)] <= aes_data[*j]) {
      ++j;
    }

    RawData raw_data;
    for (int k = 0; k < data.get_raw_data().cols(); ++i) {
      std::vector<float> tmp(j - i);
      std::transform(i, j, tmp.begin(),
                     [row = data.get_raw_data().begin(k)](std::size_t i) {
                       return row[i];
                     });
      raw_data.add_column(tmp);
    }
  }

  fdata.emplace_back(data.get_map(),raw_data);
}

} // namespace trase
