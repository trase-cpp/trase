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

#include "util/Colors.hpp"

#include <iomanip>
#include <iostream>
#include <sstream>
#include "util/Exception.hpp"

namespace trase {

RGBA::RGBA(int r, int g, int b, int a) noexcept
    : m_r(r), m_g(g), m_b(b), m_a(a) {}

int RGBA::r() const noexcept { return m_r; }
int RGBA::g() const noexcept { return m_g; }
int RGBA::b() const noexcept { return m_b; }
int RGBA::a() const noexcept { return m_a; }

RGBA &RGBA::r(const int r) noexcept {
  m_r = r;
  return *this;
}

RGBA &RGBA::g(const int g) noexcept {
  m_g = g;
  return *this;
}

RGBA &RGBA::b(const int b) noexcept {
  m_b = b;
  return *this;
}

RGBA &RGBA::a(const int a) noexcept {
  m_a = a;
  return *this;
}

std::string RGBA::to_rgb_string() const noexcept {
  std::stringstream stream;
  stream.precision(2);

  stream << '#' << std::hex << std::setfill('0') << std::setw(2) << m_r
         << std::setw(2) << m_g << std::setw(2) << m_b;
  return stream.str();
}

bool RGBA::operator==(const RGBA &b) const noexcept {
  return m_r == b.r() && m_g == b.g() && m_b == b.b() && m_a == b.a();
}


bool RGBA::operator!=(const RGBA &b) const noexcept {
  return !(*this == b);
}

const int RGBA::default_alpha = 200;

// default colors taken from d3 category10 color scheme
// https://github.com/d3/d3-3.x-api-reference/blob/master/Ordinal-Scales.md#category10
const std::array<const RGBA, 10> RGBA::defaults = {
    RGBA{31, 119, 180},  RGBA{255, 127, 14},  RGBA{44, 160, 44},
    RGBA{214, 39, 40},   RGBA{148, 103, 189}, RGBA{140, 86, 75},
    RGBA{227, 119, 194}, RGBA{127, 127, 127}, RGBA{188, 189, 34},
    RGBA{23, 190, 207}};

const RGBA RGBA::black {0, 0, 0};
const RGBA RGBA::white {255, 255, 255};

} // namespace trase
