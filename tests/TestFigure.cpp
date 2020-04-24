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
#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <limits>
#include <type_traits>

#include "DummyDraw.hpp"
#include "trase.hpp"

using namespace trase;

TEST_CASE("check figure can be created", "[figure]") {
  Figure fig0({600, 300});
  auto fig1 = figure();
  auto fig2 = figure({800, 600});
}

TEST_CASE("check show throws", "[figure]") {
  auto fig = figure();
  std::ofstream out;
  out.open("test_show_exception_trase_invalid_svg.svg");
  BackendSVG backend(out);
  REQUIRE_THROWS_WITH(fig->show(backend), Catch::Contains("Figure::show()"));
}

TEST_CASE("adding a single Axis object to Figure", "[figure]") {
  auto fig = figure();

  auto ax = fig->axis();
  const std::vector<float> x = {1.f, 2.f, 3.f};
  const std::vector<float> y = {1.f, 2.f, 3.f};
  auto data = create_data().x(x).y(y);
  auto line = ax->line(data);

  CHECK(fig->axis() == ax);
  CHECK(fig->axis(0, 0) == ax);
  DummyDraw::draw("figure", fig);

  CHECK_THROWS_AS(fig->axis(-1, 1), std::out_of_range);
  CHECK_THROWS_AS(fig->axis(1, -1), std::out_of_range);
  CHECK_THROWS_AS(fig->axis(-1, -1), std::out_of_range);
}

TEST_CASE("adding various Axis objects to Figure", "[figure]") {
  auto fig = figure();

  auto ax = fig->axis();
  const std::vector<float> x = {1.f, 2.f, 3.f};
  const std::vector<float> y = {1.f, 2.f, 3.f};
  auto data = create_data().x(x).y(y);
  auto line = ax->line(data);

  CHECK(fig->axis() == ax);
  CHECK(fig->axis(0, 0) == ax);

  auto ax2 = fig->axis(1, 1);
  CHECK(ax2 != ax);
  const std::vector<float> y2 = {3.f, 2.f, 1.f};
  auto data2 = create_data().x(x).y(y2);
  ax2->points(data2);
  DummyDraw::draw("figure", fig);

  auto ax3 = fig->axis(0, 1);
  CHECK(ax3 != ax);
  CHECK(ax3 != ax2);
  ax3->line(data2);
  DummyDraw::draw("figure", fig);

  auto ax4 = fig->axis(0, 5);
  CHECK(ax4 != ax);
  CHECK(ax4 != ax2);
  CHECK(ax4 != ax3);
  ax4->points(data);
  DummyDraw::draw("figure", fig);
}

TEST_CASE("readme", "[figure]") {
  auto fig = figure();
  auto ax = fig->axis();
  const int n = 100;
  const int nframes = 10;
  std::vector<float> x(n);
  std::vector<float> y(n);

  // define x points
  for (int i = 0; i < n; ++i) {
    x[i] = static_cast<float>(i) / n;
  }

  // define y = sin(x) with given amplitude and frequency
  auto get_ysinx = [&](const float amplitude, const float freq) {
    for (int i = 0; i < n; ++i) {
      y[i] = amplitude * std::sin(6.28f * freq * x[i]);
    }
    return create_data().x(x).y(y);
  };

  // create a static sin(x) function
  auto static_plot = ax->line(get_ysinx(1.f, 2.f));
  static_plot->set_label("static");

  // create a moving sin(x) function with varying amplitude
  auto moving_plot = ax->line(get_ysinx(1.f, 5.f));
  moving_plot->set_label("moving");

  for (int i = 1; i <= nframes; ++i) {
    const float nf = static_cast<float>(nframes);
    const float amplitude = 1.f - 0.5f * std::sin(6.28f * i / nf);
    moving_plot->add_frame(get_ysinx(amplitude, 5.f), 3.f * i / nf);
  }

  // set label axes
  ax->xlabel("x");
  ax->ylabel("y");
  ax->title("the svg test");
  ax->legend();

  // output to svg
  std::ofstream out;
  out.open("readme.svg");
  BackendSVG backend(out);
  fig->draw(backend);
  out.close();
}
