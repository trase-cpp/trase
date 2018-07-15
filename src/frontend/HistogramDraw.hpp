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

#include "frontend/Histogram.hpp"

namespace trase {

template <typename Backend> void Histogram::serialise(Backend &backend) {
  serialise_frames(backend);
}

template <typename Backend>
void Histogram::draw(Backend &backend, const float time) {
  update_frame_info(time);
  draw_plot(backend);
}

template <typename Backend> void Histogram::serialise_frames(Backend &backend) {

  backend.stroke_color(m_color);
  backend.stroke_width(m_line_width);
  backend.fill_color(m_fill_color);

  // x should be constant and regular spaced
  auto x_begin = m_data[0]->begin<Aesthetic::x>();
  auto x_end = m_data[0]->end<Aesthetic::x>();
  const float min = m_axis.to_display<Aesthetic::x>(*x_begin);
  const float dx = m_axis.to_display<Aesthetic::x>(x_begin[1] - x_begin[0]);

  for (int i = 0; i < m_data[0]->rows(); ++i) {
    for (size_t f = 0; f < m_times.size(); ++f) {
      auto y_data = m_data[f]->begin<Aesthetic::y>()[i];
      auto y = m_axis.to_display<Aesthetic::y>(y_data);
      auto x_min = (i - 0.5f) * dx;
      auto x_max = (i + 0.5f) * dx;
      backend.add_animated_rect(bfloat2_t({x_min, 0.f}, {x_max, y}),
                                m_times[f]);
    }
    backend.end_animated_rect();
  }
}

template <typename Backend> void Histogram::draw_plot(Backend &backend) {

  const int f = m_frame_info.frame_above;
  const float w1 = m_frame_info.w1;
  const float w2 = m_frame_info.w2;

  backend.stroke_color(m_color);
  backend.stroke_width(m_line_width);
  backend.fill_color(m_fill_color);

  // x should be constant and regular spaced
  auto x_begin = m_data[0]->begin<Aesthetic::x>();
  auto x_end = m_data[0]->end<Aesthetic::x>();
  const float min = m_axis.to_display<Aesthetic::x>(*x_begin);
  const float dx = m_axis.to_display<Aesthetic::x>(x_begin[1] - x_begin[0]);

  if (w2 == 0.0f) {
    // exactly on a single frame
    auto y_data = m_data[f]->begin<Aesthetic::y>();
    for (int i = 1; i < m_data[0]->rows(); ++i) {
      auto x_min = (i - 0.5f) * dx;
      auto x_max = (i + 0.5f) * dx;
      auto y = m_axis.to_display<Aesthetic::y>(y_data[i]);
      backend.rect(bfloat2_t({x_min, 0.f}, {x_max, y}));
    }

  } else {
    auto y0 = m_data[f - 1]->begin<Aesthetic::y>();
    auto y1 = m_data[f]->begin<Aesthetic::y>();
    for (int i = 1; i < m_data[0]->rows(); ++i) {
      auto x_min = (i - 0.5f) * dx;
      auto x_max = (i + 0.5f) * dx;
      auto y = w1 * m_axis.to_display<Aesthetic::y>(y1[i]) +
               w2 * m_axis.to_display<Aesthetic::y>(y0[i]);
      backend.rect(bfloat2_t({x_min, 0.f}, {x_max, y}));
    }
  }
}

} // namespace trase
