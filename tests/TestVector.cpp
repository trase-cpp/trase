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

#include <iterator>
#include <type_traits>

#include "Vector.hpp"

TEST_CASE("vector constructors", "[vector]") {

    // Default construction
    trase::Vector<int, 3> a{};

    // One arg constructor fills the vector with that value
    trase::Vector<int, 5> b(123);
    for (const auto i : b){
        CHECK(i == 123);
    }

    trase::Vector<int, 2> c(1, 2);
    CHECK(c[0] == 1);
    CHECK(c[1] == 2);

    trase::Vector<int, 3> d(1, 2, 3);
    CHECK(d[0] == 1);
    CHECK(d[1] == 2);
    CHECK(d[2] == 3);

    trase::Vector<int, 4> e(1, 2, 3, 4);
    CHECK(e[0] == 1);
    CHECK(e[1] == 2);
    CHECK(e[2] == 3);
    CHECK(e[3] == 4);
}

TEST_CASE("vector iterators", "[vector]") {

    trase::Vector<int, 3> a = {123, 0, 234};

    CHECK(*a.begin() == 123);
    CHECK(*(a.end()-1) == 234);

    CHECK(*a.cbegin() == 123);
    CHECK(*(a.cend()-1) == 234);

    CHECK(*a.rbegin() == 234);
    CHECK(*(a.rend()-1) == 123);

    CHECK(*a.crbegin() == 234);
    CHECK(*(a.crend()-1) == 123);

    CHECK(std::distance(a.begin(), a.end()) == 3);
    CHECK(std::distance(a.cbegin(), a.cend()) == 3);
    CHECK(std::distance(a.rbegin(), a.rend()) == 3);
    CHECK(std::distance(a.crbegin(), a.crend()) == 3);
}

TEST_CASE("rule of five", "[vector]") {

    trase::Vector<int, 4> ref = {123, 234, 345, 456};

    // Copy constructor
    {
        trase::Vector<int, 4> a = {123, 234, 345, 456};
        trase::Vector<int, 4> b(a);
        CHECK((a == ref).all());
        CHECK((b == ref).all());
    }

    // Copy assignment
    {
        trase::Vector<int, 4> a = {123, 234, 345, 456};
        trase::Vector<int, 4> b = a;
        CHECK((a == ref).all());
        CHECK((b == ref).all());
    }

    // Vector is trivially copyable so no need to check move constructor or move assignment
    CHECK(std::is_trivially_copyable<trase::Vector<int, 2>>::value);
    CHECK(std::is_trivially_copyable<trase::Vector<int, 3>>::value);
    CHECK(std::is_trivially_copyable<trase::Vector<float, 2>>::value);
    CHECK(std::is_trivially_copyable<trase::Vector<float, 3>>::value);
}


TEST_CASE("comparison operators", "[vector]") {

  trase::Vector<int, 4> a = {123, 234, 345, 456};
  CHECK((a == a).all());
  CHECK((a <= a).all());
  CHECK((a >= a).all());
  CHECK((a != a).none());
  CHECK((a < a).none());
  CHECK((a > a).none());

  trase::Vector<int, 4> b = {0, 1, 2, 3};
  CHECK((a == b).none());
  CHECK((a <= b).none());
  CHECK((a >= b).all());
  CHECK((a != b).all());
  CHECK((a < b).none());
  CHECK((a > b).all());

  trase::Vector<int, 4> c = {0, 234, 345, 0};
  trase::Vector<bool, 4> mask = {false, true, true, false};
  CHECK(((a == c) == mask).all());
  CHECK(((a != c) == mask).none());
  CHECK(((a <= c) == mask).all());
}

TEST_CASE("all any none", "[vector]") {
    trase::Vector<bool, 2> zero = {false, false};
    trase::Vector<bool, 2> one = {false, true};
    trase::Vector<bool, 2> two = {true, true};

    CHECK(!zero.all());
    CHECK(!zero.any());
    CHECK(zero.none());

    CHECK(!one.all());
    CHECK(one.any());
    CHECK(!one.none());

    CHECK(two.all());
    CHECK(two.any());
    CHECK(!two.none());
}

TEST_CASE("inner product", "[vector]") {

    trase::Vector<int, 4> a = {1, 2, 3, 4};
    trase::Vector<int, 4> b = {2, 3, 4, 5};

    CHECK(a.inner_product(b) == 40);
    CHECK(a.inner_product(b) == b.inner_product(a));

    CHECK(a.dot(b) == 40);
    CHECK(a.dot(b) == b.dot(a));
}

TEST_CASE("cast", "[vector]") {

  trase::Vector<double, 4> a = {1.2, 2.4, 3.6, 4.8};
  trase::Vector<int, 4> b = a.cast<int>();

  for (unsigned i = 0; i < 4; ++i) {
    CHECK(b[i] == static_cast<int>(b[i]));
  }
}

TEST_CASE("min max", "[vector]") {

  trase::Vector<int, 4> a = {1, -2, 13, 4};

  CHECK(a.maxCoeff() == 13);
  CHECK(a.minCoeff() == -2);
}

TEST_CASE("product and sum", "[vector]") {

  trase::Vector<int, 4> a = {1, -2, 13, 4};

  CHECK(a.prod() == -104);
  CHECK(a.sum() == 16);
}
