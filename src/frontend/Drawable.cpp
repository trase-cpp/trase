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

#include "frontend/Drawable.hpp"

#include "util/Exception.hpp"

namespace trase {

Drawable::Drawable(Drawable *parent, const bfloat2_t &area_of_parent)
    : m_parent(parent), m_area(area_of_parent), m_time_span(0), m_times({0}) {
  if (parent) {
    resize(m_parent->m_pixels);
  }
}

void Drawable::resize(const bfloat2_t &parent_pixels) {
  m_pixels.bmin = m_area.bmin * parent_pixels.delta() + parent_pixels.min();
  m_pixels.bmax = m_area.bmax * parent_pixels.delta() + parent_pixels.min();
  for (auto &i : m_children) {
    i->resize(m_pixels);
  }
}

void Drawable::update_frame_info(const float time) {

  // if time is outside time range clip it
  float clipped_time = time;
  if (time < 0.f) {
    clipped_time = 0.f;
  } else if (time > m_time_span) {
    clipped_time = m_time_span;
  }

  m_frame_info.update(m_times, clipped_time);
}

const FrameInfo &Drawable::get_frame_info() const { return m_frame_info; }

void Drawable::add_frame_time(const float time) {
  if (time < m_times.back()) {
    throw Exception("cannot add frame with time less than max frame time");
  }
  m_times.push_back(time);
  update_time_span(time);
}

void Drawable::update_time_span(const float time) {
  if (time > m_time_span) {
    m_time_span = time;
  }

  // need to inform parents
  if (m_parent != nullptr) {
    m_parent->update_time_span(time);
  }
}

Style &Drawable::style() noexcept { return m_style; }

} // namespace trase
