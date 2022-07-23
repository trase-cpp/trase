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
#include <limits>
#include <type_traits>

#include "DummyDraw.hpp"
#include "trase.hpp"

using namespace trase;

TEST_CASE("axis can be created", "[axis]") {
  auto fig = figure({800, 600});
  auto ax = fig->axis();
}

TEST_CASE("check plot methods can be invoked", "[axis]") {

  std::vector<int> three_ints_a = {1, 2, 3};
  std::vector<int> three_ints_b = {1, 2, 3};
  std::vector<int> four_ints = {1, 2, 3, 4};

  auto fig = figure();
  auto ax = fig->axis();

  CHECK_NOTHROW(ax->line(create_data().x(three_ints_a).y(three_ints_b)));
  CHECK_NOTHROW(ax->line(create_data().x(three_ints_a).y(three_ints_b)));
  CHECK_NOTHROW(ax->plot(0));
  CHECK_NOTHROW(ax->plot(1));

  CHECK_THROWS_AS(ax->plot(2), std::out_of_range);
  CHECK_THROWS_AS(ax->plot(-1), std::out_of_range);

  CHECK_THROWS_AS(ax->line(create_data().x(three_ints_a).y(four_ints)),
                  Exception);
}

TEST_CASE("check limit setting", "[axis]") {
  auto fig = figure();
  auto ax = fig->axis();
  CHECK(ax->limits().is_empty() == true);
  ax->xlim({0.0f, 1.0f});
  CHECK(ax->limits().bmin[0] == 0.0f);
  CHECK(ax->limits().bmax[0] == 1.0f);
  ax->ylim({-1.0f, 1.0f});
  CHECK(ax->limits().bmin[1] == -1.0f);
  CHECK(ax->limits().bmax[1] == 1.0f);
}

TEST_CASE("to/from pixel coordinates", "[axis]") {
  // axis will be 80 x 80 pixels from 10 - 90 each dim
  auto fig = figure({100, 100});
  auto ax = fig->axis();
  CHECK(ax->pixels().bmin[0] == 10.f);
  CHECK(ax->pixels().bmin[1] == 10.f);
  CHECK(ax->pixels().bmax[0] == 90.f);
  CHECK(ax->pixels().bmax[1] == 90.f);
  ax->xlim({0.0f, 1.0f});
  ax->ylim({0.0f, 1.0f});
  auto middle1 = vfloat2_t{ax->to_display<Aesthetic::x>(0.5f),
                           ax->to_display<Aesthetic::y>(0.5f)};
  CHECK(middle1[0] == 50.f);
  CHECK(middle1[1] == 50.f);
  auto middle2 = vfloat2_t{ax->from_display<Aesthetic::x>(middle1[0]),
                           ax->from_display<Aesthetic::y>(middle1[1])};
  CHECK(middle2[0] == 0.5f);
  CHECK(middle2[1] == 0.5f);
}

TEST_CASE("labels", "[axis]") {
  // axis will be 80 x 80 pixels from 10 - 90 each dim
  auto fig = figure();
  auto ax = fig->axis();
  const std::vector<float> x = {1.f, 2.f, 3.f};
  const std::vector<float> y = {1.f, 2.f, 3.f};
  auto data = create_data().x(x).y(y);
  auto plot1 = ax->line(data);
  plot1->set_label("plot1");
  CHECK(plot1->get_label() == "plot1");
  auto plot2 = ax->line(data);
  plot2->set_label("plot2");
  CHECK(plot1->get_label() == "plot1");
  CHECK(plot2->get_label() == "plot2");
  auto plot3 = ax->line(data);
  CHECK(plot3->get_label() == "");
}

TEST_CASE("set number of ticks", "[axis]") {
  auto fig = figure();
  auto ax = fig->axis();
  const std::vector<float> x = {1.f, 2.f, 3.f};
  const std::vector<float> y = {1.f, 2.f, 3.f};
  auto data = create_data().x(x).y(y);
  auto line = ax->line(data);
  CHECK(ax->get_ticks()[0] == 0);
  CHECK(ax->get_ticks()[1] == 0);
  DummyDraw::draw("axis", fig);
  ax->set_ticks({10, 0});
  DummyDraw::draw("axis", fig);
  CHECK(ax->get_ticks()[0] == 10);
  CHECK(ax->get_ticks()[1] == 0);
  ax->set_ticks({10, 10});
  DummyDraw::draw("axis", fig);
  CHECK(ax->get_ticks()[0] == 10);
  CHECK(ax->get_ticks()[1] == 10);
}


TEST_CASE("ylabel position: case 1", "[axis]") {
  std::vector<double> predicted_data = {
        -13.8881, -13.8671, -13.8361, -13.7881, -13.7681, -13.7381, -12.6881, -12.661, -12.6481
    };
    std::vector<double> exp_data = {
        -13.8881, -13.8671, -13.8361, -13.7881, -13.7681, -13.7381, -12.6881, -12.661, -12.6481
    };

    auto fig = figure();
    auto ax = fig->axis();
    int predicted_size = predicted_data.size();
    int exp_size = exp_data.size();

    std::vector<float> x(predicted_size);
    std::vector<float> y(exp_size);
    for (int i = 0; i < predicted_size; ++i) {
        x[i] = predicted_data[i];
    }
    for (int i = 0; i < exp_size; ++i) {
        y[i] = exp_data[i];
    }

    auto data = create_data().x(x).y(y);
    auto plt = ax->points(data);

    ax->xlabel("Pred. Target");
    ax->ylabel("Exp. Target");

    // the default number of digits for the labels is set as 2
    CHECK(ax->max_ytick_len() == int(5 * 18.0 / 1.25));

    // output to svg
    std::ofstream out;
    out.open("test_axis_ylabel_position_case1.svg");
    BackendSVG backend(out);
    fig->draw(backend);
    out.close();
}

TEST_CASE("ylabel position: case 2", "[axis]") {
  std::vector<double> predicted_data = {
        -13.8881, -13.8671, -13.8361, -13.7881, -13.7681, -13.7381, -12.6881, -12.661, -12.6481
    };
    std::vector<double> exp_data = {
        -13.8881, -13.8671, -13.8361, -13.7881, -13.7681, -13.7381, -12.6881, -12.661, -12.6481
    };

    auto fig = figure();
    auto ax = fig->axis();
    int predicted_size = predicted_data.size();
    int exp_size = exp_data.size();

    std::vector<float> x(predicted_size);
    std::vector<float> y(exp_size);
    for (int i = 0; i < predicted_size; ++i) {
        x[i] = predicted_data[i] * 10.0;
    }
    for (int i = 0; i < exp_size; ++i) {
        y[i] = exp_data[i] * 10.0;
    }

    auto data = create_data().x(x).y(y);
    auto plt = ax->points(data);

    ax->xlabel("Pred. Target");
    ax->ylabel("Exp. Target");

    // the default number of digits for the labels is set as 2
    CHECK(ax->max_ytick_len() == int(4 * 18.0 / 1.25));

    // output to svg
    std::ofstream out;
    out.open("test_axis_ylabel_position_case2.svg");
    BackendSVG backend(out);
    fig->draw(backend);
    out.close();
}
