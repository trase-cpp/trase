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

#include "frontend/Axis.hpp"
#include "frontend/Figure.hpp"
#include "util/Vector.hpp"

namespace trase {

Axis::Axis(Figure &figure, const bfloat2_t &area)
    : Drawable(&figure, area), m_sig_digits(2), m_nx_ticks(0), m_ny_ticks(0),
      m_tick_len(10.f), m_line_width(3.f), m_font_size(18.f),
      m_font_face("Roboto"), m_legend(false) {}

std::shared_ptr<Plot1D> Axis::plot(int n) { return m_plot1d.at(n); }

std::shared_ptr<Plot1D> Axis::plot_impl(const std::shared_ptr<Plot1D> &plot,
                                        const DataWithAesthetic &values) {
  m_plot1d.emplace_back(plot);
  m_children.push_back(m_plot1d.back().get());
  m_plot1d.back()->add_frame(values, 0);
  m_plot1d.back()->set_color(RGBA::defaults[m_plot1d.size() - 1]);
  m_plot1d.back()->resize(m_pixels);
  return m_plot1d.back();
}

} // namespace trase
