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

/// \page example_animated_line Animated Line Geometry
///  This is an example for the line geometry, showing an animated plot
///
/// \image html https://cdn.jsdelivr.net/gh/trase-cpp/generated_files@master/examples/example_animated_line.svg "Output" 
/// \snippet examples/AnimatedLine.cpp animated line example

/// [animated line example]
#include "trase.hpp"
#include <fstream>

using namespace trase;

int main() {

  // create figure and axis
  auto fig = figure();
  auto ax = fig->axis();

  // set x steps, logistic parameter and create x and y vectors
  const int n = 100;
  const float dx = 12.f / static_cast<float>(n);
  const float k = 1.f;
  auto logistic = [k](const float x) { return 1.f / (1.f + std::exp(-k * x)); };
  std::vector<float> x = {-6};
  std::vector<float> y = {logistic(x.back())};

  // create the initial dataset and then plot it using a line geometry
  auto data = create_data().x(x).y(y);
  auto plt = ax->line(data);

  // loop through rest of the points, generating a new frame for every point
  for (int i = 1; i < n; ++i) {
    x.push_back(x.back() + dx);
    y.push_back(logistic(x.back()));
    auto data = create_data().x(x).y(y);
    plt->add_frame(data, (x.back() - x[0]) / 4.f);
  }

  // label axis
  ax->xlabel("x");
  ax->ylabel("y");

  // output to chosen backend
#ifdef TRASE_EXAMPLES_SVG_BACKEND
  std::ofstream out;
  out.open("example_animated_line.svg");
  BackendSVG backend(out);
  fig->draw(backend);
  out.close();
#endif
#ifdef TRASE_EXAMPLES_GL_BACKEND
  BackendGL backend;
  fig->show(backend);
#endif
}
/// [animated line example]
