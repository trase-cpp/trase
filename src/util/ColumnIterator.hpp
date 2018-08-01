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

/// \file ColumnIterator.hpp

#ifndef COLUMNITERATOR_H_
#define COLUMNITERATOR_H_

#include <vector>

namespace trase {

/// A const iterator that iterates through a single column of the raw data class
/// Impliments an random access iterator with a given stride
class ColumnIterator {
public:
  using pointer = float const *;
  using iterator_category = std::random_access_iterator_tag;
  using reference = float const &;
  using value_type = float const;
  using difference_type = std::ptrdiff_t;

  ColumnIterator() = default;

  ColumnIterator(const std::vector<float>::const_iterator &p, const int stride)
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

  void increment() { std::advance(m_p, m_stride); }

  void increment(const int n) { std::advance(m_p, n * m_stride); }

  pointer m_p;
  int m_stride;
};

} // namespace trase

#endif // COLUMNITERATOR_H_
