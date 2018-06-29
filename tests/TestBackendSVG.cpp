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

#include <fstream>

#include "backend/BackendSVG.hpp"
#include "trase.hpp"

using namespace trase;

TEST_CASE("figure can written using SVG backend", "[figure]") {
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
  auto write_y = [&](const float amplitude, const float freq) {
    for (int i = 0; i < n; ++i) {
      y[i] = amplitude * std::sin(6.28f * freq * x[i]);
    }
  };

  // create a static sin(x) function
  write_y(1.f, 2.f);
  auto static_plot = ax->plot(x, y, "static");

  // create a moving sin(x) function with varying amplitude
  write_y(1.f, 5.f);
  auto moving_plot = ax->plot(x, y, "moving");

  for (int i = 1; i <= nframes; ++i) {
    const float nf = static_cast<float>(nframes);
    const float amplitude = 1.f - 0.5f * std::sin(6.28f * i / nf);
    write_y(amplitude, 5.f);
    moving_plot->add_frame(x, y, 3.f * i / nf);
  }

  // set label axes
  ax->xlabel("x");
  ax->ylabel("y");
  ax->title("the svg test");
  ax->legend();

  // output to svg
  std::ofstream out;
  out.open("test_figure.svg");
  BackendSVG backend(out);
  fig->serialise(backend);
  out.close();
}
