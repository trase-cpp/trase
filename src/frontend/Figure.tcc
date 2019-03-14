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

#include <array>
#include <string>

#include "frontend/Figure.hpp"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

namespace trase {

template <typename AnimatedBackend>
void Figure::draw(AnimatedBackend &backend) {
  auto name = "Figure " + std::to_string(m_id);
  backend.init(m_pixels.bmax, name.c_str(), m_time_span);
  for (const auto &i : m_children) {
    i->dispatch(backend);
  }
  backend.finalise();
}

template <typename Backend>
void Figure::render_interactive_frame(Backend &backend) {
  const vfloat2_t win_limits = backend.begin_frame();
  if ((win_limits != m_pixels.bmax).any()) {
    m_pixels.bmax = win_limits;
    for (const auto &i : m_children) {
      i->resize(m_pixels);
    }
  }

  if (backend.mouse_dragging()) {
    vfloat2_t delta = backend.mouse_drag_delta();
    for (const auto &drawable : m_children) {
      auto axis = std::dynamic_pointer_cast<Axis>(drawable);
      // scale by axis pixel area
      vfloat2_t ax_delta = delta / (axis->pixels().bmax * vfloat2_t(-1, 1));

      // scale by axis limits
      ax_delta[0] *= axis->limits().bmax[Aesthetic::x::index] -
                     axis->limits().bmin[Aesthetic::x::index];
      ax_delta[1] *= axis->limits().bmax[Aesthetic::y::index] -
                     axis->limits().bmin[Aesthetic::y::index];

      axis->limits().bmin[Aesthetic::x::index] += ax_delta[0];
      axis->limits().bmax[Aesthetic::x::index] += ax_delta[0];
      axis->limits().bmin[Aesthetic::y::index] += ax_delta[1];
      axis->limits().bmax[Aesthetic::y::index] += ax_delta[1];
    }
    backend.mouse_drag_reset_delta();
  }

  const float time = backend.get_time();
  const float looped_time = std::fmod(time, m_time_span);

  for (const auto &i : m_children) {
    i->dispatch(backend, looped_time);
  }

  backend.end_frame();
}

#ifdef __EMSCRIPTEN__
template <typename Backend> void Figure::emscripten_callback(void *data) {
  using data_t = std::pair<Figure *, Backend *>;
  auto self = static_cast<data_t *>(data)->first;
  auto backend = static_cast<data_t *>(data)->second;
  self->render_interactive_frame(*backend);
}
#endif

template <typename Backend> void Figure::show(Backend &backend) {
  auto name = "Figure " + std::to_string(m_id);
  backend.init(this->m_pixels.bmax, name.c_str());

  if (!backend.is_interactive()) {
    throw Exception("Figure::show() should only be called with interactive "
                    "backends. Use Figure::draw() instead");
  }

// Main loop
#ifdef __EMSCRIPTEN__
  auto data = std::make_pair(this, &backend);
  emscripten_set_main_loop_arg(
      (em_arg_callback_func)&emscripten_callback<Backend>, &data, 0, 1);
#else
  while (!backend.should_close()) {
    render_interactive_frame(backend);
  }
#endif
  backend.finalise();
}

template <typename Backend>
void Figure::draw(Backend &backend, const float time) {
  auto name = "Figure " + std::to_string(m_id);
  backend.init(m_pixels.bmax, name.c_str());
  for (const auto &i : m_children) {
    i->dispatch(backend, time);
  }
  backend.finalise();
}

} // namespace trase
