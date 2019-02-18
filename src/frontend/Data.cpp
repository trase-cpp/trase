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

#include "frontend/Data.hpp"

namespace trase {

template <>
float RawData::cast_to_float<std::string>(const std::string &arg) const {
  return std::stof(arg);
}

ColumnIterator RawData::begin(const int i) const {
  if (i < 0 || i >= cols()) {
    throw std::out_of_range("column does not exist");
  }
  return {m_matrix.cbegin() + i, m_cols};
}

ColumnIterator RawData::end(const int i) const {
  if (i < 0 || i >= cols()) {
    throw std::out_of_range("column does not exist");
  }
  return {m_matrix.cend() + i, m_cols};
}

int DataWithAesthetic::rows() const { return m_data->rows(); }

int DataWithAesthetic::cols() const { return m_data->cols(); }

const Limits &DataWithAesthetic::limits() const { return m_limits; }

template <typename Aesthetic> ColumnIterator DataWithAesthetic::begin() const {

  auto search = m_map.find(Aesthetic::index);

  if (search == m_map.end()) {
    throw Exception(Aesthetic::name + std::string(" aestheic not provided"));
  }
  return m_data->begin(search->second);
}

template <typename Aesthetic> ColumnIterator DataWithAesthetic::end() const {

  auto search = m_map.find(Aesthetic::index);

  if (search == m_map.end()) {
    throw Exception(Aesthetic::name + std::string(" aestheic not provided"));
  }
  return m_data->end(search->second);
}

template ColumnIterator DataWithAesthetic::begin<Aesthetic::x>() const;
template ColumnIterator DataWithAesthetic::begin<Aesthetic::y>() const;
template ColumnIterator DataWithAesthetic::begin<Aesthetic::color>() const;
template ColumnIterator DataWithAesthetic::begin<Aesthetic::size>() const;

template ColumnIterator DataWithAesthetic::end<Aesthetic::x>() const;
template ColumnIterator DataWithAesthetic::end<Aesthetic::y>() const;
template ColumnIterator DataWithAesthetic::end<Aesthetic::color>() const;
template ColumnIterator DataWithAesthetic::end<Aesthetic::size>() const;

const int Aesthetic::N;
const int Aesthetic::x::index;
const char *Aesthetic::x::name = "x";
const int Aesthetic::y::index;
const char *Aesthetic::y::name = "y";
const int Aesthetic::color::index;
const char *Aesthetic::color::name = "color";
const int Aesthetic::size::index;
const char *Aesthetic::size::name = "size";

float Aesthetic::x::to_display(const float data, const Limits &data_lim,
                               const bfloat2_t &display_lim) {
  float len_ratio = (display_lim.bmax[0] - display_lim.bmin[0]) /
                    (data_lim.bmax[index] - data_lim.bmin[index]);

  float rel_pos = data - data_lim.bmin[index];
  return display_lim.bmin[0] + rel_pos * len_ratio;
}

float Aesthetic::x::from_display(const float display, const Limits &data_lim,
                                 const bfloat2_t &display_lim) {
  float len_ratio = (data_lim.bmax[index] - data_lim.bmin[index]) /
                    (display_lim.bmax[1] - display_lim.bmin[1]);

  float rel_pos = display - display_lim.bmin[1];
  return data_lim.bmin[index] + rel_pos * len_ratio;
}

/// the data to display on the y-axis of the plot
float Aesthetic::y::to_display(const float data, const Limits &data_lim,
                               const bfloat2_t &display_lim) {
  float len_ratio = (display_lim.bmax[1] - display_lim.bmin[1]) /
                    (data_lim.bmax[index] - data_lim.bmin[index]);

  // Get the relative position and invert y by default (e.g. limits->pixels)
  float rel_pos = data_lim.bmax[index] - data;
  return display_lim.bmin[1] + rel_pos * len_ratio;
}

float Aesthetic::y::from_display(const float display, const Limits &data_lim,
                                 const bfloat2_t &display_lim) {
  float len_ratio = (data_lim.bmax[index] - data_lim.bmin[index]) /
                    (display_lim.bmax[1] - display_lim.bmin[1]);

  float rel_pos = display_lim.bmax[1] - display;
  return data_lim.bmin[index] + rel_pos * len_ratio;
}

float Aesthetic::color::to_display(const float data, const Limits &data_lim,
                                   const bfloat2_t &display_lim) {
  (void)display_lim;
  float len_ratio = 1.f / (data_lim.bmax[index] - data_lim.bmin[index]);

  float rel_pos = data - data_lim.bmin[index];
  return rel_pos * len_ratio;
}

float Aesthetic::color::from_display(const float display,
                                     const Limits &data_lim,
                                     const bfloat2_t &display_lim) {
  (void)display_lim;
  float len_ratio = (data_lim.bmax[index] - data_lim.bmin[index]);

  float rel_pos = display;
  return data_lim.bmin[index] + rel_pos * len_ratio;
}

float Aesthetic::size::to_display(const float data, const Limits &data_lim,
                                  const bfloat2_t &display_lim) {
  float len_ratio = 0.05f * (display_lim.bmax[1] - display_lim.bmin[1]) /
                    (data_lim.bmax[index] - data_lim.bmin[index]);

  float rel_pos = data - data_lim.bmin[index];
  return 1.f + rel_pos * len_ratio;
}

float Aesthetic::size::from_display(const float display, const Limits &data_lim,
                                    const bfloat2_t &display_lim) {
  float len_ratio = 20.f * (data_lim.bmax[index] - data_lim.bmin[index]) /
                    (display_lim.bmax[1] - display_lim.bmin[1]);

  float rel_pos = display - 1.f;
  return data_lim.bmin[index] + rel_pos * len_ratio;
}

template <typename Aesthetic>
void DataWithAesthetic::set(const float min, const float max) {
  m_limits.bmin[Aesthetic::index] = min;
  m_limits.bmax[Aesthetic::index] = max;
}

DataWithAesthetic &DataWithAesthetic::x(const float min, const float max) {
  set<Aesthetic::x>(min, max);
  return *this;
}

DataWithAesthetic &DataWithAesthetic::y(const float min, const float max) {
  set<Aesthetic::y>(min, max);
  return *this;
}

DataWithAesthetic &DataWithAesthetic::color(const float min, const float max) {
  set<Aesthetic::color>(min, max);
  return *this;
}

DataWithAesthetic &DataWithAesthetic::size(const float min, const float max) {
  set<Aesthetic::size>(min, max);
  return *this;
}

DataWithAesthetic create_data() { return DataWithAesthetic(); }

} // namespace trase
