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

#include <limits>
#include <type_traits>

#include "trase.hpp"

using namespace trase;

TEST_CASE("axis can be created", "[axis]") {
  auto fig = figure({800, 600});

  Axis ax(*fig, bfloat2_t({0.1f, 0.1f}, {0.9f, 0.9f}));
}

TEST_CASE("check limit setting", "[axis]") {
  auto fig = figure();
  auto ax = fig->axis();
  CHECK(ax->limits().is_empty() == true);
  ax->xlim({0.0f, 1.0f});
  CHECK(ax->limits().is_empty() == true);
  CHECK(ax->limits().bmin[0] == 0.0f);
  CHECK(ax->limits().bmax[0] == 1.0f);
  ax->ylim({-1.0f, 1.0f});
  CHECK(ax->limits().is_empty() == false);
  CHECK(ax->limits().bmin[1] == -1.0f);
  CHECK(ax->limits().bmax[1] == 1.0f);
}

TEST_CASE("to/from pixel coordinates", "[axis]") {
  // axis will be 80 x 80 pixels from 10 - 90 each dim
  auto fig = figure({100, 100});
  auto ax = fig->axis();
  CHECK(ax->pixels().bmin[0] == 10.f);
  CHECK(ax->pixels().bmin[1] == 10.f);
  CHECK(ax->pixels().bmax[0] == 90.f);
  CHECK(ax->pixels().bmax[1] == 90.f);
  ax->xlim({0.0f, 1.0f});
  ax->ylim({0.0f, 1.0f});
  auto middle1 = ax->to_pixel({0.5f, 0.5f});
  CHECK(middle1[0] == 50.f);
  CHECK(middle1[1] == 50.f);
  auto middle2 = ax->from_pixel(middle1);
  CHECK(middle2[0] == 0.5f);
  CHECK(middle2[1] == 0.5f);
}
