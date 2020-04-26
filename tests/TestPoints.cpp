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

TEST_CASE("points creation", "[points]") {
  auto fig = figure();
  auto ax = fig->axis();
  std::vector<float> x, y, r, c;
  ax->points(create_data().x(x).y(y));
  DummyDraw::draw("points", fig);
  x.resize(1);
  y.resize(1);
  r.resize(1);
  x[0] = 0.1f;
  y[0] = 0.2f;
  r[0] = 1.0f;
  ax->points(create_data().x(x).y(y).size(r));
  DummyDraw::draw("points", fig);
  x.resize(5);
  y.resize(5);
  r.resize(5);
  c.resize(5);
  for (int i = 0; i < 5; ++i) {
    x[1] = 0.1f * i;
    y[1] = 0.1f * i;
    r[1] = 0.1f * i;
    c[1] = 0.1f * i;
  }
  ax->points(create_data().x(x).y(y).size(r).color(c));
  DummyDraw::draw("points", fig);
}

TEST_CASE("points legend", "[points]") {
  auto fig = figure();
  auto ax = fig->axis();
  {
    std::vector<float> x = {0.f, 1.f, 2.f, 3.f};
    std::vector<float> y = {0.f, 0.f, 0.f, 0.f};
    std::vector<float> r = {1.f, 2.f, 3.f, 4.f};
    std::vector<float> c = {0.f, 1.f, 2.f, 3.f};
    auto pts = ax->points(create_data().x(x).y(y).size(r).color(c));
    pts->set_label("y=0");
  }
  {
    std::vector<float> x = {0.f, 1.f, 2.f, 3.f};
    std::vector<float> y = {1.f, 1.f, 1.f, 1.f};
    std::vector<float> r = {1.f, 2.f, 3.f, 4.f};
    auto pts = ax->points(create_data().x(x).y(y).size(r));
    pts->set_label("y=1");
  }
  {
    std::vector<float> x = {0.f, 1.f, 2.f, 3.f};
    std::vector<float> y = {2.f, 2.f, 2.f, 2.f};
    auto pts = ax->points(create_data().x(x).y(y));
    pts->set_label("y=2");
  }
  ax->legend();
  DummyDraw::draw("points_legend", fig);
}

TEST_CASE("points animate color", "[points]") {
  auto fig = figure();
  auto ax = fig->axis();
  std::vector<float> x = {0.f};
  std::vector<float> y = {0.f};
  std::vector<float> r = {1.f};
  std::vector<float> c = {0.f};
  auto pts = ax->points(create_data().x(x).y(y).size(r).color(c));
  c[0] = 1;
  pts->add_frame(create_data().x(x).y(y).size(r).color(c), 1);
  DummyDraw::draw("points_animate_color", fig);
}

TEST_CASE("points color frames exception", "[points]") {
  auto fig = figure();
  auto ax = fig->axis();
  std::vector<float> x = {0.f};
  std::vector<float> y = {0.f};
  std::vector<float> r = {1.f};
  std::vector<float> c = {0.f};
  auto pts = ax->points(create_data().x(x).y(y).size(r).color(c));
  pts->add_frame(create_data().x(x).y(y).size(r), 1);

  REQUIRE_THROWS_WITH(
      DummyDraw::draw("points_color_exception_trase_invalid_svg", fig),
      Catch::Contains("color"));
}

TEST_CASE("points size frames exception", "[points]") {
  auto fig = figure();
  auto ax = fig->axis();
  std::vector<float> x = {0.f};
  std::vector<float> y = {0.f};
  std::vector<float> r = {1.f};
  std::vector<float> c = {0.f};
  auto pts = ax->points(create_data().x(x).y(y).size(r).color(c));
  pts->add_frame(create_data().x(x).y(y).color(c), 1);

  REQUIRE_THROWS_WITH(
      DummyDraw::draw("points_size_exception_trase_invalid_svg", fig),
      Catch::Contains("size"));
}

TEST_CASE("points number frames exception", "[points]") {
  auto fig = figure();
  auto ax = fig->axis();
  std::vector<float> x = {0.f};
  std::vector<float> y = {0.f};
  std::vector<float> r = {1.f};
  std::vector<float> c = {0.f};
  auto pts = ax->points(create_data().x(x).y(y).size(r).color(c));
  x.push_back(0.f);
  y.push_back(0.f);
  r.push_back(0.f);
  c.push_back(0.f);
  pts->add_frame(create_data().x(x).y(y).size(r).color(c), 1);
  REQUIRE_THROWS_WITH(
      DummyDraw::draw("points_number_exception_trase_invalid_svg", fig),
      Catch::Contains("number"));
}
