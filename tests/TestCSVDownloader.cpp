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

#include <map>
#include <type_traits>

#include "trase.hpp"

using namespace trase;

TEST_CASE("download test file", "[csv downloader]") {
  CSVDownloader dl;
  dl.set_delim('\t');
#ifdef TRASE_HAVE_CURL
  auto data = dl.download("https://www.stat.ubc.ca/~jenny/notOcto/STAT545A/"
                          "examples/gapminder/data/gapminderDataFiveYear.txt");
  std::map<std::string, std::string> expected;
  expected["country"] = "Afghanistan";
  expected["year"] = "1952";
  expected["pop"] = "8425333";
  expected["continent"] = "Asia";
  expected["lifeExp"] = "28.801";
  expected["gdpPercap"] = "779.4453145";
  int i = 0;
  for (auto it = data.begin(); it != data.end(); ++it, ++i) {
    CHECK(it->second[0] == expected[it->first]);
  }
  CHECK(i == 6);
#else
  REQUIRE_THROWS_WITH(
      dl.download("https://www.stat.ubc.ca/~jenny/notOcto/STAT545A/"
                  "examples/gapminder/data/gapminderDataFiveYear.txt"),
      Catch::Contains("libcurl not found"));
#endif
}

TEST_CASE("download test file 2", "[csv downloader]") {
  CSVDownloader dl;
#ifdef TRASE_HAVE_CURL
  auto data = dl.download(
      "http://archive.ics.uci.edu/ml/machine-learning-databases/iris/iris.data",
      {"sepal_length", "sepal_width", "petal_length", "petal_width", "class"});
  std::map<std::string, std::string> expected;
  expected["sepal_length"] = "5.1";
  expected["sepal_width"] = "3.5";
  expected["petal_length"] = "1.4";
  expected["petal_width"] = "0.2";
  expected["class"] = "Iris-setosa";
  int i = 0;
  for (auto it = data.begin(); it != data.end(); ++it, ++i) {
    CHECK(it->second[0] == expected[it->first]);
  }
  CHECK(i == 5);
#endif
}
