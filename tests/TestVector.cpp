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

#include <iomanip>
#include <iterator>
#include <sstream>
#include <type_traits>

#include "util/Vector.hpp"

TEST_CASE("vector constructors", "[vector]") {

  // Default construction
  trase::Vector<int, 3> a{};

  // One arg constructor fills the vector with that value
  trase::Vector<int, 5> b(123);
  for (const auto i : b) {
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
  CHECK(*(a.end() - 1) == 234);

  CHECK(*a.cbegin() == 123);
  CHECK(*(a.cend() - 1) == 234);

  CHECK(*a.rbegin() == 234);
  CHECK(*(a.rend() - 1) == 123);

  CHECK(*a.crbegin() == 234);
  CHECK(*(a.crend() - 1) == 123);

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

  // Vector is trivially copyable so no need to check move constructor or move
  // assignment
  //    CHECK(std::is_trivially_copyable<trase::Vector<int, 2>>::value);
  //    CHECK(std::is_trivially_copyable<trase::Vector<int, 3>>::value);
  //    CHECK(std::is_trivially_copyable<trase::Vector<float, 2>>::value);
  //    CHECK(std::is_trivially_copyable<trase::Vector<float, 3>>::value);
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

TEST_CASE("inner product and norms", "[vector]") {

  trase::Vector<int, 4> a = {1, 2, 3, 4};
  trase::Vector<int, 4> b = {2, 3, 4, 5};

  trase::Vector<double, 4> c = {-1.2, 2.3, -5.2, 1.2};

  CHECK(a.inner_product(b) == 40);
  CHECK(a.inner_product(b) == b.inner_product(a));

  CHECK(a.dot(b) == 40);
  CHECK(a.dot(b) == b.dot(a));

  CHECK(a.squaredNorm() == 30);
  CHECK(b.squaredNorm() == 54);
  CHECK(c.squaredNorm() == 1.2 * 1.2 + 2.3 * 2.3 + 5.2 * 5.2 + 1.2 * 1.2);

  CHECK(a.norm() == std::sqrt(30));
  CHECK(b.norm() == std::sqrt(54));
  CHECK(c.norm() == std::sqrt(1.2 * 1.2 + 2.3 * 2.3 + 5.2 * 5.2 + 1.2 * 1.2));

  CHECK(a.inf_norm() == 4);
  CHECK(b.inf_norm() == 5);
  CHECK(c.inf_norm() == 5.2);

  CHECK(a.inf_norm() == (-a).inf_norm());
  CHECK(b.inf_norm() == (-b).inf_norm());
  CHECK(c.inf_norm() == (-c).inf_norm());

  c.normalize();
  CHECK(c.norm() == Approx(1.0));
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

TEST_CASE("operators", "[vector]") {

  // Unary minus
  {
    trase::Vector<int, 4> a = {1, -2, 13, 4};
    trase::Vector<int, 4> minus_a = {-1, 2, -13, -4};

    CHECK((-a == minus_a).all());
  }

  // Binary plus
  {
    trase::Vector<int, 4> a = {-3, -1, 14, 20};
    trase::Vector<int, 4> b = {-5, 8, 11, 17};
    int k = 27;

    trase::Vector<int, 4> vec_vec = b + a;
    trase::Vector<int, 4> sca_vec = k + a;
    trase::Vector<int, 4> vec_sca = a + k;

    for (unsigned i = 0; i < 4; ++i) {
      CHECK(vec_vec[i] == b[i] + a[i]);
      CHECK(sca_vec[i] == k + a[i]);
      CHECK(vec_sca[i] == a[i] + k);
    }
  }

  // Binary minus
  {
    trase::Vector<int, 4> a = {-3, -1, 14, 20};
    trase::Vector<int, 4> b = {-5, 8, 11, 17};
    int k = 27;

    trase::Vector<int, 4> vec_vec = b - a;
    trase::Vector<int, 4> sca_vec = k - a;
    trase::Vector<int, 4> vec_sca = a - k;

    for (unsigned i = 0; i < 4; ++i) {
      CHECK(vec_vec[i] == b[i] - a[i]);
      CHECK(sca_vec[i] == k - a[i]);
      CHECK(vec_sca[i] == a[i] - k);
    }
  }

  // Binary multiplies
  {
    trase::Vector<int, 4> a = {-3, -1, 14, 20};
    trase::Vector<int, 4> b = {-5, 8, 11, 17};
    int k = 27;

    trase::Vector<int, 4> vec_vec = b * a;
    trase::Vector<int, 4> sca_vec = k * a;
    trase::Vector<int, 4> vec_sca = a * k;

    for (unsigned i = 0; i < 4; ++i) {
      CHECK(vec_vec[i] == b[i] * a[i]);
      CHECK(sca_vec[i] == k * a[i]);
      CHECK(vec_sca[i] == a[i] * k);
    }
  }

  // Binary divides
  {
    trase::Vector<int, 4> a = {-3, -1, 14, 20};
    trase::Vector<int, 4> b = {-5, 8, 11, 17};
    int k = 27;

    trase::Vector<int, 4> vec_vec = b / a;
    trase::Vector<int, 4> sca_vec = k / a;
    trase::Vector<int, 4> vec_sca = a / k;

    for (unsigned i = 0; i < 4; ++i) {
      CHECK(vec_vec[i] == b[i] / a[i]);
      CHECK(sca_vec[i] == k / a[i]);
      CHECK(vec_sca[i] == a[i] / k);
    }
  }

  // Compound plus
  {
    trase::Vector<int, 4> a = {-3, -1, 14, 20};
    trase::Vector<int, 4> b = {-5, 8, 11, 17};
    int k = 2;

    auto copy_of_a = a;

    a += b;
    a += k;

    for (unsigned i = 0; i < 4; ++i) {
      CHECK(a[i] == (copy_of_a[i] + b[i]) + k);
    }
  }

  // Compound minus
  {
    trase::Vector<int, 4> a = {-3, -1, 14, 20};
    trase::Vector<int, 4> b = {-5, 8, 11, 17};
    int k = 2;

    auto copy_of_a = a;

    a -= b;
    a -= k;

    for (unsigned i = 0; i < 4; ++i) {
      CHECK(a[i] == (copy_of_a[i] - b[i]) - k);
    }
  }

  // Compound multiples
  {
    trase::Vector<int, 4> a = {-3, -1, 14, 20};
    trase::Vector<int, 4> b = {-5, 8, 11, 17};
    int k = 2;

    auto copy_of_a = a;

    a *= b;
    a *= k;

    for (unsigned i = 0; i < 4; ++i) {
      CHECK(a[i] == (copy_of_a[i] * b[i]) * k);
    }
  }

  // Compound divides
  {
    trase::Vector<int, 4> a = {-300, -100, 1400, 2000};
    trase::Vector<int, 4> b = {-5, 8, 11, 17};
    int k = 2;

    auto copy_of_a = a;

    a /= b;
    a /= k;

    for (unsigned i = 0; i < 4; ++i) {
      CHECK(a[i] == (copy_of_a[i] / b[i]) / k);
    }
  }
}

TEST_CASE("power", "[vector]") {

  trase::Vector<int, 4> a = {1, -2, 13, 4};
  trase::Vector<int, 4> a_squared = {1, 4, 169, 16};

  CHECK((a.pow<int>(2) == a_squared).all());

  trase::Vector<long double, 3> b = {2.3l, 3.4l, 4.5l};
  const float exp = 0.678f;
  trase::Vector<long double, 3> b_pow = {
      std::pow(2.3l, exp), std::pow(3.4l, exp), std::pow(4.5l, exp)};

  CHECK((b.pow<float>(exp) == b_pow).all());
  CHECK((trase::pow<long double, 3, float>(b, exp) == b_pow).all());
  CHECK((trase::pow(b, exp) == b_pow).all());
}

TEST_CASE("floor ceil round abs abs2", "[vector]") {

  trase::Vector<double, 3> a = {1.4, 2.5, 3.6};

  trase::Vector<double, 3> floor = {1.0, 2.0, 3.0};
  trase::Vector<double, 3> ceil = {2.0, 3.0, 4.0};
  trase::Vector<double, 3> round = {1.0, 3.0, 4.0};

  CHECK((trase::floor(a) == floor).all());
  CHECK((trase::ceil(a) == ceil).all());
  CHECK((trase::round(a) == round).all());

  trase::Vector<int, 3> b = {-2, -3, 4};

  trase::Vector<int, 3> abs = {2, 3, 4};
  trase::Vector<int, 3> abs2 = {4, 9, 16};

  CHECK((trase::abs(b) == abs).all());
  CHECK((trase::abs2(b) == abs2).all());
}

TEST_CASE("cross product", "[vector]") {

  trase::Vector<int, 3> a = {1, 2, 3};
  trase::Vector<int, 3> b = {12, -6, 97};

  trase::Vector<int, 3> a_cross_b = {212, -61, -30};

  CHECK((trase::cross(a, b) == a_cross_b).all());
}

TEST_CASE("round off", "[vector]") {

  trase::Vector<double, 3> a = {1.234567, 2.345678, 3.7};
  trase::Vector<double, 3> a4 = {1.235, 2.346, 3.7};
  trase::Vector<double, 3> a3 = {1.23, 2.35, 3.7};
  trase::Vector<double, 3> a2 = {1.2, 2.3, 3.7};
  trase::Vector<double, 3> a1 = {1.0, 2.0, 4.0};

  CHECK((trase::round_off(a, 4) == a4).all());
  CHECK((trase::round_off(a, 3) == a3).all());
  CHECK((trase::round_off(a, 2) == a2).all());
  CHECK((trase::round_off(a, 1) == a1).all());

  trase::Vector<int, 4> b = {1, 2, 3, 5};
  CHECK((trase::round_off(b, 1) == b).all());
}

TEST_CASE("approx_equal", "[vector]") {

  // Floating point tests
  trase::Vector<double, 3> a = {1.234567, 2.345678, 3.7};
  trase::Vector<double, 3> b = {1.2345670001, 2.3456780001, 3.7000000001};

  const double epsilon = std::numeric_limits<double>::epsilon();
  const double tooo_small = 0.00000000009;
  const double just_right = 0.00000000011;

  CHECK(trase::approx_equal(a, a, epsilon).all());
  CHECK(trase::approx_equal(b, b, epsilon).all());

  CHECK(!trase::approx_equal(a, b, epsilon).all());
  CHECK(!trase::approx_equal(a, b, tooo_small).all());
  CHECK(trase::approx_equal(a, b, just_right).all());

  // Integer tests
  trase::Vector<int, 3> c = {1, 2, 3};
  trase::Vector<int, 3> d = {0, 1, 2};

  CHECK(trase::approx_equal(c, c, 0).all());
  CHECK(trase::approx_equal(d, d, 0).all());

  CHECK(!trase::approx_equal(c, d, 0).all());
  CHECK(trase::approx_equal(c, d, 1).all());
}

TEST_CASE("stream operators", "[vector]") {

  // Floating point ostream
  trase::Vector<double, 3> a = {1.23456789123456, 2.34567891234567, 3.7};

  std::stringstream x_default;
  x_default << a;
  std::stringstream x_8;
  x_8 << std::setprecision(8) << a;
  std::stringstream x_3;
  x_3 << std::setprecision(3) << a;
  std::stringstream x_1;
  x_1 << std::setprecision(1) << a;

  CHECK(x_default.str() == "(1.23457,2.34568,3.7)");
  CHECK(x_8.str() == "(1.2345679,2.3456789,3.7)");
  CHECK(x_3.str() == "(1.23,2.35,3.7)");
  CHECK(x_1.str() == "(1,2,4)");

  // Integer ostream
  trase::Vector<int, 3> b = {1234, -78, 5};
  std::stringstream y;
  y << b;

  CHECK(y.str() == "(1234,-78,5)");

  // Floating point istream
  std::stringstream float_input;
  float_input << "(1.23457,2.34568,3.7)";
  trase::Vector<double, 3> float_vec;
  float_input >> float_vec;

  CHECK((float_vec == trase::Vector<double, 3>{1.23457, 2.34568, 3.7}).all());

  // Integer istream
  std::stringstream int_input;
  int_input << "(1234,-78,5)";
  trase::Vector<int, 3> int_vec;
  int_input >> int_vec;

  CHECK((int_vec == trase::Vector<int, 3>{1234, -78, 5}).all());
}
