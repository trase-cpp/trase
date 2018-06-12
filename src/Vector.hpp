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

#include <cmath>
#include <iostream>

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
  typedef T value_type;
  const static int size = N;

  /// Constructs an vector and allocates memory
  Vector() {}

  /// Constructs an vector with initial values.
  ///
  /// \param arg1 All the elements of the vector are set to
  /// this value
  Vector(T arg1) {
    for (int i = 0; i < N; i++) {
      mem[i] = arg1;
    }
  }

  /// Constructs an vector with initial values.
  ///
  /// \param arg1 The first element is set to this value
  /// \param arg2 The second element is set to this value
  Vector(T arg1, T arg2) {
    mem[0] = arg1;
    mem[1] = arg2;
  }

  /// Constructs an vector with initial values.
  ///
  /// \param arg1 The first element is set to this value
  /// \param arg2 The second element is set to this value
  /// \param arg3 The third element is set to this value
  Vector(T arg1, T arg2, T arg3) {
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
    mem[0] = arg1;
    mem[1] = arg2;
    mem[2] = arg3;
    mem[3] = arg4;
  }

  Vector(const Vector<T, N> &arg) {
    for (size_t i = 0; i < N; ++i) {
      mem[i] = arg.mem[i];
    }
  };

  /// Vector copy-constructor
  ///
  /// \param arg constructs a vector as a copy of this arguement
  template <typename T2> Vector(const Vector<T2, N> &arg) {
    for (size_t i = 0; i < N; ++i) {
      mem[i] = arg[i];
    }
  }

  /// Zero Vector
  ///
  /// Returns a zero vector
  static Vector Zero() {
    Vector ret;
    for (int i = 0; i < N; ++i) {
      ret.mem[i] = 0;
    }
    return ret;
  }

  /// Constant Vector
  ///
  /// Returns a constant vector
  static Vector Constant(const T &c) {
    Vector ret;
    for (int i = 0; i < N; ++i) {
      ret.mem[i] = c;
    }
    return ret;
  }

  /// Vector assignment
  ///
  /// Assigns a vector with different type `T2` but same length `N` to this
  /// vector
  ///
  /// \param arg Assigns the first N values from arg to this vector.
  template <typename T2> Vector<T, N> &operator=(Vector<T2, N> &arg) {
    for (size_t i = 0; i < N; ++i) {
      mem[i] = arg[i];
    }
    return *this;
  }

  /// Other Vector assignment
  ///
  /// Assigns a vector-like object (arg) to this vector.
  ///
  /// \param arg Vector-like object (with index operator)
  template <typename T2> Vector<T, N> &operator=(T2 *arg) {
    for (size_t i = 0; i < N; ++i) {
      mem[i] = arg[i];
    }
    return *this;
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
  const T &operator[](int n) const { return mem[n]; }

  /// Index operator
  ///
  /// Returns a reference to the `n`-th element of the vector
  ///
  /// \param n the element number to index
  T &operator[](int n) { return mem[n]; }

  /// inner product
  ///
  /// \return the inner product (dot product) of this vector
  /// with `arg`
  template <typename T2> double inner_product(const Vector<T2, N> &arg) const {
    double ret = 0;
    for (size_t i = 0; i < N; ++i) {
      ret += arg[i] * mem[i];
    }
    return ret;
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
      if (absi > ret)
        ret = absi;
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

  /// collapse boolean vector using `&` operator
  /// \return the accumulated `&` of all the vectors elements,
  ///         i.e. v_1 & v_2 & v3 & ...
  bool all() const {
    bool ret = true;
    for (size_t i = 0; i < N; ++i) {
      ret &= mem[i];
    }
    return ret;
  }

  /// collapse boolean vector using `|` operator
  /// \return the accumulated `|` of all the vectors elements,
  ///         i.e. v_1 | v_2 | v3 | ...
  bool any() const {
    bool ret = false;
    for (size_t i = 0; i < N; ++i) {
      ret |= mem[i];
    }
    return ret;
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
  T *data() { return mem; }

  template <class Archive> void serialize(Archive &ar, const int version) {
    ar &BOOST_SERIALIZATION_NVP(mem);
  }

private:
  T mem[N];
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

#define COMPARISON(the_op)                                                     \
  template <typename T1, typename T2, int N>                                   \
  Vector<bool, N> operator the_op(const Vector<T1, N> &arg1,                   \
                                  const Vector<T2, N> &arg2) {                 \
    Vector<bool, N> ret;                                                       \
    for (size_t i = 0; i < N; ++i) {                                           \
      ret[i] = arg1[i] the_op arg2[i];                                         \
    }                                                                          \
    return ret;                                                                \
  }                                                                            \
  template <typename T1, typename T2, int N,                                   \
            typename =                                                         \
                typename std::enable_if<std::is_arithmetic<T2>::value>::type>  \
  Vector<bool, N> operator the_op(const Vector<T1, N> &arg1, const T2 &arg2) { \
    Vector<bool, N> ret;                                                       \
    for (size_t i = 0; i < N; ++i) {                                           \
      ret[i] = arg1[i] the_op arg2;                                            \
    }                                                                          \
    return ret;                                                                \
  }                                                                            \
  /*                                                                           \
  template<typename T1,typename T2,int N>                                      \
  Vector<bool,N> operator the_op(const T1 &arg1, const T2 &arg2) {             \
      Vector<bool,N> ret;                                                      \
      for (size_t i = 0; i < N; ++i) {                                         \
          ret[i] = arg1 the_op arg2;                                           \
      }                                                                        \
      return ret;                                                              \
  }                                                                            \
  */

/// binary `>` comparison operator for Vector class
COMPARISON(>)
/// binary `<` comparison operator for Vector class
COMPARISON(<)
/// binary `<=` comparison operator for Vector class
COMPARISON(<=)
/// binary `>=` comparison operator for Vector class
COMPARISON(>=)
/// binary `==` comparison operator for Vector class
COMPARISON(==)
/// binary `!=` comparison operator for Vector class
COMPARISON(!=)

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

/* for eigen
 */
/// returns the input Vector x (for Eigen)
template <typename T, int N>
inline const Vector<T, N> &conj(const Vector<T, N> &x) {
  return x;
}

/// returns the input Vector x (for Eigen)
template <typename T, int N>
inline const Vector<T, N> &real(const Vector<T, N> &x) {
  return x;
}

/// returns imaginary component of Vector class (i.e. 0, for Eigen)
template <typename T, int N>
inline const Vector<T, N> imag(const Vector<T, N> &x) {
  return 0;
}

/// returns new Vector made from element-wise absolute value of input arg (for
/// Eigen)
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

/// stream output operator for Vector class
template <typename T, int N>
std::ostream &operator<<(std::ostream &out, const Vector<T, N> &v) {
  out << "(";
  for (size_t i = 0; i < N; ++i) {
    out << v[i];
    if (i != N - 1)
      out << ",";
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

typedef Vector<float, 2> vfloat2_t;

#endif /* VECTOR_H_ */