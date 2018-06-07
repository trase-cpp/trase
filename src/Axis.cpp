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

#include "BackendGL.hpp"
#include "Figure.hpp"

namespace trase {

Axis::Axis(const std::array<float, 4> &area)
    : Drawable(area), m_limits{{0, 0, 1, 1}} {}

void Axis::add_limits(const std::array<float, 4> limits) {}

template <typename Backend> void Axis::draw(Backend &backend) {
  const float &x = m_pixels[0];
  const float &y = m_pixels[1];
  const float &w = m_pixels[2];
  const float &h = m_pixels[3];

  const float &xmin = m_limits[0];
  const float &ymin = m_limits[1];
  const float &xmax = m_limits[2];
  const float &ymax = m_limits[3];

  const float lw = 3.0f;
  backend.stroke_width(lw);

  // axis box
  backend.begin_path();
  backend.rect(x, y, w, h);
  backend.stroke();

  // axis lines
  /*
  backend.begin_path();
  backend.move_to(x - lw / 2, y + h);
  backend.line_to(x + w, y + h);
  backend.move_to(x, y + h + lw / 2);
  backend.line_to(x, y);
  backend.stroke_color(RGBA(0, 0, 0, 255));
  backend.stroke_width(lw);
  backend.stroke();
  */

  // drop shadow
  /*
  backend.begin_path();
  backend.move_to(x - lw, y + h + lw / 2);
  backend.line_to(x + w - lw / 2, y + h + lw / 2);
  backend.move_to(x - lw / 2, y + h + lw);
  backend.line_to(x - lw / 2, y + lw / 2);
  backend.stroke_color(RGBA(0, 0, 0, 20));
  backend.stroke();
  */

  // ticks
  const int ny_ticks = 5;
  const float tick_dx = h / ny_ticks;
  const float tick_dx_y = (ymax - ymin) / ny_ticks;
  const int nx_ticks = w / tick_dx;
  const float tick_len = 10.0f;
  const float tick_dx_x = (xmax - xmin) / nx_ticks;

  backend.begin_path();
  backend.font_size(18.0f);
  backend.font_blur(0.0f);
  backend.font_face("sans-bold");
  backend.text_align(ALIGN_CENTER | ALIGN_TOP);
  backend.fill_color(RGBA(0, 0, 0, 255));

  char buffer[10];
  for (int i = 0; i < nx_ticks; ++i) {
    const float tick_pos = x + (i + 0.5) * tick_dx;
    const float tick_val = xmin + (i + 0.5) * tick_dx_x;
    backend.move_to(tick_pos, y + h + tick_len / 2);
    backend.line_to(tick_pos, y + h - tick_len / 2);
    std::sprintf(buffer, "%1.2f", tick_val);
    backend.text(tick_pos, y + h + tick_len / 2, buffer, NULL);
  }
  backend.text_align(ALIGN_RIGHT | ALIGN_MIDDLE);
  for (int i = 0; i < ny_ticks; ++i) {
    const float tick_pos = y + h - (i + 0.5) * tick_dx;
    const float tick_val = ymin + (i + 0.5) * tick_dx_y;
    backend.move_to(x - tick_len / 2, tick_pos);
    backend.line_to(x + tick_len / 2, tick_pos);
    std::sprintf(buffer, "%1.2f", tick_val);
    backend.text(x - tick_len / 2, tick_pos, buffer, NULL);
  }
  backend.stroke_color(RGBA(0, 0, 0, 255));
  backend.stroke_width(lw / 2);
  backend.stroke();
}

template void Axis::draw<BackendGL>(BackendGL &backend);

} // namespace trase
