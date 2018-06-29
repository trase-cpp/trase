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

#include <cmath>
#include <limits>

#include "backend/Backend.hpp"
#include "frontend/Axis.hpp"

namespace trase {

template <typename Backend>
void Axis::draw(Backend &backend, const float time) {
  draw_common(backend);

  // draw plots
  backend.scissor(m_pixels);
  for (auto &i : m_plot1d) {
    i->draw(backend, time);
  }
  backend.reset_scissor();
}

template <typename Backend> void Axis::serialise(Backend &backend) {
  draw_common(backend);

  // serialise plots
  backend.scissor(m_pixels);
  for (auto &i : m_plot1d) {
    i->serialise(backend);
  }
  backend.reset_scissor();
}

template <typename Backend> void Axis::draw_common(Backend &backend) {
  update_tick_information();

  draw_common_axis_box(backend);
  draw_common_ticks(backend);
  draw_common_gridlines(backend);
  draw_common_title(backend);
  draw_common_xlabel(backend);
  draw_common_ylabel(backend);
  draw_common_legend(backend);
}

template <typename Backend> void Axis::draw_common_axis_box(Backend &backend) {
  backend.stroke_width(m_line_width);
  backend.stroke_color(RGBA(200, 200, 200, 255));
  backend.fill_color(RGBA(200, 200, 200, 255));
  backend.rect(m_pixels);
}

template <typename Backend> void Axis::draw_common_ticks(Backend &backend) {

  char buffer[100];

  backend.begin_path();
  backend.font_size(m_font_size);
  backend.font_blur(0.0f);
  backend.font_face(m_font_face.c_str());
  backend.text_align(ALIGN_CENTER | ALIGN_TOP);
  backend.fill_color(RGBA(0, 0, 0, 255));

  // x ticks
  for (std::size_t i = 0; i < m_tick_info.x_pos.size(); ++i) {
    const float pos = m_tick_info.x_pos[i];
    const float val = m_tick_info.x_val[i];

    backend.move_to(vfloat2_t(pos, m_pixels.bmax[1] + m_tick_len / 2));
    backend.line_to(vfloat2_t(pos, m_pixels.bmax[1]));
    std::snprintf(buffer, sizeof(buffer), "%.*g", m_sig_digits + 1, val);
    backend.text(vfloat2_t(pos, m_pixels.bmax[1] + m_tick_len / 2), buffer,
                 NULL);
  }

  backend.text_align(ALIGN_RIGHT | ALIGN_MIDDLE);

  // y ticks
  for (std::size_t i = 0; i < m_tick_info.y_pos.size(); ++i) {
    const float pos = m_tick_info.y_pos[i];
    const float val = m_tick_info.y_val[i];

    backend.move_to(vfloat2_t(m_pixels.bmin[0] - m_tick_len / 2, pos));
    backend.line_to(vfloat2_t(m_pixels.bmin[0], pos));
    std::snprintf(buffer, sizeof(buffer), "%.*g", m_sig_digits + 1, val);
    backend.text(vfloat2_t(m_pixels.bmin[0] - m_tick_len / 2, pos), buffer,
                 NULL);
  }

  backend.stroke_color(RGBA(0, 0, 0, 255));
  backend.stroke_width(m_line_width / 2);
  backend.stroke();
}

template <typename Backend> void Axis::draw_common_gridlines(Backend &backend) {

  backend.begin_path();

  // x gridlines
  for (const float &tick_pos : m_tick_info.x_pos) {
    backend.move_to(vfloat2_t(tick_pos, m_pixels.bmin[1]));
    backend.line_to(vfloat2_t(tick_pos, m_pixels.bmax[1]));
  }

  // y gridlines
  for (const float &tick_pos : m_tick_info.y_pos) {
    backend.move_to(vfloat2_t(m_pixels.bmin[0], tick_pos));
    backend.line_to(vfloat2_t(m_pixels.bmax[0], tick_pos));
  }

  backend.stroke_color(RGBA(255, 255, 255, 255));
  backend.stroke_width(m_line_width / 2.f);
  backend.stroke();
}

void Axis::update_tick_information() {

  // Use num ticks if user defined, or calculate with defaults
  vfloat2_t n_ticks = calculate_num_ticks();

  // Calculate ideal distance between ticks in limits coords
  const vfloat2_t tick_dx = round_off(m_limits.delta() / n_ticks, m_sig_digits);

  // Idealise the lowest pick position
  const vfloat2_t tick_min = ceil(m_limits.bmin / tick_dx) * tick_dx;

  // scale to pixels
  const vfloat2_t tick_dx_pixels =
      tick_dx * m_pixels.delta() / m_limits.delta();
  const vfloat2_t tick_min_pixels = to_pixel(tick_min);

  // Update values in the struct
  m_tick_info.clear();

  // x tick values and positions
  for (int i = 0; i < static_cast<int>(n_ticks[0]); ++i) {
    m_tick_info.x_val.emplace_back(tick_min[0] + i * tick_dx[0]);
    m_tick_info.x_pos.emplace_back(tick_min_pixels[0] + i * tick_dx_pixels[0]);
  }

  // y tick values and positions
  for (int i = 0; i < static_cast<int>(n_ticks[1]); ++i) {
    m_tick_info.y_val.emplace_back(tick_min[1] + i * tick_dx[1]);
    m_tick_info.y_pos.emplace_back(tick_min_pixels[1] - i * tick_dx_pixels[1]);
  }
}

vfloat2_t Axis::calculate_num_ticks() {

  if (m_nx_ticks > 0 && m_ny_ticks > 0) {
    return {static_cast<float>(m_nx_ticks), static_cast<float>(m_ny_ticks)};
  }

  const float pix_ratio = m_pixels.delta()[0] / m_pixels.delta()[1];

  if (m_nx_ticks > 0) {
    const float n = static_cast<float>(m_nx_ticks);
    return {n, std::floor(n / pix_ratio)};
  }

  if (m_ny_ticks > 0) {
    const float n = static_cast<float>(m_ny_ticks);
    return {std::floor(n * pix_ratio), n};
  }

  return {std::floor(5.f * pix_ratio), 5.f};
}

template <typename Backend> void Axis::draw_common_title(Backend &backend) {

  if (m_title.empty()) {
    return;
  }

  backend.text_align(ALIGN_CENTER | ALIGN_BOTTOM);
  backend.text(vfloat2_t(0.5f * (m_pixels.bmax[0] + m_pixels.bmin[0]),
                         m_pixels.bmin[1] -
                             0.05f * (m_pixels.bmax[1] - m_pixels.bmin[1])),
               m_title.c_str(), NULL);
}

template <typename Backend> void Axis::draw_common_xlabel(Backend &backend) {

  if (m_xlabel.empty()) {
    return;
  }

  backend.text_align(ALIGN_CENTER | ALIGN_TOP);
  backend.text(vfloat2_t(0.5f * (m_pixels.bmax[0] + m_pixels.bmin[0]),
                         m_pixels.bmax[1] +
                             0.05f * (m_pixels.bmax[1] - m_pixels.bmin[1])),
               m_xlabel.c_str(), NULL);
}

template <typename Backend> void Axis::draw_common_ylabel(Backend &backend) {

  if (m_ylabel.empty()) {
    return;
  }

  backend.text_align(ALIGN_CENTER | ALIGN_BOTTOM);
  const vfloat2_t point = vfloat2_t(
      m_pixels.bmin[0] - 0.05f * (m_pixels.bmax[0] - m_pixels.bmin[0]),
      0.5f * (m_pixels.bmax[1] + m_pixels.bmin[1]));
  backend.translate(point);
  backend.rotate(-pi / 2.0f);
  backend.text(vfloat2_t(0.f, 0.f), m_ylabel.c_str(), NULL);
  backend.reset_transform();
}

template <typename Backend> void Axis::draw_common_legend(Backend &backend) {

  if (!m_legend) {
    return;
  }

  const float sample_length = 20.f;

  // draw legend in upper right corner
  const vfloat2_t upper_right_corner = {m_pixels.bmax[0], m_pixels.bmin[1]};
  vfloat2_t text_loc = upper_right_corner;
  backend.text_align(ALIGN_RIGHT | ALIGN_TOP);
  backend.stroke_width(m_line_width);
  for (const auto &i : m_plot1d) {
    backend.begin_path();
    backend.move_to(text_loc +
                    vfloat2_t(-sample_length / 3.f, m_font_size / 2.f));
    backend.line_to(text_loc +
                    vfloat2_t(-(4.f / 3.f) * sample_length, m_font_size / 2.f));
    backend.stroke_color(i->get_color());
    backend.stroke();
    backend.text(text_loc + vfloat2_t(-(5.f / 3.f) * sample_length, 0.f),
                 i->get_label().c_str(), nullptr);
    text_loc[1] += m_font_size;
  }
}

} // namespace trase
