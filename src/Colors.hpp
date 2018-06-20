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

#ifndef COLORS_H_
#define COLORS_H_

#include <array>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

namespace trase {

struct RGBA {
  RGBA() = default;
  RGBA(int r, int g, int b, int a) noexcept : m_r(r), m_g(g), m_b(b), m_a(a) {}
  std::string to_rgb_string() const {
    std::stringstream stream;
    stream.precision(2);

    stream << '#' << std::hex << std::setfill('0') << std::setw(2) << m_r
           << std::setw(2) << m_g << std::setw(2) << m_b;
    return stream.str();
  }
  int m_r, m_g, m_b, m_a;
};

const static int default_alpha = 200;

// default colors taken from d3 category10 color scheme
// https://github.com/d3/d3-3.x-api-reference/blob/master/Ordinal-Scales.md#category10
const static std::array<RGBA, 10> default_colors = {
    RGBA(31, 119, 180, default_alpha),  RGBA(255, 127, 14, default_alpha),
    RGBA(44, 160, 44, default_alpha),   RGBA(214, 39, 40, default_alpha),
    RGBA(148, 103, 189, default_alpha), RGBA(140, 86, 75, default_alpha),
    RGBA(227, 119, 194, default_alpha), RGBA(127, 127, 127, default_alpha),
    RGBA(188, 189, 34, default_alpha),  RGBA(23, 190, 207, default_alpha)};

} // namespace trase

#endif // COLORS_H_
