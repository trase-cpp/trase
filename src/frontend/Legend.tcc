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

#include <algorithm>

#include "frontend/Legend.hpp"

namespace trase {

template <typename AnimatedBackend>
void Legend::draw(AnimatedBackend &backend) {

  const float sample_length = 20.f;

  // draw legend in upper right corner
  const vfloat2_t upper_right_corner = {m_pixels.bmax[0], m_pixels.bmin[1]};
  vfloat2_t text_loc = upper_right_corner;
  backend.text_align(ALIGN_RIGHT | ALIGN_TOP);
  backend.stroke_width(m_line_width);
  for (const auto &geometry : m_entries) {
    backend.begin_path();
    backend.move_to(text_loc +
                    vfloat2_t(-sample_length / 3.f, m_font_size / 2.f));
    backend.line_to(text_loc +
                    vfloat2_t(-(4.f / 3.f) * sample_length, m_font_size / 2.f));
    backend.stroke_color(geometry->get_color());
    backend.stroke();
    backend.text(text_loc + vfloat2_t(-(5.f / 3.f) * sample_length, 0.f),
                 geometry->get_label().c_str(), nullptr);
    text_loc[1] += m_font_size;
  }
}

template <typename Backend>
void Legend::draw(Backend &backend, const float time) {
  draw(backend);
}

} // namespace trase
