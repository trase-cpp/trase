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

namespace trase {

std::string strip_whitespace(const std::string &arg) {
  std::string ret(arg);
  ret.erase(std::remove_if(ret.begin(), ret.end(),
                           [](unsigned char x) { return std::isspace(x); }),
            ret.end());
  return ret;
}

bool compare_ignoring_whitespace(const std::string &a, const std::string &b) {
  return strip_whitespace(a) == strip_whitespace(b);
}

} // namespace trase

using namespace trase;

TEST_CASE("figure can written using SVG backend", "[figure]") {
  auto fig = figure();
  auto ax = fig->axis();
  const int n = 100;
  const int nframes = 10;
  std::vector<float> x(n);
  std::vector<float> y(n);
  std::vector<float> r(n);

  // define x points
  for (int i = 0; i < n; ++i) {
    x[i] = static_cast<float>(i) / n;
  }

  // define y = sin(x) with given amplitude and frequency
  auto write_y = [&](const float amplitude, const float freq) {
    for (int i = 0; i < n; ++i) {
      y[i] = amplitude * std::sin(6.28f * freq * x[i]);
      r[i] = static_cast<float>(i) / n * amplitude;
    }
  };

  // create a static sin(x) function
  write_y(1.f, 2.f);
  auto static_plot = ax->plot(x, y, "static");

  // create a moving sin(x) function with varying amplitude
  write_y(1.f, 5.f);
  auto moving_plot = ax->plot(x, y, "moving");

  auto data = std::make_shared<DataWithAesthetic>();
  data->x(x).y(y).color(r).size(r);
  auto points = ax->points(data, "points");

  for (int i = 1; i <= nframes; ++i) {
    const float nf = static_cast<float>(nframes);
    const float amplitude = 1.f - 0.5f * std::sin(6.28f * i / nf);
    write_y(amplitude, 5.f);
    moving_plot->add_frame(x, y, 3.f * i / nf);

    auto data = std::make_shared<DataWithAesthetic>();
    data->x(x).y(y).color(r).size(r);
    points->add_frame(data, 3.f * i / nf);
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

TEST_CASE("svg backend init and finalise work as expected", "[svg_backend]") {

  std::stringstream out_ss;
  BackendSVG backend(out_ss);

  REQUIRE(out_ss.str().empty());

  SECTION("init produces correct string") {

    backend.init(123.4f, 234.5f, 345.6f, "r@ndom^name");

    const std::string expected_str =
        R"del(<?xml version="1.0" encoding="utf-8" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN"
  "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<svg width="123.4px" height="234.5px" version="1.1" xmlns="http://www.w3.org/2000/svg">
<desc>r@ndom^name</desc>
<script>
function tooltip(x,y,string,size,face) {
    var txtElem = document.createElementNS("http://www.w3.org/2000/svg", "text");
    txtElem.setAttributeNS(null,"id","tooltip");
    txtElem.setAttributeNS(null,"x",x);
    txtElem.setAttributeNS(null,"y",y);
    txtElem.setAttributeNS(null,"font-size",size);
    txtElem.setAttributeNS(null,"font-family",face);

    txtElem.appendChild(document.createTextNode(string))
    document.documentElement.appendChild(txtElem);
}
function remove_tooltip() {
    var txtElem = document.getElementById("tooltip");
    document.documentElement.removeChild(txtElem);
}
</script>
)del";

    CHECK(compare_ignoring_whitespace(out_ss.str(), expected_str));
  }

  SECTION("finalise produces correct string") {

    backend.finalise();
    CHECK(compare_ignoring_whitespace(out_ss.str(), "</svg>\n"));
  }

  SECTION("forms valid svg") {

    backend.init(123.4f, 234.5f, 345.6f, "r@ndom^name");
    backend.finalise();

    std::ofstream out_f;
    out_f.open("backend_svg_init_finalise.svg");

    out_f << out_ss.str();
    out_f.close();
  }
}
