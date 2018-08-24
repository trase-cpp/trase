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
      m_id(++m_num_windows) {
  m_pixels = m_area;
}

std::shared_ptr<Axis> Figure::axis() noexcept {
  auto new_axis =
      std::make_shared<Axis>(this, bfloat2_t({0.1f, 0.1f}, {0.9f, 0.9f}));
  new_axis->resize(m_pixels);
  m_children.push_back(new_axis);
  return new_axis;
}

std::shared_ptr<Axis> axis(int i, int j) noexcept;

std::shared_ptr<Axis> Figure::axis(int n) {
  return std::dynamic_pointer_cast<Axis>(m_children.at(n));
}

} // namespace trase
