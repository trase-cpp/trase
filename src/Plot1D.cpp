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
#include "BackendGL.hpp"
#include <algorithm>

namespace trase {

void Plot1D::set_values(std::vector<float> &&x, std::vector<float> &&y) {
  m_x = x;
  m_y = y;
  auto minmax = std::minmax_element(m_x.begin(), m_x.end());
  m_limits[0] = *minmax.first;
  m_limits[2] = *minmax.second;
  minmax = std::minmax_element(m_y.begin(), m_y.end());
  m_limits[1] = *minmax.first;
  m_limits[3] = *minmax.second;
  m_axis.add_limits(m_limits);
}

template <typename Backend> void Plot1D::draw(Backend &backend) {
  const float &x = m_pixels[0];
  const float &y = m_pixels[1];
  const float &w = m_pixels[2];
  const float &h = m_pixels[3];

  const float &xmin = m_limits[0];
  const float &ymin = m_limits[1];
  const float &xmax = m_limits[2];
  const float &ymax = m_limits[3];

  const float inv_yh = 1.0 / (ymax - ymin);
  const float inv_xw = 1.0 / (xmax - xmin);

  auto f_win_x = [&](const auto i) { return x + w * (i - xmin) * inv_xw; };
  auto f_win_y = [&](const auto i) {
    return y + h * (1 - (i - ymin) * inv_yh);
  };

  const float win_x = f_win_x(m_x[0]);
  const float win_y = f_win_y(m_y[0]);
  backend.begin_path();
  backend.move_to(win_x, win_y);
  for (size_t i = 1; i < m_x.size(); ++i) {
    const float win_x = f_win_x(m_x[i]);
    const float win_y = f_win_y(m_y[i]);
    backend.line_to(win_x, win_y);
  }
  backend.stroke_color(RGBA(0, 50, 100, 200));
  backend.stroke_width(3.0f);
  backend.stroke();
}

template void Plot1D::draw<BackendGL>(BackendGL &backend);

} // namespace trase
