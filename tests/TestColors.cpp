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

TEST_CASE("color construction", "[colors]") {

  trase::RGBA b{1,2,3};
  trase::RGBA c{1,2,3, trase::RGBA::default_alpha};

  CHECK(b == c);
}

TEST_CASE("color setters and getters", "[colors]") {

  trase::RGBA my_col{4, 5, 6, 7};

  CHECK(my_col.r() == 4);
  CHECK(my_col.g() == 5);
  CHECK(my_col.b() == 6);
  CHECK(my_col.a() == 7);

  my_col.r(12).g(13).b(15).a(21);

  CHECK(my_col.r() == 12);
  CHECK(my_col.g() == 13);
  CHECK(my_col.b() == 15);
  CHECK(my_col.a() == 21);
}

TEST_CASE("color to string", "[colors]") {

  CHECK(trase::RGBA::black.to_rgb_string() == "#000000");
  CHECK(trase::RGBA::white.to_rgb_string() == "#ffffff");
}

TEST_CASE("color equality operators", "[colors]") {

  CHECK(trase::RGBA::black == trase::RGBA::black);
  CHECK(trase::RGBA::white == trase::RGBA::white);

  CHECK(trase::RGBA::black != trase::RGBA::white);
  CHECK(trase::RGBA::white != trase::RGBA::black);

  CHECK(!(trase::RGBA::black == trase::RGBA::white));
  CHECK(!(trase::RGBA::white == trase::RGBA::black));
}

TEST_CASE("color statics", "[colors]") {

  CHECK(trase::RGBA::default_alpha == 200);

  trase::RGBA black = trase::RGBA::black;
  CHECK(black.r() == 0);
  CHECK(black.g() == 0);
  CHECK(black.b() == 0);
  CHECK(black.a() == trase::RGBA::default_alpha);

  trase::RGBA white = trase::RGBA::white;
  CHECK(white.r() == 255);
  CHECK(white.g() == 255);
  CHECK(white.b() == 255);
  CHECK(white.a() == trase::RGBA::default_alpha);

  CHECK(trase::RGBA::defaults.at(0) == trase::RGBA{31, 119, 180});
  CHECK(trase::RGBA::defaults.at(1) == trase::RGBA{255, 127, 14});
  CHECK(trase::RGBA::defaults.at(2) == trase::RGBA{44, 160, 44});
  CHECK(trase::RGBA::defaults.at(3) == trase::RGBA{214, 39, 40});
  CHECK(trase::RGBA::defaults.at(4) == trase::RGBA{148, 103, 189});
  CHECK(trase::RGBA::defaults.at(5) == trase::RGBA{140, 86, 75});
  CHECK(trase::RGBA::defaults.at(6) == trase::RGBA{227, 119, 194});
  CHECK(trase::RGBA::defaults.at(7) == trase::RGBA{127, 127, 127});
  CHECK(trase::RGBA::defaults.at(8) == trase::RGBA{188, 189, 34});
  CHECK(trase::RGBA::defaults.at(9) == trase::RGBA{23, 190, 207});
}
