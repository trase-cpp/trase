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

#include <random>
#include <type_traits>

#include "trase.hpp"

using namespace trase;

// This tests the output of the `get_nth_prime` function
TEST_CASE("interactive test (only run by a human)", "[interactive]") {

  auto fig = figure();
  auto ax = fig->axis();
  const int n = 100;
  std::vector<float> x(n);
  std::vector<float> y(n);
  std::vector<float> r(n);
  for (int i = 0; i < n; ++i) {
    x[i] = static_cast<float>(i) * 6.28 / n;
    y[i] = std::sin(5 * x[i]);
    r[i] = 0.1;
  }
  auto data = create_data().x(x).y(y).color(r).size(r);
  auto static_plot = ax->line(data);
  static_plot->set_label("static");
  auto moving_plot = ax->line(data);
  moving_plot->set_label("moving");

  auto points = ax->points(data);
  points->set_label("points");

  float time = 0.0;

  auto do_plot = [&](const float theta) {
    time += 0.3;
    for (int i = 0; i < n; ++i) {
      y[i] = std::sin(theta * x[i]);
      r[i] = time * 0.1;
    }
    auto data = create_data().x(x).y(y).color(r).size(r);
    moving_plot->add_frame(data, time);
    points->add_frame(data, time);
  };

  for (int i = 0; i < 5; ++i) {
    const float theta = 5 - i;
    do_plot(theta);
  }
  for (int i = 4; i >= 0; --i) {
    const float theta = 5 - i;
    do_plot(theta);
  }

  ax->xlabel("x");
  ax->ylabel("y");
  ax->title("the interactive test");
  ax->legend();

  BackendGL backend;
  fig->show(backend);
}

TEST_CASE("histogram", "[interactive]") {

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
    time += 0.3;
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

  BackendGL backend;
  fig->show(backend);
}
