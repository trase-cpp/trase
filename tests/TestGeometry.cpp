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

// This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"

#include <limits>
#include <type_traits>

#include "trase.hpp"

using namespace trase;

TEST_CASE("line geometry can be created", "[geometry]") {
  auto fig = figure();
  auto ax = fig->axis();
  auto pl1 = ax->line(create_data()
                          .x(std::vector<float>({0.0f, 0.1f, 0.5f}))
                          .y(std::vector<float>({0.0f, 0.1f, 0.5f})));
  auto pl2 = ax->line(create_data()
                          .x(std::vector<int>({0, 1, 2}))
                          .y(std::vector<float>({0.0f, 0.1f, 0.5f})));
  auto pl3 = ax->line(create_data()
                          .x(std::vector<int>({0, 1, 2}))
                          .y(std::vector<int>({0, 2, 4})));

  // auto pl4 = ax->plot({0, 1, 2}, {0, 2, 4});
  //
  REQUIRE_THROWS_WITH(
      ax->line(create_data()
                   .x(std::vector<float>({0.0f, 0.1f}))
                   .y(std::vector<float>({0.0f, 0.1f, 0.5f}))),
      Catch::Contains("columns in dataset must have identical number of rows"));

  std::vector<float> x = {0.0f, 0.1f, 0.5f};
  std::vector<float> y = {0.0f, 0.1f, 0.5f};
  auto data = create_data().x(x).y(y);
  auto pl5 = ax->line(data);
}
