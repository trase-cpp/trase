/*
Copyright (c) 2018, University of Oxford.
All rights reserved.

University of Oxford means the Chancellor, Masters and Scholars of the
University of Oxford, having an administrative office at Wellington
Square, Oxford OX1 2JD, UK.

This file is part of trase.

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

#include "Colors.hpp"
#include "Drawable.hpp"
#include "Exception.hpp"
#include "Figure.hpp"
#include "Plot1D.hpp"
#include <array>
#include <memory>

namespace trase {

class Axis : public Drawable {
  std::vector<std::shared_ptr<Plot1D>> m_plot1d;

  /// plot extents [x_min,y_min,x_max,y_max]
  bfloat2_t m_limits;

  float m_line_width;
  float m_font_size;

  std::string m_font_face;
  std::string m_xlabel;
  std::string m_ylabel;
  std::string m_title;

  /// true if legend is visible
  bool m_legend;

public:
  Axis(Figure &figure, const bfloat2_t &area);

  const bfloat2_t &limits() const { return m_limits; }
  bfloat2_t &limits() { return m_limits; }

  void xlim(std::array<float, 2> xlimits) {
    m_limits.bmin[0] = xlimits[0];
    m_limits.bmax[0] = xlimits[1];
  }
  void ylim(std::array<float, 2> xlimits) {
    m_limits.bmin[1] = xlimits[0];
    m_limits.bmax[1] = xlimits[1];
  }
  void xlabel(const char *string) { m_xlabel.assign(string); }
  void ylabel(const char *string) { m_ylabel.assign(string); }
  void title(const char *string) { m_title.assign(string); }
  void legend() { m_legend = true; }

  /// Create a new plot and return a shared pointer to it.
  /// \param x the x-values to plot
  /// \param y the y-values to plot
  /// \param label the plot label. Defaults to empty string.
  /// \return shared pointer to the new plot
  template <typename T1, typename T2>
  std::shared_ptr<Plot1D> plot(const std::vector<T1> &x,
                               const std::vector<T2> &y,
                               const std::string &label = std::string()) {
    if (x.size() != y.size()) {
      throw Exception("x and y vector sizes do not match");
    }
    std::vector<vfloat2_t> values(x.size());
    for (size_t i = 0; i < x.size(); ++i) {
      values[i][0] = static_cast<float>(x[i]);
      values[i][1] = static_cast<float>(y[i]);
    }
    return plot_impl(std::move(values), label);
  }

  /// Return a shared pointer to an existing plot.
  /// Throws std::out_of_range exception if out of range.
  /// \param n the plot to return
  /// \return a shared pointer to the nth plot
  std::shared_ptr<Plot1D> plot(int n);

  template <typename Backend> void serialise(Backend &backend);
  template <typename Backend> void draw(Backend &backend, float time);

  vfloat2_t from_pixel(const vfloat2_t &i) {
    auto inv_delta = 1.0f / m_pixels.delta();
    return m_limits.bmin +
           m_limits.delta() *
               vfloat2_t((i[0] - m_pixels.bmin[0]) * inv_delta[0],
                         (m_pixels.bmax[1] - i[1]) * inv_delta[1]);
  }

  vfloat2_t to_pixel(const vfloat2_t &i) {
    auto inv_delta = 1.0f / m_limits.delta();
    return m_pixels.bmin +
           m_pixels.delta() *
               vfloat2_t((i[0] - m_limits.bmin[0]) * inv_delta[0],
                         (m_limits.bmax[1] - i[1]) * inv_delta[1]);
  }

  void font_face(const std::string &fontFace) { m_font_face = fontFace; }

private:
  std::shared_ptr<Plot1D> plot_impl(std::vector<vfloat2_t> &&values,
                                    const std::string &label);

  template <typename Backend> void draw_common(Backend &backend);

  void set_auto_ticks();
};

} // namespace trase

#endif // AXIS_H_
