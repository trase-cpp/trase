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

#include "frontend/Rectangle.hpp"
#include "util/Exception.hpp"

namespace trase {

template <typename AnimatedBackend>
void Rectangle::draw(AnimatedBackend &backend) {
  draw_frames(backend);
}

template <typename Backend>
void Rectangle::draw(Backend &backend, const float time) {
  update_frame_info(time);
  draw_plot(backend);
}

template <typename AnimatedBackend>
void Rectangle::draw_legend(AnimatedBackend &backend, const bfloat2_t &box) {
  bool have_color = m_data[0].has<Aesthetic::color>();

  backend.stroke_width(0);
  const auto box_middle = 0.5f * (box.bmin + box.bmax);
  const auto box_size = box.bmax - box.bmin;
  vfloat2_t s;
  s[0] = 0.25f * std::min(box_size[0], box_size[1]);
  s[1] = -s[0];
  auto p0 = box_middle - vfloat2_t{0.25f * box_size[0], 0};
  auto p1 = box_middle + vfloat2_t{0.25f * box_size[0], 0};
  if (have_color) {
    for (size_t f = 0; f < m_times.size(); ++f) {
      const auto &limits = m_data[f].limits();
      auto c = m_axis->to_display<Aesthetic::color>(
          limits.bmin[Aesthetic::color::index]);

      backend.add_animated_rect(bfloat2_t(p0 - s, p0 + s), m_times[f]);
    }
    backend.end_animated_circle();
    for (size_t f = 0; f < m_times.size(); ++f) {
      const auto &limits = m_data[f].limits();
      auto c = m_axis->to_display<Aesthetic::color>(
          limits.bmax[Aesthetic::color::index]);

      backend.add_animated_rect(bfloat2_t(p1 - s, p1 + s), m_times[f]);
    }
    backend.end_animated_rect();
  } else {
    backend.fill_color(m_colormap->to_color(0.f));
    backend.rect(bfloat2_t(p0 - s, p0 + s));
    backend.rect(bfloat2_t(p1 - s, p1 + s));
  }
}

template <typename Backend>
void Rectangle::draw_legend(Backend &backend, const float time,
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
  backend.rect(bfloat2_t(p0 - s, p0 + s));
  backend.fill_color(m_colormap->to_color(c1));
  backend.rect(bfloat2_t(p1 - s, p1 + s));
}

void Rectangle::validate_frames(const bool have_color, const int n) {
  for (size_t f = 0; f < m_times.size(); ++f) {
    const bool this_frame_have_color = m_data[f].has<Aesthetic::color>();
    const int this_frame_n = m_data[f].rows();

    if (this_frame_have_color != have_color) {
      throw Exception(
          "Frames found with and without color Aesthetic. Rectangle "
          "Geometry requires that the provided Aesthetics are "
          "identical for every frame.");
    }

    if (this_frame_n != n) {
      throw Exception(
          "Frames found with different numbers of points. Rectangle "
          "Geometry requires that the number of points for each "
          "frame are the same.");
    }
  }
}

template <typename AnimatedBackend>
void Rectangle::draw_frames(AnimatedBackend &backend) {
  // WARNING: do not make these const or gcc v5 seg faults on the lambda!!
  bool have_color = m_data[0].has<Aesthetic::color>();
  const int n = m_data[0].rows();

  validate_frames(have_color, n);

  auto to_pixel = [&](auto xmin, auto ymin, auto xmax, auto ymax, auto c) {
    // if color is not provided use the bottom of the scale
    return Vector<float, 5>{m_axis->to_display<Aesthetic::xmin>(xmin),
                            m_axis->to_display<Aesthetic::ymin>(ymin),
                            m_axis->to_display<Aesthetic::xmax>(xmax),
                            m_axis->to_display<Aesthetic::ymax>(ymax),
                            have_color ? m_axis->to_display<Aesthetic::color>(c)
                                       : 0.f};
  };

  backend.stroke_width(0);
  for (int i = 0; i < n; ++i) {
    for (size_t f = 0; f < m_times.size(); ++f) {
      auto p =
          to_pixel(m_data[f].begin<Aesthetic::xmin>()[i],
                   m_data[f].begin<Aesthetic::ymin>()[i],
                   m_data[f].begin<Aesthetic::xmax>()[i],
                   m_data[f].begin<Aesthetic::ymax>()[i],
                   have_color ? m_data[f].begin<Aesthetic::color>()[i] : 0.f);

      std::cout << "adding animated rect at "<< bfloat2_t({m_data[f].begin<Aesthetic::xmin>()[i],m_data[f].begin<Aesthetic::ymin>()[i]},{m_data[f].begin<Aesthetic::xmax>()[i],m_data[f].begin<Aesthetic::ymax>()[i]})<<" "<<bfloat2_t({p[0], p[1]}, {p[2], p[3]})<<std::endl;
      backend.add_animated_rect({{p[0], p[3]}, {p[2], p[1]}}, m_times[f]);
    }
    backend.end_animated_rect();
  }
}

template <typename Backend> void Rectangle::draw_plot(Backend &backend) {

  const int f = m_frame_info.frame_above;
  const float w1 = m_frame_info.w1;
  const float w2 = m_frame_info.w2;

  bool have_color = m_data[f].has<Aesthetic::color>();
  const int n = m_data[0].rows();

  validate_frames(have_color, n);

  backend.stroke_width(0);

  auto to_pixel = [&](auto xmin, auto ymin, auto xmax, auto ymax, auto c) {
    // if color is not provided use the bottom of the scale
    return Vector<float, 5>{m_axis->to_display<Aesthetic::xmin>(xmin),
                            m_axis->to_display<Aesthetic::ymin>(ymin),
                            m_axis->to_display<Aesthetic::xmax>(xmax),
                            m_axis->to_display<Aesthetic::ymax>(ymax),
                            have_color ? m_axis->to_display<Aesthetic::color>(c)
                                       : 0.f};
  };

  if (w2 == 0.0f) {
    // exactly on a single frame
    auto xmin = m_data[f].begin<Aesthetic::xmin>();
    auto ymin = m_data[f].begin<Aesthetic::ymin>();
    auto xmax = m_data[f].begin<Aesthetic::xmax>();
    auto ymax = m_data[f].begin<Aesthetic::ymax>();
    // if color not provided give a dummy iterator here, not used
    auto color = have_color ? m_data[f].begin<Aesthetic::color>() : xmin;
    for (int i = 0; i < m_data[0].rows(); ++i) {
      const auto p = to_pixel(xmin[i], ymin[i], xmax[i], ymax[i], color[i]);
      backend.fill_color(m_colormap->to_color(p[2]));
      backend.rect({{p[0], p[3]}, {p[2], p[1]}});
    }
  } else {
    // between two frames
    auto xmin0 = m_data[f - 1].begin<Aesthetic::xmin>();
    auto ymin0 = m_data[f - 1].begin<Aesthetic::ymin>();
    auto xmax0 = m_data[f - 1].begin<Aesthetic::xmax>();
    auto ymax0 = m_data[f - 1].begin<Aesthetic::ymax>();
    // if color not provided give a dummy iterator here, not used
    auto color0 = have_color ? m_data[f - 1].begin<Aesthetic::color>() : xmin0;
    auto xmin1 = m_data[f].begin<Aesthetic::xmin>();
    auto ymin1 = m_data[f].begin<Aesthetic::ymin>();
    auto xmax1 = m_data[f].begin<Aesthetic::xmax>();
    auto ymax1 = m_data[f].begin<Aesthetic::ymax>();
    // if color not provided give a dummy iterator here, not used
    auto color1 = have_color ? m_data[f].begin<Aesthetic::color>() : xmin0;
    for (int i = 0; i < m_data[0].rows(); ++i) {
      const auto p =
          w1 * to_pixel(xmin1[i], ymin1[i], xmax1[i], ymax1[i], color1[i]) +
          w2 * to_pixel(xmin0[i], ymin0[i], xmax0[i], ymax0[i], color0[i]);
      backend.fill_color(m_colormap->to_color(p[2]));
      backend.rect({{p[0], p[3]}, {p[2], p[1]}});
    }
  }
}

} // namespace trase
