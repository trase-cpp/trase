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

#include <cctype>
#include <fstream>
#include <random>

#include "backend/BackendSVG.hpp"
#include "trase.hpp"

namespace trase {

std::string strip_ws(const std::string &arg) {
  std::string ret(arg);
  ret.erase(std::remove_if(ret.begin(), ret.end(),
                           [](unsigned char x) { return std::isspace(x); }),
            ret.end());
  return ret;
}

bool compare_ignoring_ws(const std::string &a, const std::string &b) {
  return strip_ws(a) == strip_ws(b);
}

bool is_substr_ignoring_ws(const std::string &str, const std::string &substr) {
  return strip_ws(str).find(strip_ws(substr)) != std::string::npos;
}

bool starts_with_ignoring_ws(const std::string &str,
                             const std::string &substr) {
  return strip_ws(str).find(strip_ws(substr)) == 0;
}

bool ends_with_ignoring_ws(const std::string &str, const std::string &substr) {
  std::string str_s = strip_ws(str);
  std::string substr_s = strip_ws(substr);
  if (str_s.length() >= substr_s.length()) {
    return str_s.compare(str_s.length() - substr_s.length(), substr_s.length(),
                         substr_s) == 0;
  }
  return false;
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

  // create data

  // create a static sin(x) function
  write_y(1.f, 2.f);
  auto data = create_data().x(x).y(y);
  auto static_plot = ax->line(data);
  static_plot->set_label("static");

  // create a moving sin(x) function with varying amplitude
  write_y(1.f, 5.f);
  data = create_data().x(x).y(y).color(r).size(r);
  auto moving_plot = ax->line(data);
  moving_plot->set_label("moving");

  auto points = ax->points(data);
  points->set_label("points");

  for (int i = 1; i <= nframes; ++i) {
    const float nf = static_cast<float>(nframes);
    const float amplitude = 1.f - 0.5f * std::sin(6.28f * i / nf);
    write_y(amplitude, 5.f);
    auto data = create_data().x(x).y(y).color(r).size(r);
    moving_plot->add_frame(data, 3.f * i / nf);
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
  fig->draw(backend);
  out.close();
}

TEST_CASE("histogram looks ok", "[svg_backend]") {

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
  out.open("test_histogram.svg");
  BackendSVG backend(out);
  fig->draw(backend);
  out.close();
}

TEST_CASE("svg backend init and finalise work as expected", "[svg_backend]") {

  std::stringstream out_ss;
  BackendSVG backend(out_ss);

  REQUIRE(out_ss.str().empty());

  SECTION("init produces correct string") {

    backend.init({123.4f, 234.5f}, "r@ndom^name", 345.6f);

    CHECK(is_substr_ignoring_ws(out_ss.str(), R"(width="123.4px")"));
    CHECK(is_substr_ignoring_ws(out_ss.str(), R"(height="234.5px")"));
    CHECK(is_substr_ignoring_ws(out_ss.str(), R"(<desc>r@ndom^name</desc>)"));
  }

  SECTION("finalise produces correct string") {

    backend.finalise();

    CHECK(compare_ignoring_ws(out_ss.str(), "</svg>"));
  }

  SECTION("forms valid svg") {

    backend.init({123.4f, 234.5f}, "r@ndom^name", 345.6f);
    backend.finalise();

    std::ofstream out_f;
    out_f.open("backend_svg_init_finalise.svg");

    out_f << out_ss.str();
    out_f.close();
  }
}

TEST_CASE("svg backend rect works as expected", "[svg_backend]") {

  std::stringstream out_ss;
  BackendSVG backend(out_ss);

  REQUIRE(out_ss.str().empty());

  SECTION("basic rectangle produces correct attributes") {

    backend.rect({{1.23f, 2.34f}, {3.45f, 5.67f}});

    CHECK(starts_with_ignoring_ws(out_ss.str(), "<rect"));

    CHECK(is_substr_ignoring_ws(out_ss.str(), R"(x="1.23")"));
    CHECK(is_substr_ignoring_ws(out_ss.str(), R"(y="2.34")"));
    CHECK(is_substr_ignoring_ws(out_ss.str(), R"(width="2.22")"));
    CHECK(is_substr_ignoring_ws(out_ss.str(), R"(height="3.33")"));
  }

  SECTION("basic rectangle forms valid svg") {

    backend.init({10.f, 10.f}, "name");
    backend.rect({{1.23f, 2.34f}, {3.45f, 5.67f}});
    backend.finalise();

    std::ofstream out_f;
    out_f.open("backend_svg_basic_rect.svg");
    out_f << out_ss.str();
    out_f.close();
  }

  SECTION("rounded rectangle produces correct string") {

    backend.rounded_rect({{1.23f, 2.34f}, {3.45f, 5.67f}}, 1.78f);

    CHECK(starts_with_ignoring_ws(out_ss.str(), "<rect"));

    CHECK(is_substr_ignoring_ws(out_ss.str(), R"(x="1.23")"));
    CHECK(is_substr_ignoring_ws(out_ss.str(), R"(y="2.34")"));
    CHECK(is_substr_ignoring_ws(out_ss.str(), R"(width="2.22")"));
    CHECK(is_substr_ignoring_ws(out_ss.str(), R"(height="3.33")"));
    CHECK(is_substr_ignoring_ws(out_ss.str(), R"(rx="1.78")"));
    CHECK(is_substr_ignoring_ws(out_ss.str(), R"(ry="1.78")"));
  }

  SECTION("rounded rectangle forms valid svg") {

    backend.init({10.f, 10.f}, "name");
    backend.rounded_rect({{1.23f, 2.34f}, {3.45f, 5.67f}}, 1.78f);
    backend.finalise();

    std::ofstream out_f;
    out_f.open("backend_svg_rounded_rect.svg");
    out_f << out_ss.str();
    out_f.close();
  }
}

TEST_CASE("svg backend circle works as expected", "[svg_backend]") {

  std::stringstream out_ss;
  BackendSVG backend(out_ss);

  REQUIRE(out_ss.str().empty());

  SECTION("basic circle produces correct attributes") {

    backend.circle({1.23f, 2.34f}, 3.45f);

    CHECK(starts_with_ignoring_ws(out_ss.str(), "<circle"));

    CHECK(is_substr_ignoring_ws(out_ss.str(), R"(cx="1.23")"));
    CHECK(is_substr_ignoring_ws(out_ss.str(), R"(cy="2.34")"));
    CHECK(is_substr_ignoring_ws(out_ss.str(), R"(r="3.45")"));
  }

  SECTION("basic circle forms valid svg") {

    backend.init({10.f, 10.f}, "name");
    backend.circle({1.23f, 2.34f}, 3.45f);
    backend.finalise();

    std::ofstream out_f;
    out_f.open("backend_svg_basic_circle.svg");
    out_f << out_ss.str();
    out_f.close();
  }
}
