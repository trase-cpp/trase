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

using namespace trase;

TEST_CASE("legend creation", "[legend]") {
  auto fig = figure();
  auto ax = fig->axis();
  const int n = 100;
  std::vector<float> x(n);
  std::vector<float> y(n);
  for (int i = 0; i < n; ++i) {
    x[i] = 6.28f * static_cast<float>(i) / n;
    y[i] = (n / 2) * std::sin(x[i]);
  }
  auto data = create_data().x(x).y(y);
  auto geom = ax->histogram(data);
  geom->set_label("histogram");
  DummyDraw::draw("legend", fig);
  ax->legend();
  DummyDraw::draw("legend", fig);
  geom = ax->line(data);
  geom->set_label("line");
  DummyDraw::draw("legend", fig);
  geom = ax->points(data);
  geom->set_label("points");
  DummyDraw::draw("legend", fig);
}

TEST_CASE("animated points", "[legend]") {
  auto fig = figure();
  auto ax = fig->axis();
  const int n = 100;
  std::vector<float> x(n);
  std::vector<float> y(n);
  std::vector<float> c(n);

  std::vector<DataWithAesthetic> datas(5);
  for (int i = 0; i < 5; ++i) {
    for (int j = 0; j < n; ++j) {
      x[j] = 6.28f * static_cast<float>(j) / n;
      y[j] = (n / 2) * std::sin(x[j]);
      c[j] = (n / 2) * std::sin(x[j] + i * 6.28f / 5.f);
    }
    datas[i].x(x).y(y).color(c);
  }

  auto geom = ax->points(datas[0]);
  for (int i = 1; i < 5; ++i) {
    geom->add_frame(datas[i], static_cast<float>(i) / 5.f);
  }
  geom->set_label("points");
  ax->legend();
  DummyDraw::draw("legend_animated_points", fig);
}
