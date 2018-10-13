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
void Points::draw_legend(AnimatedBackend &backend, const bfloat2_t &box) {
  bool have_color = m_data[0].has<Aesthetic::color>();

  backend.stroke_width(0);
  const auto box_middle = 0.5f * (box.bmin + box.bmax);
  const auto box_size = box.bmax - box.bmin;
  auto s = 0.25f * std::min(box_size[0], box_size[1]);
  auto p0 = box_middle - vfloat2_t{0.25f * box_size[0], 0};
  auto p1 = box_middle + vfloat2_t{0.25f * box_size[0], 0};
  if (have_color) {
    for (size_t f = 0; f < m_times.size(); ++f) {
      const auto &limits = m_data[f].limits();
      auto c = m_axis->to_display<Aesthetic::color>(
          limits.bmin[Aesthetic::color::index]);

      backend.fill_color(m_colormap->to_color(c));
      backend.add_animated_circle(p0, s, m_times[f]);
    }
    backend.end_animated_circle();
    for (size_t f = 0; f < m_times.size(); ++f) {
      const auto &limits = m_data[f].limits();
      auto c = m_axis->to_display<Aesthetic::color>(
          limits.bmax[Aesthetic::color::index]);

      backend.fill_color(m_colormap->to_color(c));
      backend.add_animated_circle(p1, s, m_times[f]);
    }
    backend.end_animated_circle();
  } else {
    backend.fill_color(m_colormap->to_color(0.f));
    backend.circle(p0, s);
    backend.circle(p1, s);
  }
}

template <typename Backend>
void Points::draw_legend(Backend &backend, const float time,
                         const bfloat2_t &box) {

  update_frame_info(time);
  const int f = m_frame_info.frame_above;
  const float w1 = m_frame_info.w1;
  const float w2 = m_frame_info.w2;

  bool have_color = m_data[f].has<Aesthetic::color>();

  const auto box_middle = 0.5f * (box.bmin + box.bmax);
  const auto box_size = box.bmax - box.bmin;
  auto s = 0.25f * std::min(box_size[0], box_size[1]);
  auto p0 = box_middle - vfloat2_t{0.25f * box_size[0], 0};
  auto p1 = box_middle + vfloat2_t{0.25f * box_size[0], 0};

  // get min and max colors at this time
  float c0 = 0.f;
  float c1 = 0.f;
  if (have_color) {
    if (w2 == 0.0f) {
      c0 = m_data[f].limits().bmin[Aesthetic::color::index];
      c1 = m_data[f].limits().bmax[Aesthetic::color::index];
    } else {
      c0 = w1 * m_data[f].limits().bmin[Aesthetic::color::index] +
           w2 * m_data[f - 1].limits().bmin[Aesthetic::color::index];
      c1 = w1 * m_data[f].limits().bmax[Aesthetic::color::index] +
           w2 * m_data[f - 1].limits().bmax[Aesthetic::color::index];
    }
  }
  c0 = m_axis->to_display<Aesthetic::color>(c0);
  c1 = m_axis->to_display<Aesthetic::color>(c1);

  backend.stroke_width(0);
  backend.fill_color(m_colormap->to_color(c0));
  backend.circle(p0, s);
  backend.fill_color(m_colormap->to_color(c1));
  backend.circle(p1, s);
}

template <typename AnimatedBackend>
void Points::draw_frames(AnimatedBackend &backend) {
  // TODO: assumption that same aesthetics are provided for each frame
  bool have_color = m_data[0].has<Aesthetic::color>();
  bool have_size = m_data[0].has<Aesthetic::size>();

  auto to_pixel = [&](auto x, auto y, auto c, auto s) {
    // if color or size is not provided use the bottom of the scale
    return Vector<float, 4>{
        m_axis->to_display<Aesthetic::x>(x),
        m_axis->to_display<Aesthetic::y>(y),
        have_color ? m_axis->to_display<Aesthetic::color>(c) : 0.f,
        have_size ? m_axis->to_display<Aesthetic::size>(s) : 1.f};
  };

  backend.stroke_width(0);
  for (int i = 0; i < m_data[0].rows(); ++i) {
    for (size_t f = 0; f < m_times.size(); ++f) {
      auto p =
          to_pixel(m_data[f].begin<Aesthetic::x>()[i],
                   m_data[f].begin<Aesthetic::y>()[i],
                   have_color ? m_data[f].begin<Aesthetic::color>()[i] : 0.f,
                   have_size ? m_data[f].begin<Aesthetic::size>()[i] : 0.f);

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

  bool have_color = m_data[f].has<Aesthetic::color>();
  bool have_size = m_data[f].has<Aesthetic::size>();

  backend.stroke_width(0);

  auto to_pixel = [&](auto x, auto y, auto c, auto s) {
    // if color or size is not provided use the bottom of the scale
    return Vector<float, 4>{
        m_axis->to_display<Aesthetic::x>(x),
        m_axis->to_display<Aesthetic::y>(y),
        have_color ? m_axis->to_display<Aesthetic::color>(c) : 0.f,
        have_size ? m_axis->to_display<Aesthetic::size>(s) : 1.f};
  };

  if (w2 == 0.0f) {
    // exactly on a single frame
    auto x = m_data[f].begin<Aesthetic::x>();
    auto y = m_data[f].begin<Aesthetic::y>();
    // if color or size not provided give a dummy iterator here, not used
    auto color = have_color ? m_data[f].begin<Aesthetic::color>() : x;
    auto size = have_size ? m_data[f].begin<Aesthetic::size>() : x;
    for (int i = 0; i < m_data[0].rows(); ++i) {
      const auto p = to_pixel(x[i], y[i], color[i], size[i]);
      backend.fill_color(m_colormap->to_color(p[2]));
      backend.circle({p[0], p[1]}, p[3]);
    }
  } else {
    // between two frames
    auto x0 = m_data[f - 1].begin<Aesthetic::x>();
    auto y0 = m_data[f - 1].begin<Aesthetic::y>();
    // if color or size not provided give a dummy iterator here, not used
    auto color0 = have_color ? m_data[f - 1].begin<Aesthetic::color>() : x0;
    auto size0 = have_size ? m_data[f - 1].begin<Aesthetic::size>() : x0;
    auto x1 = m_data[f].begin<Aesthetic::x>();
    auto y1 = m_data[f].begin<Aesthetic::y>();
    // if color or size not provided give a dummy iterator here, not used
    auto color1 = have_color ? m_data[f].begin<Aesthetic::color>() : x1;
    auto size1 = have_size ? m_data[f].begin<Aesthetic::size>() : x1;
    for (int i = 0; i < m_data[0].rows(); ++i) {
      const auto p = w1 * to_pixel(x1[i], y1[i], color1[i], size1[i]) +
                     w2 * to_pixel(x0[i], y0[i], color0[i], size0[i]);
      backend.fill_color(m_colormap->to_color(p[2]));
      backend.circle({p[0], p[1]}, p[3]);
    }
  }
}

} // namespace trase
