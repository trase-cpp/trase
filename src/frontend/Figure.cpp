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

#include "frontend/Figure.hpp"

#include <array>
#include <string>

#include "util/BBox.hpp"
#include "util/Vector.hpp"

namespace trase {

int Figure::m_num_windows = 0;

Figure::Figure(const std::array<float, 2> &pixels)
    : Drawable(nullptr,
               bfloat2_t(vfloat2_t(0, 0), vfloat2_t(pixels[0], pixels[1]))),
      m_id(++m_num_windows), m_axis_layout{0, 0} {
  m_pixels = m_area;
}

std::shared_ptr<Axis> Figure::update_layout(const vint2_t &new_position) {
  // only allow positive layout positions
  if ((new_position < vint2_t{0, 0}).any()) {
    throw std::out_of_range("Subplot layout position must be non-negative.");
  }

  // determine new layout size
  bool resize = false;
  for (int i = 0; i < 2; ++i) {
    if (m_axis_layout[i] < new_position[i] + 1) {
      resize = true;
      m_axis_layout[i] = new_position[i] + 1;
    }
  }

  // if layout changed then resize all children
  vfloat2_t axis_size = vfloat2_t{1.0f, 1.0f} / m_axis_layout.cast<float>();
  if (resize) {
    for (auto &subplot : m_axis_subplots) {
      vfloat2_t position_from_lower_left = {
          static_cast<float>(subplot.first[0]),
          static_cast<float>(m_axis_layout[1] - subplot.first[1] - 1)};
      subplot.second->area().bmin =
          (position_from_lower_left + 0.1f) * axis_size;
      subplot.second->area().bmax =
          (position_from_lower_left + 0.9f) * axis_size;
      subplot.second->resize(m_pixels);
    }
  }

  // insert new axis into layout or get existing
  vfloat2_t position_from_lower_left = {
      static_cast<float>(new_position[0]),
      static_cast<float>(m_axis_layout[1] - new_position[1] - 1)};

  bfloat2_t bounding_box((position_from_lower_left + 0.1f) * axis_size,
                         (position_from_lower_left + 0.9f) * axis_size);
  auto result =
      m_axis_subplots
          .insert(std::make_pair(new_position,
                                 std::make_shared<Axis>(this, bounding_box)))
          .first;
  result->second->resize(m_pixels);

  // return the actual shared_ptr to the Axis
  return result->second;
}

std::shared_ptr<Axis> Figure::axis() { return axis(0, 0); }

std::shared_ptr<Axis> Figure::axis(int i, int j) {
  auto new_axis = update_layout({j, i});
  m_children.push_back(new_axis);
  return new_axis;
}

} // namespace trase
