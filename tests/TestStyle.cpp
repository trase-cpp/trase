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

#include "catch.hpp"

#include "util/Colors.hpp"
#include "util/Style.hpp"

#include "trase.hpp"

TEST_CASE("style construction and defaults", "[style]") {

  trase::Style s{};

  CHECK(s.line_width() == 3.f);
  CHECK(s.font_size() == 18.f);
  CHECK(s.font() == "Roboto");
  CHECK(s.color() == trase::RGBA::black);
}

TEST_CASE("style on drawable", "[style]") {

  auto fig = trase::figure({800, 600});
  auto ax = fig->axis();

  const trase::Style &s = ax->style();

  // Create a new style
  trase::Style my_style;
  CHECK(s.line_width() == my_style.line_width());

  // Change line width in my_style and set it as new style on the axis
  my_style.line_width(1.23f);
  ax->style() = my_style;
  CHECK(s.line_width() == 1.23f);
}

TEST_CASE("style setting and getting", "[style]") {

  trase::Style my_style;

  trase::RGBA my_col{1, 2, 3, 4};

  my_style.line_width(2.3f).font_size(3.4f).color(my_col).font("a string");

  CHECK(my_style.line_width() == 2.3f);
  CHECK(my_style.font_size() == 3.4f);
  CHECK(my_style.color() == my_col);
  CHECK(my_style.font() == "a string");
}
