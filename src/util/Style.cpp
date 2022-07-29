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

#include "util/Style.hpp"

namespace trase {

float Style::line_width() const noexcept { return m_line_width; }

std::string Style::line_style() const noexcept {
  if (m_line_style == "solid")
    return "0";
  else if (m_line_style == "dashed")
    return "10,5";
  else if (m_line_style == "dotted")
    return "10,3,2";
  else
    return "0";
}

float Style::font_size() const noexcept { return m_font_size; }

RGBA Style::color() const noexcept { return m_color; }

std::string Style::font() const noexcept { return m_font; }

Style &Style::line_width(const float lineWidth) noexcept {
  m_line_width = lineWidth;
  return *this;
}

Style &Style::line_style(const std::string lineStyle) noexcept {
  m_line_style = lineStyle;
  return *this;
}

Style &Style::font_size(const float fontSize) noexcept {
  m_font_size = fontSize;
  return *this;
}

Style &Style::color(const RGBA color) noexcept {
  m_color = color;
  return *this;
}

Style &Style::font(const std::string font) noexcept {
  m_font = font;
  return *this;
}

} // namespace trase
