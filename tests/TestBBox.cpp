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

#include <sstream>

#include "util/BBox.hpp"

TEST_CASE("bbox constructors", "[bbox]") {

  using vec3int = trase::Vector<int, 3>;
  const int int_max = std::numeric_limits<int>::max();
  const int epsilon = std::numeric_limits<int>::epsilon();

  vec3int lower = {1, 2, 3};
  vec3int upper = {11, 22, 33};

  trase::bbox<int, 3> no_args{};
  CHECK((no_args.bmin == vec3int::Constant(int_max)).all());
  CHECK((no_args.bmax == vec3int::Constant(-int_max)).all());

  trase::bbox<int, 3> one_arg{lower};
  CHECK((one_arg.bmin == lower).all());
  CHECK((one_arg.bmax == lower + vec3int::Constant(epsilon)).all());

  trase::bbox<int, 3> two_args{lower, upper};
  CHECK((two_args.bmin == lower).all());
  CHECK((two_args.bmax == upper).all());
}

TEST_CASE("bbox conversion to_coords", "[bbox]") {

  using vec2f = trase::Vector<float, 2>;
  const float epsilon = std::numeric_limits<float>::epsilon();

  vec2f pix_min = {10.f, 15.f};
  vec2f pix_max = {75.f, 70.f};
  trase::bbox<float, 2> pixels = {pix_min, pix_max};

  vec2f lim_min = {0.f, 0.f};
  vec2f lim_max = {1.f, 1.f};
  trase::bbox<float, 2> limits = {lim_min, lim_max};

  // points in the limits coordinate system
  vec2f p_lim_random = {.25f, .3f};
  vec2f p_lim_bot_lt = {0.f, 0.f};
  vec2f p_lim_bot_rt = {1.f, 0.f};
  vec2f p_lim_top_lt = {0.f, 1.f};
  vec2f p_lim_top_rt = {1.f, 1.f};

  // corresponding points in the pixels coordinate system
  vec2f p_pix_random = {26.25f, 53.5f};
  vec2f p_pix_bot_lt = {10.f, 70.f};
  vec2f p_pix_bot_rt = {75.f, 70.f};
  vec2f p_pix_top_lt = {10.f, 15.f};
  vec2f p_pix_top_rt = {75.f, 15.f};

  // Check transforms to pixels from limits
  CHECK(trase::approx_equal(limits.to_coords(pixels, p_lim_random),
                            p_pix_random, epsilon)
            .all());
  CHECK(trase::approx_equal(limits.to_coords(pixels, p_lim_bot_lt),
                            p_pix_bot_lt, epsilon)
            .all());
  CHECK(trase::approx_equal(limits.to_coords(pixels, p_lim_bot_rt),
                            p_pix_bot_rt, epsilon)
            .all());
  CHECK(trase::approx_equal(limits.to_coords(pixels, p_lim_top_lt),
                            p_pix_top_lt, epsilon)
            .all());
  CHECK(trase::approx_equal(limits.to_coords(pixels, p_lim_top_rt),
                            p_pix_top_rt, epsilon)
            .all());

  // Check transforms to limits from pixels
  CHECK(trase::approx_equal(pixels.to_coords(limits, p_pix_random),
                            p_lim_random, epsilon)
            .all());
  CHECK(trase::approx_equal(pixels.to_coords(limits, p_pix_bot_lt),
                            p_lim_bot_lt, epsilon)
            .all());
  CHECK(trase::approx_equal(pixels.to_coords(limits, p_pix_bot_rt),
                            p_lim_bot_rt, epsilon)
            .all());
  CHECK(trase::approx_equal(pixels.to_coords(limits, p_pix_top_lt),
                            p_lim_top_lt, epsilon)
            .all());
  CHECK(trase::approx_equal(pixels.to_coords(limits, p_pix_top_rt),
                            p_lim_top_rt, epsilon)
            .all());
}
