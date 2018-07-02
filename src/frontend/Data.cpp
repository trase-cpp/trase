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

#include "frontend/Data.hpp"

namespace trase {

const int Aesthetic::size;
const int Aesthetic::x::index;
const char *Aesthetic::x::name = "x";
const int Aesthetic::y::index;
const char *Aesthetic::y::name = "y";
const int Aesthetic::color::index;
const char *Aesthetic::color::name = "y";

template <typename T> void RawDat::add_column(const std::vector<T> &new_col) {
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

} // namespace trase
