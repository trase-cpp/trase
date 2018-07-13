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

#include <limits>
#include <type_traits>

#include "trase.hpp"

using namespace trase;

TEST_CASE("create raw data", "[data]") {
  RawData data;
  CHECK(data.rows() == 0);
  CHECK(data.cols() == 0);
}

TEST_CASE("add some cols to raw data", "[data]") {
  RawData data;
  std::vector<int> first_col = {1, 2, 3};
  data.add_column(first_col);

  CHECK(data.rows() == 3);
  CHECK(data.cols() == 1);

  {
    int ii = 0;
    for (auto i = data.begin(0); i != data.end(0); ++i, ++ii) {
      CHECK(*i == first_col[ii]);
    }
  }

  CHECK_THROWS_AS(data.begin(1), std::out_of_range);
  CHECK_THROWS_AS(data.end(1), std::out_of_range);
  CHECK_THROWS_AS(data.begin(-1), std::out_of_range);
  CHECK_THROWS_AS(data.end(-1), std::out_of_range);

  std::vector<float> second_col_incorrect = {3, 2, 1, 0};
  std::vector<float> second_col = {3, 2, 1};

  CHECK_THROWS_AS(data.add_column(second_col_incorrect), Exception);

  data.add_column(second_col);

  {
    int ii = 0;
    for (auto i = data.begin(0); i != data.end(0); ++i, ++ii) {
      CHECK(*i == first_col[ii]);
    }
  }

  {
    int ii = 0;
    for (auto i = data.begin(1); i != data.end(1); ++i, ++ii) {
      CHECK(*i == second_col[ii]);
    }
  }
}

TEST_CASE("create data with aesthetics", "[data]") {
  DataWithAesthetic data_w_aes(std::make_shared<RawData>());

  CHECK_THROWS_AS(data_w_aes.begin<Aesthetic::x>(), Exception);
  CHECK_THROWS_AS(data_w_aes.begin<Aesthetic::y>(), Exception);
  CHECK(data_w_aes.rows() == 0);

  DataWithAesthetic data_w_aes2;

  CHECK_THROWS_AS(data_w_aes2.begin<Aesthetic::x>(), Exception);
  CHECK(data_w_aes2.rows() == 0);

  auto data = std::make_shared<RawData>();
  std::vector<int> first_col = {1, 2, 3};
  data->add_column(first_col);

  DataWithAesthetic data_w_aes3(data);

  CHECK(data_w_aes3.rows() == 3);

  CHECK_THROWS_AS(data_w_aes3.begin<Aesthetic::x>(), Exception);
  CHECK_THROWS_AS(data_w_aes3.begin<Aesthetic::y>(), Exception);
  CHECK_THROWS_AS(data_w_aes3.begin<Aesthetic::size>(), Exception);
  CHECK_THROWS_AS(data_w_aes3.begin<Aesthetic::color>(), Exception);
}

TEST_CASE("use data with aesthetics", "[data]") {
  DataWithAesthetic data;
  std::vector<int> x = {1, 2, 3};
  std::vector<float> y = {3, 2, 1};
  std::vector<double> size = {-1, -1, -1};
  std::vector<float> color = {2, 2, 2};

  data.x(x).y(y).size(size).color(color);

  CHECK(data.rows() == 3);

  {
    int ii = 0;
    for (auto i = data.begin<Aesthetic::x>(); i != data.end<Aesthetic::x>();
         ++i, ++ii) {
      CHECK(*i == x[ii]);
    }
  }

  {
    int ii = 0;
    for (auto i = data.begin<Aesthetic::y>(); i != data.end<Aesthetic::y>();
         ++i, ++ii) {
      CHECK(*i == y[ii]);
    }
  }

  {
    int ii = 0;
    for (auto i = data.begin<Aesthetic::color>();
         i != data.end<Aesthetic::color>(); ++i, ++ii) {
      CHECK(*i == color[ii]);
    }
  }
}

TEST_CASE("aesthetics", "[data]") {
  // x/y lims = 0->100
  // color lims = 100->200
  // size lims = 1->2
  Limits lim({0, 0, 100, 1}, {100, 100, 200, 2});

  // xy pixel limits = 0 -> 200
  bfloat2_t pixels({0, 0}, {200, 200});

  // x
  float x_data = 1.f;
  auto x_display = Aesthetic::x::to_display(x_data, lim, pixels);
  CHECK(x_display == 2.f);
  auto x_data_check = Aesthetic::x::from_display(x_display, lim, pixels);
  CHECK(x_data_check == x_data);

  // y
  float y_data = 2.f;
  auto y_display = Aesthetic::y::to_display(y_data, lim, pixels);
  CHECK(y_display == pixels.bmax[1] - 4.f);
  auto y_data_check = Aesthetic::y::from_display(y_display, lim, pixels);
  CHECK(y_data_check == y_data);

  float size_data = 0.5f;
  auto size_display = Aesthetic::size::to_display(size_data, lim, pixels);
  // TODO: consistant way of checking size
  auto size_data_check =
      Aesthetic::size::from_display(size_display, lim, pixels);
  CHECK(size_data_check == size_data);

  // color
  float color_data = 100.f;
  auto color_display = Aesthetic::color::to_display(color_data, lim, pixels);
  CHECK(color_display == 0.f);
  auto color_data_check =
      Aesthetic::color::from_display(color_display, lim, pixels);
  CHECK(color_data_check == color_data);
}
