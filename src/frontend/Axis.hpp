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

/// \file Axis.hpp

#ifndef AXIS_H_
#define AXIS_H_

#include <array>
#include <memory>

#include "frontend/Data.hpp"
#include "frontend/Drawable.hpp"
#include "frontend/Histogram.hpp"
#include "frontend/Line.hpp"
#include "frontend/Points.hpp"
#include "util/Colors.hpp"
#include "util/Exception.hpp"

namespace trase {

/// A helper struct for Axis that holds tick-related information
struct TickInfo {
  std::vector<float> x_val;
  std::vector<float> y_val;
  std::vector<float> x_pos;
  std::vector<float> y_pos;

  void clear() {
    x_val.clear();
    y_val.clear();
    x_pos.clear();
    y_pos.clear();
  }
};

/// An 2D axis that can contain zero or more Plot1D objects
///
/// Each axis stores minimum and maximum limits for each Aesthetic, these are
/// used to both set the ticks displayed on the x and y axis, and to scale the
/// drawn Plot1D objects (how each Plot1D is scaled depends on the specifics of
/// the Geometry used). Each time a new Plot1D is added to the Axis, it updates
/// these limits using the limits of the its DataWithAestheic.
///
/// TODO: issue #87, the name of the Plot1D class will probably change to
/// Geometry in the future
///
/// An Axis can optionally contain:
///     * a title
///     * a label for the x axis
///     * a label for the y axis
///     * a legend that identifies each Plot1D
class Axis : public Drawable {
  /// limits of all children plots
  Limits m_limits;

  /// the number of significant digits displayed on the ticks
  int m_sig_digits;

  /// the number of ticks displayed on the x axis
  int m_nx_ticks;

  /// the number of ticks displayed on the y axis
  int m_ny_ticks;

  /// the length (in pixels) of each tick
  float m_tick_len;

  /// the linewidth used
  float m_line_width;

  /// the font size used
  float m_font_size;

  /// the font used
  std::string m_font_face;

  /// the label displayed on the x axis
  std::string m_xlabel;

  /// the label displayed on the y axis
  std::string m_ylabel;

  /// the Axis title
  std::string m_title;

  /// true if legend is visible
  bool m_legend;

  /// tick helper
  TickInfo m_tick_info;

public:
  /// create a new Axis contained in the given Figure
  /// \param figure the parent \Drawable object
  /// \param area the drawable area assigned to this Drawable as a ratio of the
  /// parent size
  Axis(Drawable &parent, const bfloat2_t &area);

  DEFINE_VISITABLE()

  /// returns the current Aesthetic limits
  const Limits &limits() const { return m_limits; }

  /// returns the current Aesthetic limits, allowing them to be set manually
  Limits &limits() { return m_limits; }

  /// a helper function to set the x Aesthetic limits manually
  void xlim(std::array<float, 2> xlimits) {
    m_limits.bmin[Aesthetic::x::index] = xlimits[0];
    m_limits.bmax[Aesthetic::x::index] = xlimits[1];
  }

  /// a helper function to set the y Aesthetic limits manually
  void ylim(std::array<float, 2> ylimits) {
    m_limits.bmin[Aesthetic::y::index] = ylimits[0];
    m_limits.bmax[Aesthetic::y::index] = ylimits[1];
  }

  /// set the label on the x axis
  void xlabel(const char *string) { m_xlabel.assign(string); }

  /// set the label on the y axis
  void ylabel(const char *string) { m_ylabel.assign(string); }

  /// set the title of the Axis
  void title(const char *string) { m_title.assign(string); }

  /// show a legend identifying each Plot1D in the Axis
  void legend() { m_legend = true; }

  /// Create a new plot and return a shared pointer to it.
  ///
  /// TODO: issue 88 - this function might be removed in the future, use line()
  /// instead
  ///
  /// \param x the x-values to plot
  /// \param y the y-values to plot
  /// \param label the plot label. Defaults to empty string.
  /// \return shared pointer to the new plot
  template <typename T1, typename T2>
  std::shared_ptr<Plot1D> plot(const std::vector<T1> &x,
                               const std::vector<T2> &y) {
    if (x.size() != y.size()) {
      throw Exception("x and y vector sizes do not match");
    }
    return plot_impl(std::make_shared<Line>(*this), Transform(Identity()),
                     DataWithAesthetic().x(x).y(y));
  }

  /// Create a new Points plot and return a shared pointer to it.
  /// \param data the `DataWithAesthetic` dataset to use
  /// \param transform (optional) the transform to apply
  /// \return shared pointer to the new plot
  std::shared_ptr<Plot1D>
  points(const DataWithAesthetic &data,
         const Transform &transform = Transform(Identity())) {
    return plot_impl(std::make_shared<Points>(*this), transform, data);
  }

  /// Create a new Line and return a shared pointer to it.
  /// \param data the `DataWithAesthetic` dataset to use
  /// \param transform (optional) the transform to apply
  /// \return shared pointer to the new plot
  std::shared_ptr<Plot1D>
  line(const DataWithAesthetic &data,
       const Transform &transform = Transform(Identity())) {
    return plot_impl(std::make_shared<Line>(*this), transform, data);
  }

  /// Create a new histogram and return a shared pointer to it.
  /// \param data the `DataWithAesthetic` dataset to use
  /// \param transform (optional) the transform to apply
  /// \return shared pointer to the new plot
  std::shared_ptr<Plot1D>
  histogram(const DataWithAesthetic &data,
            const Transform &transform = Transform(BinX())) {
    return plot_impl(std::make_shared<Histogram>(*this), transform, data);
  }

  /// Return a shared pointer to an existing plot.
  /// Throws std::out_of_range exception if out of range.
  /// \param n the plot to return
  /// \return a shared pointer to the nth plot
  std::shared_ptr<Plot1D> plot(int n);

  template <typename AnimatedBackend> void draw(AnimatedBackend &backend);
  template <typename Backend> void draw(Backend &backend, float time);

  /// convert from display coordinates to data coordinates, using the given
  /// Aesthetic
  template <typename Aesthetic> float from_display(const float i) const {
    return Aesthetic::from_display(i, m_limits, m_pixels);
  }

  /// convert from data coordinates to display coordinates, using the given
  /// Aesthetic
  template <typename Aesthetic> float to_display(const float i) const {
    return Aesthetic::to_display(i, m_limits, m_pixels);
  }

  void font_face(const std::string &fontFace) { m_font_face = fontFace; }

private:
  /// Create a new Plot1D on this axis and return a shared pointer to it.
  ///
  /// All the plotting functions go through this function, which does the actual
  /// work of adding the new Plot1D, its data and transform to the axis. A
  /// default color is given to the new plot based on the number of already
  /// existing plots (see Colors.hpp for list of default colors)
  ///
  /// \param data the `DataWithAesthetic` dataset to use
  /// \param transform the transform to apply
  /// \return shared pointer to the new plot
  std::shared_ptr<Plot1D> plot_impl(const std::shared_ptr<Plot1D> &plot,
                                    const Transform &transform,
                                    const DataWithAesthetic &values);

  void update_tick_information();
  vfloat2_t calculate_num_ticks();

  template <typename Backend> void draw_common(Backend &backend);
  template <typename Backend> void draw_common_axis_box(Backend &backend);
  template <typename Backend> void draw_common_ticks(Backend &backend);
  template <typename Backend> void draw_common_gridlines(Backend &backend);
  template <typename Backend> void draw_common_title(Backend &backend);
  template <typename Backend> void draw_common_xlabel(Backend &backend);
  template <typename Backend> void draw_common_ylabel(Backend &backend);
  template <typename Backend> void draw_common_legend(Backend &backend);

  void set_auto_ticks();
};

} // namespace trase

#endif // AXIS_H_
