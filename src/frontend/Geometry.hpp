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

/// \file Geometry.hpp

#ifndef GEOMETRY_H_
#define GEOMETRY_H_

#include <memory>
#include <vector>

#include "frontend/Data.hpp"
#include "frontend/Drawable.hpp"
#include "frontend/Transform.hpp"
#include "util/BBox.hpp"
#include "util/Colors.hpp"
#include "util/Exception.hpp"

namespace trase {

// forward declare to be able to store a pointer in Axis
class Axis;

class Geometry : public Drawable {
protected:
  /// dataset
  std::vector<DataWithAesthetic> m_data;

  /// label
  std::string m_label;

  /// colormap
  const Colormap *m_colormap;

  /// min/max limits of m_data across all frames
  Limits m_limits;

  /// transform
  Transform m_transform;

  /// parent axis
  Axis *m_axis;

public:
  explicit Geometry(Axis *parent);

  /// Adds a new data frame to this plot
  ///
  /// The limits of the new data frame will be added to the limits of this
  /// plot, and the parent axis
  ///
  /// \param data the new data frame
  /// \param time the timestamp for this frame. This must be greater than the
  /// time for all previously added frames
  void add_frame(const DataWithAesthetic &data, float time);

  float get_time(const int i) const { return m_times[i]; }

  const DataWithAesthetic &get_data(const int i) const { return m_data[i]; }
  DataWithAesthetic &get_data(const int i) { return m_data[i]; }
  size_t data_size() const { return m_data.size(); }

  /// Sets the transform
  ///
  /// \param transform the new transform
  ///
  /// All new data frames added to the plot will have this transform applied
  /// before the data is stored internally
  void set_transform(const Transform &transform) { m_transform = transform; }

  /// Set the label
  ///
  /// This label describes the plot and is shown on the axis legend
  ///
  /// \param label a string description of the plot
  void set_label(const std::string &label) { m_label = label; }

  const std::string &get_label() const { return m_label; }
  const Colormap &get_colormap() const { return *m_colormap; }

#ifdef TRASE_BACKEND_GL
  virtual void dispatch_legend(BackendGL &figure, float time,
                               const bfloat2_t &box) = 0;
#endif
  virtual void dispatch_legend(BackendSVG &file, float time,
                               const bfloat2_t &box) = 0;
  virtual void dispatch_legend(BackendSVG &file, const bfloat2_t &box) = 0;

  template <typename AnimatedBackend> void draw(AnimatedBackend &backend);
  template <typename Backend> void draw(Backend &backend, float time);
  template <typename AnimatedBackend>
  void draw_legend(AnimatedBackend &backend, const bfloat2_t &box);
  template <typename Backend>
  void draw_legend(Backend &backend, float time, const bfloat2_t &box);
};

} // namespace trase

#define TRASE_DISPATCH_LEGEND(backend_type)                                    \
  void dispatch_legend(backend_type &backend, float time,                      \
                       const bfloat2_t &box) override {                        \
    draw_legend(backend, time, box);                                           \
  }

#define TRASE_ANIMATED_DISPATCH_LEGEND(backend_type)                           \
  void dispatch_legend(backend_type &backend, const bfloat2_t &box) override { \
    draw_legend(backend, box);                                                 \
  }

#define TRASE_DISPATCH_LEGEND_SVG                                              \
  TRASE_DISPATCH_LEGEND(BackendSVG)                                            \
  TRASE_ANIMATED_DISPATCH_LEGEND(BackendSVG)

#ifdef TRASE_BACKEND_GL
#define TRASE_DISPATCH_LEGEND_GL TRASE_DISPATCH_LEGEND(BackendGL)
#else
#define TRASE_DISPATCH_LEGEND_GL
#endif

#define TRASE_GEOMETRY_DISPATCH_BACKENDS                                       \
  TRASE_DISPATCH_SVG                                                           \
  TRASE_DISPATCH_GL                                                            \
  TRASE_DISPATCH_LEGEND_SVG                                                    \
  TRASE_DISPATCH_LEGEND_GL

#include "frontend/Geometry.tcc"

#endif // PLOT1D_H_
