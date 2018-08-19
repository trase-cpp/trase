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

//! [points example includes]
#include "trase.hpp"
#include <fstream>
#include <random>
//! [points example includes]

using namespace trase;

TEST_CASE("points example", "[points]") {
  /// \page line_example Example of using the points geometry
  ///  This is an example for the points geometry
  ///
  /// \snippet tests/TestPoints.cpp points example includes
  /// \snippet tests/TestPoints.cpp points example

  /// [points example]
  auto fig = figure();
  auto ax = fig->axis();
  const int n = 100;
  std::vector<float> x(n), y(n), r(n), c(n);
  std::default_random_engine gen;
  std::normal_distribution<float> normal(0, 1);
  std::generate(x.begin(), x.end(), [&]() { return normal(gen); });
  std::generate(y.begin(), y.end(), [&]() { return normal(gen); });
  std::generate(r.begin(), r.end(), [&]() { return normal(gen); });
  std::generate(c.begin(), c.end(), [&]() { return normal(gen); });

  auto data = create_data().x(x).y(y).size(r).color(c);

  auto points = ax->points(data);
  points->set_label("points");

  float time = 0.0;

  auto do_plot = [&]() {
    time += 0.3f;
    std::normal_distribution<float> normal(0, 1);
    std::generate(x.begin(), x.end(), [&]() { return normal(gen); });
    std::generate(y.begin(), y.end(), [&]() { return normal(gen); });
    std::generate(r.begin(), r.end(), [&]() { return normal(gen); });
    std::generate(c.begin(), c.end(), [&]() { return normal(gen); });
    auto data = create_data().x(x).y(y).size(r).color(c);
    points->add_frame(data, time);
  };

  for (int i = 0; i < 5; ++i) {
    do_plot();
  }

  ax->xlabel("x");
  ax->ylabel("y");
  ax->title("points test");
  ax->legend();

  // output to svg
  std::ofstream out;
  out.open("example_points.svg");
  BackendSVG backend(out);
  fig->draw(backend);
  out.close();
  /// [points example]
}

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
