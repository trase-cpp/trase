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

//! [histogram example includes]
#include "trase.hpp"
#include <fstream>
#include <random>
//! [histogram example includes]

using namespace trase;

TEST_CASE("histogram example", "[histogram]") {
  /// \page line_example Example of using the histogram geometry
  ///  This is an example for the histogram geometry
  ///
  /// \snippet tests/TestHistogram.cpp histogram example includes
  /// \snippet tests/TestHistogram.cpp histogram example

  /// [histogram example]
  auto fig = figure();
  auto ax = fig->axis();
  const int n = 100;
  std::vector<float> x(n);
  std::default_random_engine gen;
  std::normal_distribution<float> normal(0, 1);
  std::generate(x.begin(), x.end(), [&]() { return normal(gen); });

  auto data = create_data().x(x);

  auto hist = ax->histogram(data);
  hist->set_label("hist");

  float time = 0.0;

  auto do_plot = [&](const float theta) {
    time += 0.3f;
    std::normal_distribution<float> normal(theta, 1);
    std::generate(x.begin(), x.end(), [&]() { return normal(gen); });
    auto data = create_data().x(x);
    hist->add_frame(data, time);
  };

  for (int i = 0; i < 5; ++i) {
    const float theta = i / 5.f;
    do_plot(theta);
  }

  ax->xlabel("x");
  ax->ylabel("y");
  ax->title("histogram test");
  ax->legend();

  // output to svg
  std::ofstream out;
  out.open("example_histogram.svg");
  BackendSVG backend(out);
  fig->draw(backend);
  out.close();
  /// [histogram example]
}

TEST_CASE("histogram creation", "[histogram]") {
  auto fig = figure();
  auto ax = fig->axis();
  std::vector<float> x;
  ax->histogram(create_data().x(x));
  DummyDraw::draw("histogram", fig);
  x.resize(1);
  x[0] = 0.1f;
  ax->histogram(create_data().x(x));
  DummyDraw::draw("histogram", fig);
  x.resize(5);
  x[1] = 0.2f;
  x[2] = 0.3f;
  x[3] = 0.4f;
  x[4] = 0.5f;
  ax->histogram(create_data().x(x));
  DummyDraw::draw("histogram", fig);
}
