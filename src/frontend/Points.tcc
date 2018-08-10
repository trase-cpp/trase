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

#include "frontend/Points.hpp"

namespace trase {

template <typename AnimatedBackend>
void Points::draw(AnimatedBackend &backend) {
  draw_frames(backend);
}

template <typename Backend>
void Points::draw(Backend &backend, const float time) {
  update_frame_info(time);
  draw_plot(backend);
}

template <typename AnimatedBackend>
void Points::draw_frames(AnimatedBackend &backend) {

  auto to_pixel = [&](auto x, auto y, auto c, auto s) {
    return Vector<float, 4>{m_axis->to_display<Aesthetic::x>(x),
                            m_axis->to_display<Aesthetic::y>(y),
                            m_axis->to_display<Aesthetic::color>(c),
                            m_axis->to_display<Aesthetic::size>(s)};
  };

  backend.stroke_width(0);
  for (int i = 0; i < m_data[0].rows(); ++i) {
    for (size_t f = 0; f < m_times.size(); ++f) {
      auto p = to_pixel(m_data[f].begin<Aesthetic::x>()[i],
                        m_data[f].begin<Aesthetic::y>()[i],
                        m_data[f].begin<Aesthetic::color>()[i],
                        m_data[f].begin<Aesthetic::size>()[i]);

      backend.fill_color(m_colormap->to_color(p[2]));
      backend.add_animated_circle({p[0], p[1]}, p[3], m_times[f]);
    }
    backend.end_animated_circle();
  }
}

template <typename Backend> void Points::draw_plot(Backend &backend) {

  const int f = m_frame_info.frame_above;
  const float w1 = m_frame_info.w1;
  const float w2 = m_frame_info.w2;

  backend.stroke_width(0);

  auto to_pixel = [&](auto x, auto y, auto c, auto s) {
    return Vector<float, 4>{m_axis->to_display<Aesthetic::x>(x),
                            m_axis->to_display<Aesthetic::y>(y),
                            m_axis->to_display<Aesthetic::color>(c),
                            m_axis->to_display<Aesthetic::size>(s)};
  };

  if (w2 == 0.0f) {
    // exactly on a single frame
    auto x = m_data[f].begin<Aesthetic::x>();
    auto y = m_data[f].begin<Aesthetic::y>();
    auto color = m_data[f].begin<Aesthetic::color>();
    auto size = m_data[f].begin<Aesthetic::size>();
    for (int i = 0; i < m_data[0].rows(); ++i) {
      const auto p = to_pixel(x[i], y[i], color[i], size[i]);
      backend.fill_color(m_colormap->to_color(p[2]));
      backend.circle({p[0], p[1]}, p[3]);
    }
  } else {
    // between two frames
    auto x0 = m_data[f - 1].begin<Aesthetic::x>();
    auto y0 = m_data[f - 1].begin<Aesthetic::y>();
    auto color0 = m_data[f - 1].begin<Aesthetic::color>();
    auto size0 = m_data[f - 1].begin<Aesthetic::size>();
    auto x1 = m_data[f].begin<Aesthetic::x>();
    auto y1 = m_data[f].begin<Aesthetic::y>();
    auto color1 = m_data[f].begin<Aesthetic::color>();
    auto size1 = m_data[f].begin<Aesthetic::size>();
    for (int i = 0; i < m_data[0].rows(); ++i) {
      const auto p = w1 * to_pixel(x1[i], y1[i], color1[i], size1[i]) +
                     w2 * to_pixel(x0[i], y0[i], color0[i], size0[i]);
      backend.fill_color(m_colormap->to_color(p[2]));
      backend.circle({p[0], p[1]}, p[3]);
    }
  }
}

} // namespace trase
