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

#include "frontend/Axis.hpp"
#include "frontend/Histogram.hpp"

namespace trase {

template <typename AnimatedBackend>
void Histogram::draw(AnimatedBackend &backend) {
  draw_frames(backend);
}

template <typename Backend>
void Histogram::draw(Backend &backend, const float time) {
  update_frame_info(time);
  draw_plot(backend);
}

template <typename AnimatedBackend>
void Histogram::draw_legend(AnimatedBackend &backend, const bfloat2_t &box) {
  backend.stroke_color(m_style.color());
  backend.stroke_width(m_style.line_width());
  backend.fill_color(m_style.color());
  backend.rect(box * vfloat2_t{0.75f, 0.75f});
}

template <typename Backend>
void Histogram::draw_legend(Backend &backend, const float time,
                            const bfloat2_t &box) {
  draw_legend(backend, box);
}

template <typename AnimatedBackend>
void Histogram::draw_frames(AnimatedBackend &backend) {

  backend.stroke_color(m_style.color());
  backend.stroke_width(m_style.line_width());
  backend.fill_color(m_style.color());

  // x should be constant and regular spaced with a dx calculated by the limits
  // and the number of rows
  const float x0 = m_data[0].limits().bmin[Aesthetic::x::index];
  const float dx =
      (m_data[0].limits().bmax[Aesthetic::x::index] - x0) / m_data[0].rows();

  for (int i = 0; i < m_data[0].rows(); ++i) {
    for (size_t f = 0; f < m_times.size(); ++f) {
      auto y_data = m_data[f].begin<Aesthetic::y>()[i];
      auto y_min = m_axis->to_display<Aesthetic::y>(y_data);
      auto y_max = m_axis->to_display<Aesthetic::y>(0.f);
      auto x_min = m_axis->to_display<Aesthetic::x>(i * dx + x0);
      auto x_max = m_axis->to_display<Aesthetic::x>((i + 1.f) * dx + x0);
      backend.add_animated_rect(bfloat2_t({x_min, y_min}, {x_max, y_max}),
                                m_times[f]);
    }
    backend.end_animated_rect();
  }
}

template <typename Backend> void Histogram::draw_plot(Backend &backend) {

  const int f = m_frame_info.frame_above;
  const float w1 = m_frame_info.w1;
  const float w2 = m_frame_info.w2;

  backend.stroke_color(m_style.color());
  backend.stroke_width(m_style.line_width());
  backend.fill_color(m_style.color());

  // x should be constant and regular spaced with a dx calculated by the limits
  // and the number of rows
  const float x0 = m_data[0].limits().bmin[Aesthetic::x::index];
  const float dx =
      (m_data[0].limits().bmax[Aesthetic::x::index] - x0) / m_data[0].rows();

  if (w2 == 0.0f) {
    // exactly on a single frame
    auto y_data = m_data[f].begin<Aesthetic::y>();
    for (int i = 0; i < m_data[0].rows(); ++i) {
      auto x_min = m_axis->to_display<Aesthetic::x>(i * dx + x0);
      auto x_max = m_axis->to_display<Aesthetic::x>((i + 1.f) * dx + x0);
      auto y_min = m_axis->to_display<Aesthetic::y>(y_data[i]);
      auto y_max = m_axis->to_display<Aesthetic::y>(0.f);
      backend.rect(bfloat2_t({x_min, y_min}, {x_max, y_max}));
    }
  } else {
    auto y0 = m_data[f - 1].begin<Aesthetic::y>();
    auto y1 = m_data[f].begin<Aesthetic::y>();
    for (int i = 0; i < m_data[0].rows(); ++i) {
      auto x_min = m_axis->to_display<Aesthetic::x>(i * dx + x0);
      auto x_max = m_axis->to_display<Aesthetic::x>((i + 1.f) * dx + x0);
      auto y_min = w1 * m_axis->to_display<Aesthetic::y>(y1[i]) +
                   w2 * m_axis->to_display<Aesthetic::y>(y0[i]);
      auto y_max = m_axis->to_display<Aesthetic::y>(0.f);
      backend.rect(bfloat2_t({x_min, y_min}, {x_max, y_max}));
    }
  }
}

} // namespace trase
