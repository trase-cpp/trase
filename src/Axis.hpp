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

#ifndef AXIS_H_
#define AXIS_H_

// forward declare Axis so can be stored in Plot1D
namespace trase {
class Axis;
}

#include "Drawable.hpp"
#include "Exception.hpp"
#include "Plot1D.hpp"
#include <array>
#include <memory>

namespace trase {

class Axis : public Drawable {
  /// plot extents [x_min,y_min,x_max,y_max]
  std::array<float, 4> m_limits;
  std::vector<std::shared_ptr<Plot1D>> m_plot1d;

public:
  Axis(const std::array<float, 4> &area);

  void add_limits(const std::array<float, 4> limits);

  // Container1 and 2 should be std::span in C++20?
  template <typename T1, typename T2>
  std::shared_ptr<Plot1D> plot(const std::vector<T1> &x,
                               const std::vector<T2> &y) {
    if (x.size() != y.size()) {
      throw Exception("x and y vector sizes do not match");
    }
    std::vector<float> x_copy(x.size());
    std::vector<float> y_copy(y.size());
    std::copy(x.begin(), x.end(), x_copy.begin());
    std::copy(y.begin(), y.end(), y_copy.begin());
    return plot_impl(std::move(x_copy), std::move(y_copy));
  }

  template <typename Backend> void draw(Backend &backend);

private:
  std::shared_ptr<Plot1D> plot_impl(std::vector<float> &&x,
                                    std::vector<float> &&y);

}; // namespace trase

} // namespace trase

#endif // AXIS_H_
