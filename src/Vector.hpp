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

#ifndef VECTOR_H_
#define VECTOR_H_

#include <algorithm>
#include <array>
#include <cmath>
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
  const static int size = N;

  using iter = T *;
  using const_iter = const T *;
  using reverse_iter = std::reverse_iterator<iter>;
  using const_reverse_iter = std::reverse_iterator<const_iter>;

  /// Constructs an vector and allocates memory
  Vector() = default;

  /// Constructs an vector with initial values.
  ///
  /// \param arg1 All the elements of the vector are set to
  /// this value
  explicit Vector(T arg1) { std::fill_n(begin(), N, arg1); }

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
  
  // Iterators
  iter begin() noexcept { return iter(mem.begin()); }
  const_iter begin() const noexcept { return const_iter(mem.begin()); }
  iter end() noexcept { return iter(mem.end()); }
  const_iter end() const noexcept { return const_iter(mem.end()); }
  reverse_iter rbegin() noexcept { return reverse_iter(end()); }
  const_reverse_iter rbegin() const noexcept { return const_reverse_iter(end()); }
  reverse_iter rend() noexcept { return reverse_iter(begin()); }
  const_reverse_iter rend() const noexcept { return const_reverse_iter(begin()); }
  const_iter cbegin() const noexcept { return const_iter(mem.begin()); }
  const_iter cend() const noexcept { return const_iter(mem.end()); }
  const_reverse_iter crbegin() const noexcept { return const_reverse_iter(end()); }
  const_reverse_iter crend() const noexcept { return const_reverse_iter(begin()); }


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

#ifdef HAVE_EIGEN
  /// Eigen Vector assignment
  ///
  /// Assigns an eigen vector object (arg) to this vector.
  ///
  template <typename Derived>
  Vector<T, N> &operator=(const Eigen::DenseBase<Derived> &arg) {
    static_assert(Eigen::DenseBase<Derived>::RowsAtCompileTime == N ||
                      Eigen::DenseBase<Derived>::ColsAtCompileTime == N,
                  "vector assignment has different or dynamic lengths");
    for (size_t i = 0; i < N; ++i) {
      mem[i] = arg[i];
    }
    return *this;
  }
#endif

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
  /// \return the inner product (dot product) of this vector
  /// with `arg`
  T inner_product(const Vector<T, N> &arg) const noexcept {
    return std::inner_product(begin(), end(), arg.begin(), static_cast<T>(0));
  }

  /// change vector type
  ///
  /// \return A new vector with each element `static_cast` to
  /// `T2`
  template <typename T2> Vector<T2, N> cast() {
    Vector<T2, N> ret;
    for (size_t i = 0; i < N; ++i) {
      ret[i] = static_cast<T2>(mem[i]);
    }
    return ret;
  }

  /// inner product
  ///
  /// \return The inner product (dot product) of this vector
  /// with `arg`
  ///
  /// \see inner_product
  template <typename T2> double dot(const Vector<T2, N> &arg) const {
    return inner_product(arg);
  }

  /// squared norm
  /// \return the squared 2-norm of the vector $\sum_i v_i^2$
  double squaredNorm() const {
    double ret = 0;
    for (size_t i = 0; i < N; ++i) {
      ret += mem[i] * mem[i];
    }
    return ret;
  }

  /// 2-norm
  /// \return the 2-norm of the vector $\sqrt{\sum_i v_i^2}$
  double norm() const { return std::sqrt(squaredNorm()); }

  /// inf-norm
  /// \return the infinity norm of the vector $\max_i |v_i|$
  double inf_norm() const {
    double ret = std::abs(mem[0]);
    for (int i = 1; i < N; ++i) {
      const double absi = std::abs(mem[i]);
      if (absi > ret) {
        ret = absi;
      }
    }
    return ret;
  }

  // element-wise `pow` function
  // \return a new vector with each element taken to the power of `exponent`
  // \param exponent the exponent
  template <typename EXP_T> Vector<T, N> pow(const EXP_T exponent) {
    Vector<T, N> n = *this;
    for (size_t i = 0; i < N; ++i) {
      n[i] = std::pow(n[i], exponent);
    }
    return n;
  }

  /// normalise vector so that its length (2-norm) equals one.
  /// \see norm
  void normalize() {
    double n = norm();
    for (size_t i = 0; i < N; ++i) {
      mem[i] /= n;
    }
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
  /// \return the minimum element of the vector
  T minCoeff() const {
    T min = mem[0];
    for (int i = 1; i < N; ++i) {
      if (mem[i] < min) {
        min = mem[i];
      }
    }
    return min;
  }

  /// find the maximum element of the vector
  /// \return the maximum element of the vector
  T maxCoeff() const {
    T max = mem[0];
    for (size_t i = 1; i < N; ++i) {
      if (mem[i] > max) {
        max = mem[i];
      }
    }
    return max;
  }

  /// returns the product of every element in the vector
  T prod() const {
    T ret = 1;
    for (size_t i = 0; i < N; ++i) {
      ret *= mem[i];
    }
    return ret;
  }

  /// returns the raw memory array containing the data for the vector
  T *data() noexcept { return mem.data(); }
  const T *data() const noexcept { return mem.data(); }

  template <class Archive> void serialize(Archive &ar, const int version) {
    (void)version;
    ar &BOOST_SERIALIZATION_NVP(mem);
  }

private:
  std::array<T, N> mem;
};

/// returns arg.pow(exponent)
template <typename T, int N, typename EXP_T>
Vector<T, N> pow(Vector<T, N> arg, EXP_T exponent) {
  return arg.pow(exponent);
}

/*
template<typename T1,typename T2,int N>
bool operator ==(const Vector<T1,N> &arg1, const Vector<T2,N> &arg2) {
    bool ret = true;
    for (size_t i = 0; i < N; ++i) {
        ret &= arg1[i] == arg2[i];
    }
    return ret;
}
*/

#define UNARY_OPERATOR(the_op)                                                 \
  template <typename T, int N>                                                 \
  Vector<double, N> operator the_op(const Vector<T, N> &arg1) {                \
    Vector<double, N> ret;                                                     \
    for (size_t i = 0; i < N; ++i) {                                           \
      ret[i] = the_op arg1[i];                                                 \
    }                                                                          \
    return ret;                                                                \
  }

/// unary `-` operator for Vector class
UNARY_OPERATOR(-)

#define OPERATOR(the_op)                                                       \
  template <typename T1, typename T2, int N,                                   \
            typename =                                                         \
                typename std::enable_if<std::is_arithmetic<T1>::value>::type>  \
  Vector<double, N> operator the_op(const T1 &arg1,                            \
                                    const Vector<T2, N> &arg2) {               \
    Vector<double, N> ret;                                                     \
    for (size_t i = 0; i < N; ++i) {                                           \
      ret[i] = arg1 the_op arg2[i];                                            \
    }                                                                          \
    return ret;                                                                \
  }                                                                            \
  template <typename T1, typename T2, int N,                                   \
            typename =                                                         \
                typename std::enable_if<std::is_arithmetic<T2>::value>::type>  \
  Vector<double, N> operator the_op(const Vector<T1, N> &arg1,                 \
                                    const T2 &arg2) {                          \
    Vector<double, N> ret;                                                     \
    for (size_t i = 0; i < N; ++i) {                                           \
      ret[i] = arg1[i] the_op arg2;                                            \
    }                                                                          \
    return ret;                                                                \
  }                                                                            \
                                                                               \
  template <typename T1, typename T2, int N>                                   \
  Vector<double, N> operator the_op(const Vector<T1, N> &arg1,                 \
                                    const Vector<T2, N> &arg2) {               \
    Vector<double, N> ret;                                                     \
    for (size_t i = 0; i < N; ++i) {                                           \
      ret[i] = arg1[i] the_op arg2[i];                                         \
    }                                                                          \
    return ret;                                                                \
  }                                                                            \
                                                                               \
  template <int N>                                                             \
  Vector<float, N> operator the_op(const Vector<float, N> &arg1,               \
                                   const Vector<float, N> &arg2) {             \
    Vector<float, N> ret;                                                      \
    for (size_t i = 0; i < N; ++i) {                                           \
      ret[i] = arg1[i] the_op arg2[i];                                         \
    }                                                                          \
    return ret;                                                                \
  }                                                                            \
  template <int N>                                                             \
  Vector<float, N> operator the_op(const float &arg1,                          \
                                   const Vector<float, N> &arg2) {             \
    Vector<float, N> ret;                                                      \
    for (size_t i = 0; i < N; ++i) {                                           \
      ret[i] = arg1 the_op arg2[i];                                            \
    }                                                                          \
    return ret;                                                                \
  }                                                                            \
  template <int N>                                                             \
  Vector<float, N> operator the_op(const Vector<float, N> &arg1,               \
                                   const float &arg2) {                        \
    Vector<float, N> ret;                                                      \
    for (size_t i = 0; i < N; ++i) {                                           \
      ret[i] = arg1[i] the_op arg2;                                            \
    }                                                                          \
    return ret;                                                                \
  }                                                                            \
                                                                               \
  template <int, int, int N>                                                   \
  Vector<int, N> operator the_op(const Vector<int, N> &arg1,                   \
                                 const Vector<int, N> &arg2) {                 \
    Vector<int, N> ret;                                                        \
    for (size_t i = 0; i < N; ++i) {                                           \
      ret[i] = arg1[i] the_op arg2[i];                                         \
    }                                                                          \
    return ret;                                                                \
  }                                                                            \
  template <int, int, int N>                                                   \
  Vector<int, N> operator the_op(const int &arg1,                              \
                                 const Vector<int, N> &arg2) {                 \
    Vector<int, N> ret;                                                        \
    for (size_t i = 0; i < N; ++i) {                                           \
      ret[i] = arg1 the_op arg2[i];                                            \
    }                                                                          \
    return ret;                                                                \
  }                                                                            \
  template <int, int, int N>                                                   \
  Vector<int, N> operator the_op(const Vector<int, N> &arg1,                   \
                                 const int &arg2) {                            \
    Vector<int, N> ret;                                                        \
    for (size_t i = 0; i < N; ++i) {                                           \
      ret[i] = arg1[i] the_op arg2;                                            \
    }                                                                          \
    return ret;                                                                \
  }

/// binary `+` operator for Vector class
OPERATOR(+)
/// binary `-` operator for Vector class
OPERATOR(-)
/// binary `/` operator for Vector class
OPERATOR(/)
/// binary `*` operator for Vector class
OPERATOR(*)

/*
template<typename T1,typename T2,int N>
Vector<double,N> operator *(const Vector<T1,N*N> &arg1, const Vector<T2,N>
&arg2) { Vector<double,N> ret; for (size_t i = 0; i < N; ++i) { ret[i] =
arg1[i*N] * arg2[0]; for (int j = 1; j < N; ++j) { ret[i] += arg1[i*N+j] *
arg2[j];
        }
    }
    return ret;
}
*/

// Comparison operators: == != < > <= >=

template <typename T, int N>
Vector<bool, N> operator==(const Vector<T, N> &a, const Vector<T, N> &b) {
  Vector<bool, N> ret{};
  std::transform(a.begin(), a.end(), b.begin(), ret.begin(),
                 std::equal_to<T>());
  return ret;
}

template <typename T, int N>
Vector<bool, N> operator!=(const Vector<T, N> &a, const Vector<T, N> &b) {
  Vector<bool, N> ret{};
  std::transform(a.begin(), a.end(), b.begin(), ret.begin(),
                 std::not_equal_to<T>());
  return ret;
}

template <typename T, int N>
Vector<bool, N> operator<(const Vector<T, N> &a, const Vector<T, N> &b) {
  Vector<bool, N> ret{};
  std::transform(a.begin(), a.end(), b.begin(), ret.begin(),
                 std::less<T>());
  return ret;
}

template <typename T, int N>
Vector<bool, N> operator>(const Vector<T, N> &a, const Vector<T, N> &b) {
  Vector<bool, N> ret{};
  std::transform(a.begin(), a.end(), b.begin(), ret.begin(),
                 std::greater<T>());
  return ret;
}

template <typename T, int N>
Vector<bool, N> operator<=(const Vector<T, N> &a, const Vector<T, N> &b) {
  Vector<bool, N> ret{};
  std::transform(a.begin(), a.end(), b.begin(), ret.begin(),
                 std::less_equal<T>());
  return ret;
}

template <typename T, int N>
Vector<bool, N> operator>=(const Vector<T, N> &a, const Vector<T, N> &b) {
  Vector<bool, N> ret{};
  std::transform(a.begin(), a.end(), b.begin(), ret.begin(),
                 std::greater_equal<T>());
  return ret;
}


#define COMPOUND_ASSIGN(the_op)                                                \
  template <typename T1, typename T2, int N>                                   \
  Vector<double, N> &operator the_op(Vector<T1, N> &arg1,                      \
                                     const Vector<T2, N> &arg2) {              \
    for (size_t i = 0; i < N; ++i) {                                           \
      arg1[i] the_op arg2[i];                                                  \
    }                                                                          \
    return arg1;                                                               \
  }                                                                            \
  template <typename T1, typename T2, int N>                                   \
  Vector<double, N> &operator the_op(Vector<T1, N> &arg1, const T2 &arg2) {    \
    for (size_t i = 0; i < N; ++i) {                                           \
      arg1[i] the_op arg2;                                                     \
    }                                                                          \
    return arg1;                                                               \
  }                                                                            \
                                                                               \
  template <int N>                                                             \
  Vector<float, N> &operator the_op(Vector<float, N> &arg1,                    \
                                    const Vector<float, N> &arg2) {            \
    for (size_t i = 0; i < N; ++i) {                                           \
      arg1[i] the_op arg2[i];                                                  \
    }                                                                          \
    return arg1;                                                               \
  }                                                                            \
  template <int N>                                                             \
  Vector<float, N> &operator the_op(Vector<float, N> &arg1,                    \
                                    const float &arg2) {                       \
    for (size_t i = 0; i < N; ++i) {                                           \
      arg1[i] the_op arg2;                                                     \
    }                                                                          \
    return arg1;                                                               \
  }                                                                            \
                                                                               \
  template <int, int, int N>                                                   \
  Vector<int, N> &operator the_op(Vector<int, N> &arg1,                        \
                                  const Vector<int, N> &arg2) {                \
    for (size_t i = 0; i < N; ++i) {                                           \
      arg1[i] the_op arg2[i];                                                  \
    }                                                                          \
    return arg1;                                                               \
  }                                                                            \
  template <int, int, int N>                                                   \
  Vector<int, N> &operator the_op(Vector<int, N> &arg1, const int &arg2) {     \
    for (size_t i = 0; i < N; ++i) {                                           \
      arg1[i] the_op arg2;                                                     \
    }                                                                          \
    return arg1;                                                               \
  }

/// compound assign `+=` comparison operator for Vector class
COMPOUND_ASSIGN(+=)
/// compound assign `-=` comparison operator for Vector class
COMPOUND_ASSIGN(-=)
/// compound assign `*=` comparison operator for Vector class
COMPOUND_ASSIGN(*=)
/// compound assign `/=` comparison operator for Vector class
COMPOUND_ASSIGN(/=)

#define UFUNC(the_op)                                                          \
  template <typename T, int N> Vector<T, N> the_op(const Vector<T, N> &arg1) { \
    Vector<T, N> ret;                                                          \
    for (size_t i = 0; i < N; ++i) {                                           \
      ret[i] = std::the_op(arg1[i]);                                           \
    }                                                                          \
    return ret;                                                                \
  }

/// element-wise `floor` rounding function for Vector class
UFUNC(floor)
/// element-wise `ceil` rounding function for Vector class
UFUNC(ceil)
/// element-wise `round` rounding function for Vector class
UFUNC(round)

/// return arg1.norm()
template <typename T, int I> double norm(const Vector<T, I> &arg1) {
  return arg1.norm();
}

/// return arg1.squaredNorm()
template <typename T, int I> double squaredNorm(const Vector<T, I> &arg1) {
  return arg1.squaredNorm();
}

/// external dot product for vector class (probably conflicts with symbolic
/// dot?)
template <typename T1, typename T2, int I>
double dot(const Vector<T1, I> &arg1, const Vector<T2, I> &arg2) {
  return arg1.inner_product(arg2);
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

/// returns new Vector made from element-wise absolute value of input arg
template <typename T, int N>
inline const Vector<T, N> abs(const Vector<T, N> &x) {
  Vector<T, N> ret;
  for (int i = 0; i < N; ++i) {
    ret[i] = std::fabs(x[i]);
  }
  return ret;
}

/// element-wise `e_i*e_i` function for Vector class
template <typename T, int N>
inline const Vector<T, N> abs2(const Vector<T, N> &x) {
  Vector<T, N> ret;
  for (int i = 0; i < N; ++i) {
    ret[i] = x[i] * x[i];
  }
  return ret;
}

/// round off to n significant digits
template <typename T, int N>
Vector<T, N> round_off(const Vector<T, N> &x, int n) {
  Vector<T, N> num = x;
  for (int j = 0; j < N; ++j) {
    // Counting the no. of digits to the left of decimal point
    // in the given no.
    T tmp = num[j];
    int i;
    for (i = 0; tmp >= 1; ++i) {
      tmp /= 10;
    }

    // round off to the given number of sig digits
    const T d = std::pow(static_cast<T>(10.0), n - i);
    num[j] = std::floor(num[j] * d + static_cast<T>(0.5)) / d;
  }
  return num;
}

/// stream output operator for Vector class
template <typename T, int N>
std::ostream &operator<<(std::ostream &out, const Vector<T, N> &v) {
  out << "(";
  for (size_t i = 0; i < N; ++i) {
    out << v[i];
    if (i != N - 1) {
      out << ",";
    }
  }
  return out << ")";
}

/// stream input operator for Vector class
template <typename T, int N>
std::istream &operator>>(std::istream &out, Vector<T, N> &v) {
  out.get();
  for (size_t i = 0; i < N; ++i) {
    out >> v[i];
    out.get();
  }
  return out;
}

using vfloat2_t = Vector<float, 2>;

} // namespace trase

#endif /* VECTOR_H_ */
