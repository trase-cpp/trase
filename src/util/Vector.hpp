/*

Copyright (c) 2005-2016, University of Oxford.
All rights reserved.

University of Oxford means the Chancellor, Masters and Scholars of the
University of Oxford, having an administrative office at Wellington
Square, Oxford OX1 2JD, UK.

This file is part of trase.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
 * Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.
 * Neither the name of the University of Oxford nor the names of its
   contributors may be used to endorse or promote products derived from this
   software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

/// \file Vector.hpp

#ifndef VECTOR_H_
#define VECTOR_H_

#include <algorithm>
#include <array>
#include <cmath>
#include <functional>
#include <iostream>
#include <iterator>
#include <numeric>

namespace trase {

/// \brief An N-dimensional vector class
///
///  Normal C++ operators ('*','/','<' etc.) operate on this vector
///  class in an element-wise fashion.
///
///  \param T the base type of each element of the vector
///  \param N the dimension of the vector (i.e. how many elements)
///
template <typename T, int N> class Vector {
public:
  using value_type = T;

  using iter = typename std::array<T, N>::iterator;
  using const_iter = typename std::array<T, N>::const_iterator;
  using reverse_iter = typename std::array<T, N>::reverse_iterator;
  using const_reverse_iter = typename std::array<T, N>::const_reverse_iterator;

  const static int size = N;

  /// Constructs an vector and allocates memory
  Vector() = default;

  /// Constructs an vector with initial values.
  ///
  /// \param arg1 All the elements of the vector are set to
  /// this value
  explicit Vector(T arg1) { std::fill(begin(), end(), arg1); }

  /// Constructs a vector with initial values.
  ///
  /// \param arg1 The first element is set to this value
  /// \param arg2 The second element is set to this value
  Vector(T arg1, T arg2) {
    static_assert(N == 2, "Vector must have 2 elements");
    mem[0] = arg1;
    mem[1] = arg2;
  }

  /// Constructs an vector with initial values.
  ///
  /// \param arg1 The first element is set to this value
  /// \param arg2 The second element is set to this value
  /// \param arg3 The third element is set to this value
  Vector(T arg1, T arg2, T arg3) {
    static_assert(N == 3, "Vector must have 3 elements");
    mem[0] = arg1;
    mem[1] = arg2;
    mem[2] = arg3;
  }

  /// Constructs an vector with initial values.
  ///
  /// \param arg1 The first element is set to this value
  /// \param arg2 The second element is set to this value
  /// \param arg3 The third element is set to this value
  /// \param arg4 The fourth element is set to this value
  Vector(T arg1, T arg2, T arg3, T arg4) {
    static_assert(N == 4, "Vector must have 4 elements");
    mem[0] = arg1;
    mem[1] = arg2;
    mem[2] = arg3;
    mem[3] = arg4;
  }

  /// Constructs an vector with initial values.
  ///
  /// \param arg1 The first element is set to this value
  /// \param arg2 The second element is set to this value
  /// \param arg3 The third element is set to this value
  /// \param arg4 The fourth element is set to this value
  /// \param arg5 The fourth element is set to this value
  Vector(T arg1, T arg2, T arg3, T arg4, T arg5) {
    static_assert(N == 5, "Vector must have 5 elements");
    mem[0] = arg1;
    mem[1] = arg2;
    mem[2] = arg3;
    mem[3] = arg4;
    mem[4] = arg5;
  }

  /// Constructs a vector with another vector of different type.
  ///
  /// The input vector must have same length as this one
  ///
  /// \param arg The vector to be copied
  template <typename T2> explicit Vector(const Vector<T2, N> &arg) {
    std::copy(arg.begin(), arg.end(), begin());
  }

  // Iterators
  iter begin() noexcept { return iter(mem.begin()); }
  const_iter begin() const noexcept { return const_iter(mem.begin()); }
  iter end() noexcept { return iter(mem.end()); }
  const_iter end() const noexcept { return const_iter(mem.end()); }
  reverse_iter rbegin() noexcept { return reverse_iter(end()); }
  const_reverse_iter rbegin() const noexcept {
    return const_reverse_iter(end());
  }
  reverse_iter rend() noexcept { return reverse_iter(begin()); }
  const_reverse_iter rend() const noexcept {
    return const_reverse_iter(begin());
  }
  const_iter cbegin() const noexcept { return const_iter(mem.begin()); }
  const_iter cend() const noexcept { return const_iter(mem.end()); }
  const_reverse_iter crbegin() const noexcept {
    return const_reverse_iter(end());
  }
  const_reverse_iter crend() const noexcept {
    return const_reverse_iter(begin());
  }

  /// Zero Vector
  ///
  /// Returns a zero vector
  static Vector Zero() {
    Vector ret;
    std::fill(ret.mem.begin(), ret.mem.end(), 0);
    return ret;
  }

  /// Constant Vector
  ///
  /// Returns a constant vector
  static Vector Constant(const T &c) {
    Vector ret;
    std::fill(ret.mem.begin(), ret.mem.end(), c);
    return ret;
  }

  /// const Index operator
  ///
  /// Returns a const reference to the `n`-th element of the vector
  ///
  /// \param n the element number to index
  const T &operator[](const int n) const noexcept { return mem[n]; }

  /// Index operator
  ///
  /// Returns a reference to the `n`-th element of the vector
  ///
  /// \param n the element number to index
  T &operator[](const int n) noexcept { return mem[n]; }

  /// inner product
  ///
  /// \return the inner product (dot product) of this vector with `arg`
  T inner_product(const Vector<T, N> &arg) const noexcept {
    return std::inner_product(begin(), end(), arg.begin(), static_cast<T>(0));
  }

  /// dot product
  ///
  /// \return The inner product (dot product) of this vector with `arg`
  ///
  /// \see inner_product
  T dot(const Vector<T, N> &arg) const noexcept { return inner_product(arg); }

  /// change vector type
  ///
  /// \return A new vector with each element `static_cast` to
  /// `T2`
  template <typename T2> Vector<T2, N> cast() const noexcept {
    Vector<T2, N> ret;
    std::transform(begin(), end(), ret.begin(),
                   [](const T &a) { return static_cast<T2>(a); });
    return ret;
  }

  /// squared norm
  /// \return the squared 2-norm of the vector $\sum_i v_i^2$
  T squaredNorm() const noexcept { return (*this).inner_product(*this); }

  /// 2-norm
  /// \return the 2-norm of the vector $\sqrt{\sum_i v_i^2}$
  double norm() const noexcept { return std::sqrt(squaredNorm()); }

  /// inf-norm
  /// \return the infinity norm of the vector $\max_i |v_i|$
  T inf_norm() const noexcept {
    T max = *std::max_element(begin(), end(), [](const T &a, const T &b) {
      return std::fabs(a) < std::fabs(b);
    });
    return max >= static_cast<T>(0) ? max : -max;
  }

  /// element-wise `pow` function
  /// \return a new vector with each element taken to the power of `exponent`
  /// \param exponent the exponent
  template <typename EXP_T>
  Vector<T, N> pow(const EXP_T exponent) const noexcept {
    Vector<T, N> ret;
    std::transform(begin(), end(), ret.begin(), [&exponent](const T &a) {
      return static_cast<T>(std::pow(a, exponent));
    });
    return ret;
  }

  /// normalise vector so that its length (2-norm) equals one.
  /// \see norm
  void normalize() noexcept {
    const double n = norm();
    std::transform(begin(), end(), begin(),
                   [&n](const T &a) { return static_cast<T>(a / n); });
  }

  /// collapse vector to bool using std::all_of
  /// \return true if all elements cast to true
  bool all() const noexcept {
    return std::all_of(begin(), end(),
                       [](const T &a) { return static_cast<bool>(a); });
  }

  /// collapse vector to bool using std::any_of
  /// \return true if any element casts to true
  bool any() const noexcept {
    return std::any_of(begin(), end(),
                       [](const T &a) { return static_cast<bool>(a); });
  }

  /// collapse vector to bool using std::none_of
  /// \return true if no element casts to true
  bool none() const noexcept {
    return std::none_of(begin(), end(),
                        [](const T &a) { return static_cast<bool>(a); });
  }

  /// find the minimum element of the vector
  /// \return the value of the minimum element of the vector
  T minCoeff() const noexcept { return *std::min_element(begin(), end()); }

  /// find the maximum element of the vector
  /// \return the maximum element of the vector
  T maxCoeff() const noexcept { return *std::max_element(begin(), end()); }

  /// returns the product of every element in the vector
  T prod() const noexcept {
    return std::accumulate(begin(), end(), static_cast<T>(1),
                           std::multiplies<T>());
  }

  /// returns the sum of every element in the vector
  T sum() const noexcept {
    return std::accumulate(begin(), end(), static_cast<T>(0));
  }

  /// returns the raw memory array containing the data for the vector
  T *data() noexcept { return mem.data(); }
  const T *data() const noexcept { return mem.data(); }

  /*
   * Compound assignment operators += -= *= /=
   *
   * First four are this-Vector elementwise operations (this[i] op other[i] for
   * all i). Second four are this-scalar elementwise operations (this[i] op k
   * for all i).
   */

  /// this-Vector compound plus operator
  Vector<T, N> &operator+=(const Vector<T, N> &a) {
    std::transform(begin(), end(), a.begin(), begin(), std::plus<T>());
    return *this;
  }

  /// this-Vector compound minus operator
  Vector<T, N> &operator-=(const Vector<T, N> &a) {
    std::transform(begin(), end(), a.begin(), begin(), std::minus<T>());
    return *this;
  }

  /// this-Vector compound multiples operator
  Vector<T, N> &operator*=(const Vector<T, N> &a) {
    std::transform(begin(), end(), a.begin(), begin(), std::multiplies<T>());
    return *this;
  }

  /// this-Vector compound divides operator
  Vector<T, N> &operator/=(const Vector<T, N> &a) {
    std::transform(begin(), end(), a.begin(), begin(), std::divides<T>());
    return *this;
  }

  /// this-scalar compound plus operator
  Vector<T, N> &operator+=(const T &k) {
    std::transform(begin(), end(), begin(),
                   std::bind(std::plus<T>(), std::placeholders::_1, k));
    return *this;
  }

  /// this-scalar compound minus operator
  Vector<T, N> &operator-=(const T &k) {
    std::transform(begin(), end(), begin(),
                   std::bind(std::minus<T>(), std::placeholders::_1, k));
    return *this;
  }

  /// this-scalar compound multiples operator
  Vector<T, N> &operator*=(const T &k) {
    std::transform(begin(), end(), begin(),
                   std::bind(std::multiplies<T>(), std::placeholders::_1, k));
    return *this;
  }

  /// this-scalar compound divides operator
  Vector<T, N> &operator/=(const T &k) {
    std::transform(begin(), end(), begin(),
                   std::bind(std::divides<T>(), std::placeholders::_1, k));
    return *this;
  }

private:
  std::array<T, N> mem;
};

/// returns arg.pow(exponent)
template <typename T, int N, typename EXP_T>
Vector<T, N> pow(const Vector<T, N> &arg, EXP_T exponent) noexcept {
  return arg.pow(exponent);
}

/// unary `-` (minus) operator for Vector class
template <typename T, int N>
Vector<T, N> operator-(const Vector<T, N> &a) noexcept {
  Vector<T, N> ret;
  std::transform(a.begin(), a.end(), ret.begin(), std::negate<T>());
  return ret;
}

/*
 * Binary operations.
 *
 * First four are Vector-Vector elementwise operations (V[i] op U[i] for all i).
 * Second four are scalar-Vector elementwise operations (k op V[i] for all i).
 * Third four are Vector-scalar elementwise operations (V[i] op k for all i).
 */

/// binary `+` (plus) operator for two Vectors
template <typename T, int N>
Vector<T, N> operator+(const Vector<T, N> &a, const Vector<T, N> &b) noexcept {
  Vector<T, N> ret;
  std::transform(a.begin(), a.end(), b.begin(), ret.begin(), std::plus<T>());
  return ret;
}

/// binary `-` (minus) operator for two Vectors
template <typename T, int N>
Vector<T, N> operator-(const Vector<T, N> &a, const Vector<T, N> &b) noexcept {
  Vector<T, N> ret;
  std::transform(a.begin(), a.end(), b.begin(), ret.begin(), std::minus<T>());
  return ret;
}

/// binary `*` (multiples) operator for two Vectors
template <typename T, int N>
Vector<T, N> operator*(const Vector<T, N> &a, const Vector<T, N> &b) noexcept {
  Vector<T, N> ret;
  std::transform(a.begin(), a.end(), b.begin(), ret.begin(),
                 std::multiplies<T>());
  return ret;
}

/// binary `/` (divides) operator for two Vectors
template <typename T, int N>
Vector<T, N> operator/(const Vector<T, N> &a, const Vector<T, N> &b) noexcept {
  Vector<T, N> ret;
  std::transform(a.begin(), a.end(), b.begin(), ret.begin(), std::divides<T>());
  return ret;
}

/// binary `+` (plus) operator for scalar and Vector
template <typename T, int N>
Vector<T, N> operator+(const T &a, const Vector<T, N> &b) noexcept {
  Vector<T, N> ret;
  std::transform(b.begin(), b.end(), ret.begin(),
                 std::bind(std::plus<T>(), a, std::placeholders::_1));
  return ret;
}

/// binary `-` (minus) operator for scalar and Vector
template <typename T, int N>
Vector<T, N> operator-(const T &a, const Vector<T, N> &b) noexcept {
  Vector<T, N> ret;
  std::transform(b.begin(), b.end(), ret.begin(),
                 std::bind(std::minus<T>(), a, std::placeholders::_1));
  return ret;
}

/// binary `*` (multiples) operator for scalar and Vector
template <typename T, int N>
Vector<T, N> operator*(const T &a, const Vector<T, N> &b) noexcept {
  Vector<T, N> ret;
  std::transform(b.begin(), b.end(), ret.begin(),
                 std::bind(std::multiplies<T>(), a, std::placeholders::_1));
  return ret;
}

/// binary `/` (divides) operator for scalar and Vector
template <typename T, int N>
Vector<T, N> operator/(const T &a, const Vector<T, N> &b) noexcept {
  Vector<T, N> ret;
  std::transform(b.begin(), b.end(), ret.begin(),
                 std::bind(std::divides<T>(), a, std::placeholders::_1));
  return ret;
}

/// binary `+` (plus) operator for Vector and scalar
template <typename T, int N>
Vector<T, N> operator+(const Vector<T, N> &b, const T &a) noexcept {
  Vector<T, N> ret;
  std::transform(b.begin(), b.end(), ret.begin(),
                 std::bind(std::plus<T>(), std::placeholders::_1, a));
  return ret;
}

/// binary `-` (minus) operator for Vector and scalar
template <typename T, int N>
Vector<T, N> operator-(const Vector<T, N> &b, const T &a) noexcept {
  Vector<T, N> ret;
  std::transform(b.begin(), b.end(), ret.begin(),
                 std::bind(std::minus<T>(), std::placeholders::_1, a));
  return ret;
}

/// binary `*` (multiples) operator for Vector and scalar
template <typename T, int N>
Vector<T, N> operator*(const Vector<T, N> &b, const T &a) noexcept {
  Vector<T, N> ret;
  std::transform(b.begin(), b.end(), ret.begin(),
                 std::bind(std::multiplies<T>(), std::placeholders::_1, a));
  return ret;
}

/// binary `/` (divides) operator for Vector and scalar
template <typename T, int N>
Vector<T, N> operator/(const Vector<T, N> &b, const T &a) noexcept {
  Vector<T, N> ret;
  std::transform(b.begin(), b.end(), ret.begin(),
                 std::bind(std::divides<T>(), std::placeholders::_1, a));
  return ret;
}

// Comparison operators: == != < > <= >=

template <typename T, int N>
Vector<bool, N> operator==(const Vector<T, N> &a,
                           const Vector<T, N> &b) noexcept {
  Vector<bool, N> ret;
  std::transform(a.begin(), a.end(), b.begin(), ret.begin(),
                 std::equal_to<T>());
  return ret;
}

template <typename T, int N>
Vector<bool, N> operator!=(const Vector<T, N> &a,
                           const Vector<T, N> &b) noexcept {
  Vector<bool, N> ret;
  std::transform(a.begin(), a.end(), b.begin(), ret.begin(),
                 std::not_equal_to<T>());
  return ret;
}

template <typename T, int N>
Vector<bool, N> operator<(const Vector<T, N> &a,
                          const Vector<T, N> &b) noexcept {
  Vector<bool, N> ret;
  std::transform(a.begin(), a.end(), b.begin(), ret.begin(), std::less<T>());
  return ret;
}

template <typename T, int N>
Vector<bool, N> operator>(const Vector<T, N> &a,
                          const Vector<T, N> &b) noexcept {
  Vector<bool, N> ret;
  std::transform(a.begin(), a.end(), b.begin(), ret.begin(), std::greater<T>());
  return ret;
}

template <typename T, int N>
Vector<bool, N> operator<=(const Vector<T, N> &a,
                           const Vector<T, N> &b) noexcept {
  Vector<bool, N> ret;
  std::transform(a.begin(), a.end(), b.begin(), ret.begin(),
                 std::less_equal<T>());
  return ret;
}

template <typename T, int N>
Vector<bool, N> operator>=(const Vector<T, N> &a,
                           const Vector<T, N> &b) noexcept {
  Vector<bool, N> ret;
  std::transform(a.begin(), a.end(), b.begin(), ret.begin(),
                 std::greater_equal<T>());
  return ret;
}

/// element-wise `floor` rounding function for Vector class
template <typename T, int N>
Vector<T, N> floor(const Vector<T, N> &a) noexcept {
  Vector<T, N> ret;
  std::transform(a.begin(), a.end(), ret.begin(),
                 [](const T &a) { return static_cast<T>(std::floor(a)); });
  return ret;
}

/// element-wise `ceil` rounding function for Vector class
template <typename T, int N> Vector<T, N> ceil(const Vector<T, N> &a) noexcept {
  Vector<T, N> ret;
  std::transform(a.begin(), a.end(), ret.begin(),
                 [](const T &a) { return static_cast<T>(std::ceil(a)); });
  return ret;
}

/// element-wise `round` rounding function for Vector class
template <typename T, int N>
Vector<T, N> round(const Vector<T, N> &a) noexcept {
  Vector<T, N> ret;
  std::transform(a.begin(), a.end(), ret.begin(),
                 [](const T &a) { return static_cast<T>(std::round(a)); });
  return ret;
}

/// element-wise absolute value function for Vector class
template <typename T, int N> Vector<T, N> abs(const Vector<T, N> &a) noexcept {
  Vector<T, N> ret;
  std::transform(a.begin(), a.end(), ret.begin(),
                 [](const T &a) { return static_cast<T>(std::fabs(a)); });
  return ret;
}

/// element-wise `e_i*e_i` function for Vector class
template <typename T, int N> Vector<T, N> abs2(const Vector<T, N> &a) noexcept {
  Vector<T, N> ret;
  std::transform(a.begin(), a.end(), ret.begin(),
                 [](const T &a) { return a * a; });
  return ret;
}

/// cross-product function for 3D vectors
template <typename T>
Vector<T, 3> cross(const Vector<T, 3> &arg1, const Vector<T, 3> &arg2) {
  Vector<T, 3> ret;
  ret[0] = arg1[1] * arg2[2] - arg1[2] * arg2[1];
  ret[1] = -arg1[0] * arg2[2] + arg1[2] * arg2[0];
  ret[2] = arg1[0] * arg2[1] - arg1[1] * arg2[0];
  return ret;
}

/// round off to n significant digits
template <typename T, int N>
Vector<T, N> round_off(const Vector<T, N> &arg, int n) {
  Vector<T, N> ret;
  std::transform(arg.begin(), arg.end(), ret.begin(), [&n](const T &a) {
    // Number of digits to the left of the decimal point
    const int num_digits = 1 + static_cast<int>(std::log10(a));
    //\todo: this does not work for all but small integers!
    const auto d = static_cast<T>(std::pow(10.0, n - num_digits));
    return static_cast<T>(std::floor(a * d + static_cast<T>(0.5)) / d);
  });
  return ret;
}

template <typename T, int N>
Vector<bool, N> approx_equal(const Vector<T, N> &a, const Vector<T, N> &b,
                             const T epsilon) {
  return abs(b - a) <= Vector<T, N>::Constant(epsilon);
}

/// stream output operator for Vector class
template <typename T, int N>
std::ostream &operator<<(std::ostream &out, const Vector<T, N> &v) {
  out << '(' << v[0];
  for (auto it = std::next(v.cbegin()); it != v.cend(); ++it) {
    out << ',' << *it;
  }
  return out << ')';
}

/// stream input operator for Vector class
template <typename T, int N>
std::istream &operator>>(std::istream &out, Vector<T, N> &v) {
  out.get();
  for (T &x : v) {
    out >> x;
    out.get();
  }
  return out;
}

using vfloat2_t = Vector<float, 2>;
using vint2_t = Vector<int, 2>;

/// hash function for an STL map of 2d int vectors
struct vint2_hash {
  std::size_t operator()(const vint2_t &s) const noexcept {
    // Cantor's enumeration of pairs
    return ((s[0] + s[1]) * (s[0] + s[1] + 1) / 2) + s[1];
  }
};

/// equality function for an STL map of 2d int vectors
struct vint2_equal {
  bool operator()(const vint2_t &a, const vint2_t &b) const noexcept {
    return a[0] == b[0] && a[1] == b[1];
  }
};

} // namespace trase

#endif /* VECTOR_H_ */
