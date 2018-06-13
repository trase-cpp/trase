/*
Copyright (c) 2018, University of Oxford.
All rights reserved.

University of Oxford means the Chancellor, Masters and Scholars of the
University of Oxford, having an administrative office at Wellington
Square, Oxford OX1 2JD, UK.

This file is part of the Oxford RSE C++ Template project.

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

#include "Plot1D.hpp"
#include <algorithm>

namespace trase {

template <typename Backend> void Plot1D::draw(Backend &backend) {
  float frame_i_float = get_frame_index();
  int frame_i = std::ceil(frame_i_float);
  const float w2 = frame_i - frame_i_float;
  const float w1 = 1.0f - w2;

  if (w2 == 0.0f) {
    backend.begin_path();
    backend.move_to(m_axis.to_pixel(m_values[frame_i][0]));
    for (size_t i = 1; i < m_values[0].size(); ++i) {
      backend.line_to(m_axis.to_pixel(m_values[frame_i][i]));
    }
  } else {
    backend.begin_path();
    backend.move_to(m_axis.to_pixel(w1 * m_values[frame_i][0] +
                                    w2 * m_values[frame_i - 1][0]));
    for (size_t i = 1; i < m_values[0].size(); ++i) {
      backend.line_to(m_axis.to_pixel(w1 * m_values[frame_i][i] +
                                      w2 * m_values[frame_i - 1][i]));
    }
  }
  backend.stroke_color(m_color);
  backend.stroke_width(3.0f);
  backend.stroke();
}

} // namespace trase
