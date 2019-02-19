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

#include "DummyDraw.hpp"

#include "trase.hpp"
#include <fstream>
#include <random>

using namespace trase;

TEST_CASE("rectangle creation", "[rectangle]") {
  auto fig = figure();
  auto ax = fig->axis();
  std::vector<float> xmin, ymin, xmax, ymax, c;
  ax->rectangle(create_data().xmin(xmin).ymin(ymin).xmax(xmax).ymax(ymax));
  DummyDraw::draw("rect_zero", fig);
  xmin.resize(1);
  ymin.resize(1);
  xmax.resize(1);
  ymax.resize(1);
  xmin[0] = 0.2f;
  ymin[0] = 0.8f;
  xmax[0] = 0.3f;
  ymax[0] = 0.9f;
  ax->rectangle(create_data().xmin(xmin).ymin(ymin).xmax(xmax).ymax(ymax));
  DummyDraw::draw("rect_one", fig);
  xmin.resize(5);
  ymin.resize(5);
  xmax.resize(5);
  ymax.resize(5);
  c.resize(5);
  for (int i = 0; i < 5; ++i) {
    xmin[i] = 0.1f * i;
    ymin[i] = 0.1f * i;
    xmax[i] = 0.12f * i;
    ymax[i] = 0.12f * i;
    c[i] = 0.1f * i;
  }
  ax->rectangle(
      create_data().xmin(xmin).ymin(ymin).xmax(xmax).ymax(ymax).color(c));
  DummyDraw::draw("rect_five", fig);
}

TEST_CASE("rectangle animate color", "[rectangle]") {
  auto fig = figure();
  auto ax = fig->axis();
  std::vector<float> xmin = {0};
  std::vector<float> ymin = {0};
  std::vector<float> xmax = {1};
  std::vector<float> ymax = {1};
  std::vector<float> c = {0};
  auto rect = ax->rectangle(
      create_data().xmin(xmin).ymin(ymin).xmax(xmax).ymax(ymax).color(c));
  c[0] = 1;
  rect->add_frame(
      create_data().xmin(xmin).ymin(ymin).xmax(xmax).ymax(ymax).color(c), 1);
  DummyDraw::draw("rect_animate_color", fig);
}

TEST_CASE("rectangle color frames exception", "[rectangle]") {
  auto fig = figure();
  auto ax = fig->axis();
  std::vector<float> xmin = {0};
  std::vector<float> ymin = {0};
  std::vector<float> xmax = {1};
  std::vector<float> ymax = {1};
  std::vector<float> c = {0};
  auto rect = ax->rectangle(
      create_data().xmin(xmin).ymin(ymin).xmax(xmax).ymax(ymax).color(c));
  rect->add_frame(create_data().xmin(xmin).ymin(ymin).xmax(xmax).ymax(ymax), 1);
  REQUIRE_THROWS_WITH(DummyDraw::draw("rect_color_exception", fig),
                      Catch::Contains("color"));
}

TEST_CASE("rectangle number frames exception", "[rectangle]") {
  auto fig = figure();
  auto ax = fig->axis();
  std::vector<float> xmin = {0};
  std::vector<float> ymin = {0};
  std::vector<float> xmax = {1};
  std::vector<float> ymax = {1};
  std::vector<float> c = {0};
  auto rect = ax->rectangle(
      create_data().xmin(xmin).ymin(ymin).xmax(xmax).ymax(ymax).color(c));
  xmin.push_back(0);
  ymin.push_back(0);
  xmax.push_back(1);
  ymax.push_back(1);
  c.push_back(0);
  rect->add_frame(
      create_data().xmin(xmin).ymin(ymin).xmax(xmax).ymax(ymax).color(c), 1);
  REQUIRE_THROWS_WITH(DummyDraw::draw("rect_number_exception", fig),
                      Catch::Contains("number"));
}
