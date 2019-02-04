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

#include "frontend/Line.hpp"

namespace trase {

template <typename AnimatedBackend> void Line::draw(AnimatedBackend &backend) {
  draw_frames(backend);
  draw_anim_highlights(backend);
}

template <typename Backend>
void Line::draw(Backend &backend, const float time) {
  update_frame_info(time);
  draw_plot(backend);
  draw_highlights(backend);
}

template <typename AnimatedBackend>
void Line::draw_legend(AnimatedBackend &backend, const bfloat2_t &box) {
  backend.stroke_color(m_style.color());
  backend.stroke_width(m_style.line_width());
  const float y_coord = 0.5f * (box.bmin[1] + box.bmax[1]);
  backend.begin_path();
  backend.move_to({box.bmax[0], y_coord});
  backend.line_to({box.bmin[0], y_coord});
  backend.stroke();
}

template <typename Backend>
void Line::draw_legend(Backend &backend, const float time,
                       const bfloat2_t &box) {

  draw_legend(backend, box);
}

template <typename AnimatedBackend>
void Line::draw_frames(AnimatedBackend &backend) {

  backend.begin_animated_path();
  backend.stroke_color(m_style.color());
  backend.stroke_width(m_style.line_width());

  auto to_pixel = [&](auto x, auto y) {
    return vfloat2_t{m_axis->to_display<Aesthetic::x>(x),
                     m_axis->to_display<Aesthetic::y>(y)};
  };

  // find maximum length of all datasets
  // AnimatedBackend requires that an animated path be the same number of
  // points. Therefore we will find the maximum line length needed, and, for
  // shorter lines, simply repeat the last point the required number of times
  const int n =
      std::accumulate(m_data.begin(), m_data.end(), 0,
                      [](int a, auto b) { return std::max(a, b.rows()); });

  auto x = m_data[0].begin<Aesthetic::x>();
  auto y = m_data[0].begin<Aesthetic::y>();
  backend.move_to(to_pixel(x[0], y[0]));
  for (int i = 1; i < n; ++i) {
    const int clip_i = std::min(m_data[0].rows() - 1, i);
    backend.line_to(to_pixel(x[clip_i], y[clip_i]));
  }

  // other frames
  for (size_t f = 1; f < m_times.size(); ++f) {
    auto x = m_data[f].begin<Aesthetic::x>();
    auto y = m_data[f].begin<Aesthetic::y>();
    backend.add_animated_path(m_times[f - 1]);
    backend.move_to(to_pixel(x[0], y[0]));
    for (int i = 1; i < n; ++i) {
      const int clip_i = std::min(m_data[f].rows() - 1, i);
      backend.line_to(to_pixel(x[clip_i], y[clip_i]));
    }
  }

  backend.end_animated_path(m_times.back());
}

template <typename AnimatedBackend>
void Line::draw_anim_highlights(AnimatedBackend &backend) {

  // highlighted points just for frame 0 and for stationary lines
  if (m_times.size() == 1) {
    char buffer[100];
    auto color = m_style.color();
    color.a(0);
    backend.stroke_color(RGBA(0, 0, 0, 0));
    backend.fill_color(color, m_style.color());

    auto x = m_data[0].begin<Aesthetic::x>();
    auto y = m_data[0].begin<Aesthetic::y>();
    for (int i = 0; i < m_data[0].rows(); ++i) {
      vfloat2_t point = {x[i], y[i]};
      vfloat2_t point_pixel = {m_axis->to_display<Aesthetic::x>(x[i]),
                               m_axis->to_display<Aesthetic::y>(y[i])};
      std::snprintf(buffer, sizeof(buffer), "(%f,%f)", point[0], point[1]);
      backend.tooltip(
          point_pixel + 2.f * vfloat2_t(m_style.line_width(), -m_style.line_width()), buffer);
      backend.circle(point_pixel, 2 * m_style.line_width());
    }
    backend.clear_tooltip();
  }
}

template <typename Backend> void Line::draw_plot(Backend &backend) {
  backend.begin_path();

  const int f = m_frame_info.frame_above;
  const float w1 = m_frame_info.w1;
  const float w2 = m_frame_info.w2;

  auto to_pixel = [&](auto x, auto y) {
    return vfloat2_t{m_axis->to_display<Aesthetic::x>(x),
                     m_axis->to_display<Aesthetic::y>(y)};
  };

  if (w2 == 0.0f) {
    // exactly on a single frame
    auto x = m_data[f].begin<Aesthetic::x>();
    auto y = m_data[f].begin<Aesthetic::y>();
    backend.move_to(to_pixel(x[0], y[0]));
    for (int i = 1; i < m_data[0].rows(); ++i) {
      backend.line_to(to_pixel(x[i], y[i]));
    }
  } else {
    // between two frames
    auto x0 = m_data[f - 1].begin<Aesthetic::x>();
    auto y0 = m_data[f - 1].begin<Aesthetic::y>();
    auto x1 = m_data[f].begin<Aesthetic::x>();
    auto y1 = m_data[f].begin<Aesthetic::y>();
    backend.move_to(w1 * to_pixel(x1[0], y1[0]) + w2 * to_pixel(x0[0], y0[0]));
    const int last_i = std::min(m_data[f - 1].rows(), m_data[f].rows());
    for (int i = 1; i < last_i; ++i) {
      backend.line_to(w1 * to_pixel(x1[i], y1[i]) +
                      w2 * to_pixel(x0[i], y0[i]));
    }
    if (m_data[f].rows() > last_i) {
      backend.line_to(w1 * to_pixel(x1[last_i], y1[last_i]) +
                      w2 * to_pixel(x0[last_i - 1], y0[last_i - 1]));
    }
  }

  backend.stroke_color(m_style.color());
  backend.stroke_width(m_style.line_width());
  backend.stroke();
}

template <typename Backend> void Line::draw_highlights(Backend &backend) {

  // get mouse position
  auto pos = vfloat2_t(std::numeric_limits<float>::max(),
                       std::numeric_limits<float>::max());
  auto mouse_pos = pos;

  if (backend.is_interactive()) {
    mouse_pos = backend.get_mouse_pos();
    if ((mouse_pos > m_pixels.bmin).all() &&
        (mouse_pos < m_pixels.bmax).all()) {
      pos = {m_axis->from_display<Aesthetic::x>(mouse_pos[0]),
             m_axis->from_display<Aesthetic::x>(mouse_pos[1])};
    }
  }

  // highlight mouse-over point if exactly on a frame (i.e. stationary line)
  if (m_frame_info.w2 == 0.f) {
    float min_r2 = std::numeric_limits<float>::max();
    vfloat2_t min_point{};
    // exactly on a frame
    auto x = m_data[m_frame_info.frame_above].begin<Aesthetic::x>();
    auto y = m_data[m_frame_info.frame_above].begin<Aesthetic::y>();
    for (int i = 0; i < m_data[0].rows(); ++i) {
      const vfloat2_t point = {x[i], y[i]};
      auto point_r2 = (point - pos).squaredNorm();
      if (point_r2 < min_r2) {
        min_point = point;
        min_r2 = point_r2;
      }
    }

    // define search radius
    const vfloat2_t point_pixel = {
        m_axis->to_display<Aesthetic::x>(min_point[0]),
        m_axis->to_display<Aesthetic::x>(min_point[1])};

    // if a point is within r2, then draw it
    if ((mouse_pos - point_pixel).squaredNorm() <
        std::pow(2.f * m_style.line_width(), 2)) {
      backend.fill_color(m_style.color());
      backend.text_align(ALIGN_LEFT | ALIGN_BOTTOM);
      char buffer[100];
      std::snprintf(buffer, sizeof(buffer), "(%f,%f)", min_point[0],
                    min_point[1]);
      backend.circle(point_pixel, m_style.line_width() * 2);
      backend.fill_color(RGBA(0, 0, 0, 255));
      backend.text(point_pixel + 2.f * vfloat2_t(m_style.line_width(), -m_style.line_width()),
                   buffer, nullptr);
    }
  }
}

} // namespace trase
