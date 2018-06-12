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
  const auto &axis_limits = m_axis.limits();
  const auto &axis_pixels = m_axis.pixels();
  const auto axis_delta = axis_limits.delta();
  const auto pixel_delta = axis_pixels.delta();

  auto to_pixel = [&](const vfloat2_t &i) {
    auto inv_delta = 1.0f / axis_delta;
    return m_pixels.bmin +
           pixel_delta *
               vfloat2_t((i[0] - axis_limits.bmin[0]) * inv_delta[0],
                         (1 - (i[1] - axis_limits.bmin[1]) * inv_delta[1]));
  };

  backend.begin_path();
  backend.move_to(to_pixel(vfloat2_t(m_x[0], m_y[0])));
  for (size_t i = 1; i < m_x.size(); ++i) {
    backend.line_to(to_pixel(vfloat2_t(m_x[i], m_y[i])));
  }
  backend.stroke_color(m_color);
  backend.stroke_width(3.0f);
  backend.stroke();
}

} // namespace trase
