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

template <typename Backend> void Plot1D::serialise(Backend &backend) {
  const float lw = 3.0f;

  // draw first frame
  backend.begin_frames();
  backend.stroke_color(m_color);
  backend.stroke_width(lw);

  backend.move_to(m_axis.to_pixel(m_values[0][0]));
  for (size_t i = 1; i < m_values[0].size(); ++i) {
    backend.line_to(m_axis.to_pixel(m_values[0][i]));
  }

  // other frames
  for (size_t f = 1; f < m_times.size(); ++f) {
    backend.add_frame(m_times[f - 1]);
    backend.move_to(m_axis.to_pixel(m_values[f][0]));
    for (size_t i = 1; i < m_values[0].size(); ++i) {
      backend.line_to(m_axis.to_pixel(m_values[f][i]));
    }
  }

  backend.end_frames(m_times.back());
}

template <typename Backend>
void Plot1D::draw(Backend &backend, const float time) {
  const float lw = 3.0f;

  // draw plot
  backend.begin_path();

  float frame_i_float = get_frame_index(time);
  int frame_i = std::ceil(frame_i_float);
  const float w2 = frame_i - frame_i_float;
  const float w1 = 1.0f - w2;

  if (w2 == 0.0f) {
    backend.move_to(m_axis.to_pixel(m_values[frame_i][0]));
    for (size_t i = 1; i < m_values[0].size(); ++i) {
      backend.line_to(m_axis.to_pixel(m_values[frame_i][i]));
    }
  } else {
    backend.move_to(m_axis.to_pixel(w1 * m_values[frame_i][0] +
                                    w2 * m_values[frame_i - 1][0]));
    for (size_t i = 1; i < m_values[0].size(); ++i) {
      backend.line_to(m_axis.to_pixel(w1 * m_values[frame_i][i] +
                                      w2 * m_values[frame_i - 1][i]));
    }
  }

  backend.stroke_color(m_color);
  backend.stroke_width(lw);
  backend.stroke();

  // draw highlighted point
  auto pos = vfloat2_t(std::numeric_limits<float>::max(),
                       std::numeric_limits<float>::max());
  if (backend.is_interactive()) {
    auto mouse_pos = backend.get_mouse_pos();
    if ((mouse_pos > m_pixels.bmin).all() &&
        (mouse_pos < m_pixels.bmax).all()) {
      pos = m_axis.from_pixel(mouse_pos);
    }
  }

  const float r2 =
      std::pow(3 * lw * m_limits.delta()[0] / m_pixels.delta()[0], 2);
  char buffer[20];
  auto draw_close_point = [&](const vfloat2_t &point) {
    if ((point - pos).squaredNorm() < r2) {
      backend.begin_path();
      auto point_pixel = m_axis.to_pixel(point);
      backend.circle(point_pixel, lw * 2);
      backend.fill_color(m_color);
      backend.fill();

      std::sprintf(buffer, "(%f,%f)", point[0], point[1]);
      backend.fill_color(RGBA(0, 0, 0, 255));
      backend.text_align(ALIGN_LEFT | ALIGN_BOTTOM);
      backend.text(point_pixel + 2 * vfloat2_t(lw, -lw), buffer, NULL);
    }
  };

  if (w2 == 0.0f) {
    draw_close_point(m_values[frame_i][0]);
    for (size_t i = 1; i < m_values[0].size(); ++i) {
      draw_close_point(m_values[frame_i][i]);
    }
  } else {
    draw_close_point(w1 * m_values[frame_i][0] + w2 * m_values[frame_i - 1][0]);
    for (size_t i = 1; i < m_values[0].size(); ++i) {
      draw_close_point(w1 * m_values[frame_i][i] +
                       w2 * m_values[frame_i - 1][i]);
    }
  }
}

} // namespace trase
