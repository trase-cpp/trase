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
#include "frontend/Geometry.hpp"

#include "util/Colors.hpp"
#include "util/Exception.hpp"

namespace trase {

// forward declare since Axis has some functions returning legends
class Legend;

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

/// An 2D axis that can contain zero or more Geometry objects
///
/// Each axis stores minimum and maximum limits for each Aesthetic, these are
/// used to both set the ticks displayed on the x and y axis, and to scale the
/// drawn Geometry objects (how each Geometry is scaled depends on the specifics
/// of the Geometry used). Each time a new Geometry is added to the Axis, it
/// updates these limits using the limits of the its DataWithAestheic.
///
/// TODO: issue #87, the name of the Geometry class will probably change to
/// Geometry in the future
///
/// An Axis can optionally contain:
///     * a title
///     * a label for the x axis
///     * a label for the y axis
///     * a legend that identifies each Geometry
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

  /// the label displayed on the x axis
  std::string m_xlabel;

  /// the label displayed on the y axis
  std::string m_ylabel;

  /// the Axis title
  std::string m_title;

  /// tick helper
  TickInfo m_tick_info;

  /// true if axis has a Legend
  bool m_has_legend;

public:
  /// create a new Axis contained in the given Figure
  /// \param figure the parent \Drawable object
  /// \param area the drawable area assigned to this Drawable as a ratio of the
  /// parent size
  Axis(Drawable *parent, const bfloat2_t &area);

  virtual ~Axis() = default;

  TRASE_DISPATCH_BACKENDS

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

  /// show a legend identifying each Geometry in the Axis
  std::shared_ptr<Legend> legend();

  /// Create a new Points plot and return a shared pointer to it.
  /// \param data the `DataWithAesthetic` dataset to use
  /// \param transform (optional) the transform to apply
  /// \return shared pointer to the new plot
  std::shared_ptr<Geometry>
  points(const DataWithAesthetic &data,
         const Transform &transform = Transform(Identity()));

  /// Create a new Rectangle plot and return a shared pointer to it.
  /// \param data the `DataWithAesthetic` dataset to use
  /// \param transform (optional) the transform to apply
  /// \return shared pointer to the new plot
  std::shared_ptr<Geometry>
  rectangle(const DataWithAesthetic &data,
            const Transform &transform = Transform(Identity()));

  /// Create a new Line and return a shared pointer to it.
  /// \param data the `DataWithAesthetic` dataset to use
  /// \param transform (optional) the transform to apply
  /// \return shared pointer to the new plot
  std::shared_ptr<Geometry>
  line(const DataWithAesthetic &data,
       const Transform &transform = Transform(Identity()));

  /// Create a new histogram and return a shared pointer to it.
  /// \param data the `DataWithAesthetic` dataset to use
  /// \param transform (optional) the transform to apply
  /// \return shared pointer to the new plot
  std::shared_ptr<Geometry>
  histogram(const DataWithAesthetic &data,
            const Transform &transform = Transform(BinX()));

  /// Return a shared pointer to an existing plot.
  /// Throws std::out_of_range exception if out of range.
  /// \param n the plot to return
  /// \return a shared pointer to the nth plot
  std::shared_ptr<Geometry> plot(int n);

  template <typename AnimatedBackend> void draw(AnimatedBackend &backend);
  template <typename Backend> void draw(Backend &backend, float time);

  /// convert from display coordinates to data coordinates, using the given
  /// Aesthetic
  template <typename Aesthetic> float from_display(float i) const {
    return Aesthetic::from_display(i, m_limits, m_pixels);
  }

  /// convert from data coordinates to display coordinates, using the given
  /// Aesthetic
  template <typename Aesthetic> float to_display(float i) const {
    return Aesthetic::to_display(i, m_limits, m_pixels);
  }

  /// set the number of ticks on this axis
  /// \param arg a length 2 int vector with the requested number of ticks along
  /// each axis (i.e. [x_ticks,y_ticks]). Setting the number of ticks on either
  /// axis to zero uses the default number of ticks
  void set_ticks(Vector<int, 2> arg) {
    m_nx_ticks = arg[0];
    m_ny_ticks = arg[1];
  }

  /// gets the number of ticks on this axis
  Vector<int, 2> get_ticks() const { return {m_nx_ticks, m_ny_ticks}; }

private:
  /// Create a new Geometry on this axis and return a shared pointer to it.
  ///
  /// All the plotting functions go through this function, which does the actual
  /// work of adding the new Geometry, its data and transform to the axis. A
  /// default color is given to the new plot based on the number of already
  /// existing plots (see Colors.hpp for list of default colors)
  ///
  /// \param data the `DataWithAesthetic` dataset to use
  /// \param transform the transform to apply
  /// \return shared pointer to the new plot
  std::shared_ptr<Geometry> plot_impl(const std::shared_ptr<Geometry> &plot,
                                      const Transform &transform,
                                      const DataWithAesthetic &values);

  void update_tick_information();
  vint2_t calculate_num_ticks();
  void add_geometry_to_legend(const std::shared_ptr<Geometry> &geometry);
  std::shared_ptr<Legend> add_legend();

  template <typename Backend> void draw_common(Backend &backend);
  template <typename Backend> void draw_common_axis_box(Backend &backend);
  template <typename Backend> void draw_common_ticks(Backend &backend);
  template <typename Backend> void draw_common_gridlines(Backend &backend);
  template <typename Backend> void draw_common_title(Backend &backend);
  template <typename Backend> void draw_common_xlabel(Backend &backend);
  template <typename Backend> void draw_common_ylabel(Backend &backend);
};

} // namespace trase

#include "frontend/Axis.tcc"

#endif // AXIS_H_
