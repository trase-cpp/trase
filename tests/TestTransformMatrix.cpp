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

#include <iterator>
#include <type_traits>

#include "backend/Backend.hpp"

TEST_CASE("constructors", "[transform matrix]") {

  // Default construction
  trase::TransformMatrix t;

  // check identity
  CHECK(t.is_identity() == true);

  /// [a c e]   [1 0 0]
  /// [b d f] = [0 1 0]
  /// [0 0 1]   [0 0 1]

  CHECK(t.a == 1.f);
  CHECK(t.b == 0.f);
  CHECK(t.c == 0.f);
  CHECK(t.d == 1.f);
  CHECK(t.e == 0.f);
  CHECK(t.f == 0.f);
}

TEST_CASE("to_string", "[transform matrix]") {

  // Default construction
  trase::TransformMatrix t;

  CHECK(t.to_string() == "transform=\"matrix(1 0 0 1 0 0)\"");
}

TEST_CASE("translations", "[transform matrix]") {

  trase::TransformMatrix t;

  /// [a c e]   [1 0 0]
  /// [b d f] = [0 1 1]
  /// [0 0 1]   [0 0 1]
  t.translate({0.f, 1.f});

  CHECK(t.a == 1.f);
  CHECK(t.b == 0.f);
  CHECK(t.c == 0.f);
  CHECK(t.d == 1.f);
  CHECK(t.e == 0.f);
  CHECK(t.f == 1.f);

  /// [a c e]   [1 0 1]
  /// [b d f] = [0 1 1]
  /// [0 0 1]   [0 0 1]
  t.translate({1.f, 0.f});

  CHECK(t.a == 1.f);
  CHECK(t.b == 0.f);
  CHECK(t.c == 0.f);
  CHECK(t.d == 1.f);
  CHECK(t.e == 1.f);
  CHECK(t.f == 1.f);
}

TEST_CASE("rotations", "[transform matrix]") {

  trase::TransformMatrix t;

  /// [a c e]   [-1 0 0]
  /// [b d f] = [0 -1 0]
  /// [0 0 1]   [0 0  1]
  t.rotate(trase::pi);

  CHECK(t.a == Approx(-1.f));
  CHECK(t.b == Approx(0.0f).margin(1e-7));
  CHECK(t.c == Approx(0.0f).margin(1e-7));
  CHECK(t.d == Approx(-1.f));
  CHECK(t.e == Approx(0.f));
  CHECK(t.f == Approx(0.f));

  /// [a c e]   [1 0 0]
  /// [b d f] = [0 1 0]
  /// [0 0 1]   [0 0 1]
  t.rotate(-trase::pi);

  CHECK(t.a == Approx(1.f));
  CHECK(t.b == Approx(0.f));
  CHECK(t.c == Approx(0.f));
  CHECK(t.d == Approx(1.f));
  CHECK(t.e == Approx(0.f));
  CHECK(t.f == Approx(0.f));
}
