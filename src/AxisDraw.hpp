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

#include "Axis.hpp"
#include <limits>

namespace trase {

float round_off(float num, int n) {

  // Counting the no. of digits to the left of decimal point
  // in the given no.
  float tmp = num;
  float i;
  for (i = 0; tmp >= 1; ++i)
    tmp /= 10;

  // round off to the given number of sig digits
  const float d = std::pow(10, n - i);
  return std::floor(num * d + 0.5) / d;
}

template <typename Backend> void Axis::draw(Backend &backend) {
  const float &x = m_pixels[0];
  const float &y = m_pixels[1];
  const float &w = m_pixels[2];
  const float &h = m_pixels[3];

  const float &xmin = m_limits[0];
  const float &ymin = m_limits[1];
  const float &xmax = m_limits[2];
  const float &ymax = m_limits[3];
  const float yh = ymax - ymin;
  const float xw = xmax - xmin;

  const float lw = 3.0f;
  backend.stroke_width(lw);

  // axis box
  backend.begin_path();
  backend.rect(m_pixels);
  backend.fill_color(RGBA(0, 0, 0, 20));
  backend.fill();

  // ticks
  const auto pixel_delta = m_pixels.delta();
  const auto limits_delta = m_limits.delta();
  const int nx_ticks;
  const vfloat2_t n_ticks(nx_ticks, nx_ticks * pixel_delta[1] / pixel_delta[0]);
  const int sig_digits = 2;

  // calculate y tick locations
  const auto tick_min = round_off(m_limits.bmin, 1);
  const auto tick_dx = round_off(limits_delta() / n_ticks, sig_digits);
  // scale to pixels
  const auto tick_dx_pixels = tick_dx * pixel_delta / limits_delta;

  auto to_pixel = [&](const vfloat2_t &i) {
    auto inv_delta = 1.0f / limits_delta;
    return m_pixels +
           pixels_delta *
               vfloat2_t((i[0] - m_limits.bmin[0]) * inv_delta[0],
                         (1 - (i[1] - m_limits.bmin[1]) * inv_delta[1]));
  };

  const auto tick_min_pixels = to_pixel(tick_min);

  const float tick_len = 10.0f;

  backend.begin_path();
  backend.font_size(18.0f);
  backend.font_blur(0.0f);
  backend.font_face("sans-bold");
  backend.text_align(ALIGN_CENTER | ALIGN_TOP);
  backend.fill_color(RGBA(0, 0, 0, 255));

  char buffer[10];
  for (int i = 0; i < nx_ticks; ++i) {
    const float tick_pos = tick_min_x_pixels + (i + 0.5) * tick_dx_x_pixels;
    const float tick_val = tick_min_x + (i + 0.5) * tick_dx_x;
    backend.move_to(tick_pos, y + h + tick_len / 2);
    backend.line_to(tick_pos, y + h - tick_len / 2);
    std::sprintf(buffer, "%.*g", sig_digits, tick_val);
    backend.text(tick_pos, y + h + tick_len / 2, buffer, NULL);
  }
  backend.text_align(ALIGN_RIGHT | ALIGN_MIDDLE);
  for (int i = 0; i < ny_ticks; ++i) {
    const float tick_pos = tick_min_y_pixels - (i + 0.5) * tick_dx_y_pixels;
    const float tick_val = tick_min_y + (i + 0.5) * tick_dx_y;
    backend.move_to(x - tick_len / 2, tick_pos);
    backend.line_to(x + tick_len / 2, tick_pos);
    std::sprintf(buffer, "%.*g", sig_digits, tick_val);
    backend.text(x - tick_len / 2, tick_pos, buffer, NULL);
  }
  backend.stroke_color(RGBA(0, 0, 0, 255));
  backend.stroke_width(lw / 2);
  backend.stroke();

  // draw plots
  backend.scissor(x, y, w, h);
  for (auto &i : m_plot1d) {
    i->draw(backend);
  }
  backend.reset_scissor();
}

} // namespace trase
