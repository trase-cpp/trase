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

/// \file Style.hpp

#ifndef STYLE_H_
#define STYLE_H_

#include "util/Colors.hpp"
#include <string>

namespace trase {

class Style {

  /// the line width
  float m_line_width{3.f};

  /// the line style
  std::string m_line_style{"solid"};

  /// the font size
  float m_font_size{18.f};

  /// the color
  RGBA m_color{RGBA::black};

  /// the font
  std::string m_font{"Roboto"};

public:
  /// constructor
  Style() = default;

  /// get the current line width
  float line_width() const noexcept;

  /// get the current line width
  std::string line_style() const noexcept;

  /// get the current font size
  float font_size() const noexcept;

  /// get the current colour
  RGBA color() const noexcept;

  /// get the current font
  std::string font() const noexcept;

  /// set the new line width
  Style &line_width(float lineWidth) noexcept;

  /// set the new line style
  Style &line_style(std::string lineStyle) noexcept;

  /// set the new font size
  Style &font_size(float fontSize) noexcept;

  /// set the new color
  Style &color(RGBA color) noexcept;

  /// set the new font
  Style &font(std::string font) noexcept;
};

} // namespace trase

#endif // STYLE_H_
