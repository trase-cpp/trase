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

#include "Axis.hpp"
#include "Backend.hpp"
#include <cmath>
#include <limits>

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

  backend.stroke_width(m_line_width);

  // axis box
  backend.stroke_color(RGBA(200, 200, 200, 255));
  backend.fill_color(RGBA(200, 200, 200, 255));
  backend.rect(m_pixels);

  // ticks
  const auto pixel_delta = m_pixels.delta();
  const auto limits_delta = m_limits.delta();
  const int ny_ticks = 5;
  vfloat2_t n_ticks(ny_ticks * pixel_delta[0] / pixel_delta[1], ny_ticks);
  const int sig_digits = 2;

  // calculate y tick locations
  const auto tick_dx = round_off(limits_delta / n_ticks, sig_digits);
  const auto tick_min = ceil(m_limits.bmin / tick_dx) * tick_dx;
  const auto tick_max = floor(m_limits.bmax / tick_dx) * tick_dx;
  n_ticks = (tick_max - tick_min) / tick_dx + 1.0f;

  // scale to pixels
  const auto tick_dx_pixels = tick_dx * pixel_delta / limits_delta;

  const auto tick_min_pixels = to_pixel(tick_min);

  const float tick_len = 10.0f;

  backend.begin_path();
  backend.font_size(m_font_size);
  backend.font_blur(0.0f);
  backend.font_face(m_font_face.c_str());
  backend.text_align(ALIGN_CENTER | ALIGN_TOP);
  backend.fill_color(RGBA(0, 0, 0, 255));

  // axis ticks
  char buffer[100];
  for (int i = 0; i < n_ticks[0]; ++i) {
    const float tick_pos = tick_min_pixels[0] + i * tick_dx_pixels[0];
    const float tick_val = tick_min[0] + i * tick_dx[0];
    backend.move_to(vfloat2_t(tick_pos, m_pixels.bmax[1] + tick_len / 2));
    backend.line_to(vfloat2_t(tick_pos, m_pixels.bmax[1]));
    std::snprintf(buffer, sizeof(buffer), "%.*g", sig_digits + 1, tick_val);
    backend.text(vfloat2_t(tick_pos, m_pixels.bmax[1] + tick_len / 2), buffer,
                 NULL);
  }
  backend.text_align(ALIGN_RIGHT | ALIGN_MIDDLE);
  for (int i = 0; i < n_ticks[1]; ++i) {
    const float tick_pos = tick_min_pixels[1] - i * tick_dx_pixels[1];
    const float tick_val = tick_min[1] + i * tick_dx[1];
    backend.move_to(vfloat2_t(m_pixels.bmin[0] - tick_len / 2, tick_pos));
    backend.line_to(vfloat2_t(m_pixels.bmin[0], tick_pos));
    std::snprintf(buffer, sizeof(buffer), "%.*g", sig_digits + 1, tick_val);
    backend.text(vfloat2_t(m_pixels.bmin[0] - tick_len / 2, tick_pos), buffer,
                 NULL);
  }
  backend.stroke_color(RGBA(0, 0, 0, 255));
  backend.stroke_width(m_line_width / 2);
  backend.stroke();

  // axis grid lines
  backend.begin_path();
  for (int i = 0; i < n_ticks[0]; ++i) {
    const float tick_pos = tick_min_pixels[0] + i * tick_dx_pixels[0];
    backend.move_to(vfloat2_t(tick_pos, m_pixels.bmin[1]));
    backend.line_to(vfloat2_t(tick_pos, m_pixels.bmax[1]));
  }
  for (int i = 0; i < n_ticks[1]; ++i) {
    const float tick_pos = tick_min_pixels[1] - i * tick_dx_pixels[1];
    backend.move_to(vfloat2_t(m_pixels.bmin[0], tick_pos));
    backend.line_to(vfloat2_t(m_pixels.bmax[0], tick_pos));
  }
  backend.stroke_color(RGBA(255, 255, 255, 255));
  backend.stroke_width(m_line_width / 2.f);
  backend.stroke();

  // axis title
  if (!m_title.empty()) {
    backend.text_align(ALIGN_CENTER | ALIGN_BOTTOM);
    backend.text(vfloat2_t(0.5f * (m_pixels.bmax[0] + m_pixels.bmin[0]),
                           m_pixels.bmin[1] -
                               0.05f * (m_pixels.bmax[1] - m_pixels.bmin[1])),
                 m_title.c_str(), NULL);
  }

  // axis labels
  if (!m_xlabel.empty()) {
    backend.text_align(ALIGN_CENTER | ALIGN_TOP);
    backend.text(vfloat2_t(0.5f * (m_pixels.bmax[0] + m_pixels.bmin[0]),
                           m_pixels.bmax[1] +
                               0.05f * (m_pixels.bmax[1] - m_pixels.bmin[1])),
                 m_xlabel.c_str(), NULL);
  }

  if (!m_ylabel.empty()) {
    backend.text_align(ALIGN_CENTER | ALIGN_BOTTOM);
    const vfloat2_t point = vfloat2_t(
        m_pixels.bmin[0] - 0.05f * (m_pixels.bmax[0] - m_pixels.bmin[0]),
        0.5f * (m_pixels.bmax[1] + m_pixels.bmin[1]));
    backend.translate(point);
    backend.rotate(-pi / 2.0f);
    backend.text(vfloat2_t(0.f, 0.f), m_ylabel.c_str(), NULL);
    backend.reset_transform();
  }

  if (m_legend) {
    draw_common_legend(backend);
  }
}

template <typename Backend> void Axis::draw_common_legend(Backend &backend) {

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
