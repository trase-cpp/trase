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

TEST_CASE("Lines creation", "[lines]") {
  // create figure and axis
  auto fig = figure();
  auto ax = fig->axis();

  // create x and y vectors
  const int n = 100;
  std::vector<float> x(n);
  std::vector<float> y(n);
  const float k = 1.f;
  auto logistic = [k](const float x) { return 1.f / (1.f + std::exp(-k * x)); };
  for (int i = 0; i < n; ++i) {
    x[i] = -6.f + 12.f * static_cast<float>(i) / n;
    y[i] = logistic(x[i]);
  }

  // create a trase dataset and then plot it using a line geometry
  auto data = create_data().x(x).y(y);
  auto plt = ax->line(data);

  // set line color = red and line width = 10
  plt->style().color(RGBA(255, 0, 0)).line_width(10).line_style(1.0);

  // label axis
  ax->xlabel("x");
  ax->ylabel("y");

  DummyDraw::draw("lines", fig);
}

TEST_CASE("Hybrids lines creation", "[lines]") {

  auto fig = figure();
  auto ax = fig->axis();

  {
    auto data = create_data().x(std::vector<float>{0, 1}).y(std::vector<float>{0, 1});
    auto plt = ax->line(data);
    plt->style().color(RGBA(255, 0, 0)).line_width(5).line_style(0.0);
  }

  {
    auto data = create_data().x(std::vector<float>{0, 1}).y(std::vector<float>{0.5, 1.5});
    auto plt = ax->line(data);
    plt->style().color(RGBA(255, 0, 0)).line_width(5).line_style(2.0);
  }

  {
    auto data = create_data().x(std::vector<float>{0, 1}).y(std::vector<float>{-0.5, 0.5});
    auto plt = ax->line(data);
    plt->style().color(RGBA(255, 0, 0)).line_width(5).line_style(2.0);
  }

  ax->xlabel("Pred. Target");
  ax->ylabel("Exp. Target");

  DummyDraw::draw("lines", fig);
}
