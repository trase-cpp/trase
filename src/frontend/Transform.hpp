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

/// \file Transform.hpp

#ifndef TRANSFORM_H_
#define TRANSFORM_H_

#include <functional>

#include "frontend/Data.hpp"
#include "util/BBox.hpp"

namespace trase {

/// Identity transform, just pass through...
struct Identity {
  DataWithAesthetic operator()(const DataWithAesthetic &data) const {
    return data;
  }
};

/// bin x coordinates
///
/// Requires x aesthetic.
class BinX {
  int m_number_of_bins{-1};
  bbox<float, 1> m_span;

public:
  BinX() = default;
  explicit BinX(int number_of_bins);
  explicit BinX(int number_of_bins, float min, float max);
  DataWithAesthetic operator()(const DataWithAesthetic &data);
};

/// holds a `std::function` that maps between two DataWithAesthetic classes
class Transform {
  std::function<DataWithAesthetic(const DataWithAesthetic &)> m_transform;

public:
  /// construct an identity transform
  Transform() : m_transform(Identity()) {}

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

} // namespace trase

#endif // TRANSFORM_H_
