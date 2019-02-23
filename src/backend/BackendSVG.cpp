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

#include "backend/BackendSVG.hpp"

namespace trase {

BackendSVG::BackendSVG(std::ostream &out) : m_out(out) {
  stroke_color({0, 0, 0, 255});
  fill_color({0, 0, 0, 255});
  stroke_width(1);
}

bool BackendSVG::mouseover() const noexcept {
  return !m_onmouseover_fill.empty() || !m_onmouseover_stroke.empty() ||
         !m_onmouseout_tooltip.empty();
}

void BackendSVG::init(const vfloat2_t &pixels, const char *name,
                      const float time_span) noexcept {
  m_time_span = time_span;
  m_out << R"del(<?xml version="1.0" encoding="utf-8" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN"
  "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
)del";

  m_out << "<svg width=\"" << pixels[0] << "px\" height=\"" << pixels[1]
        << "px\" version=\"1.1\" xmlns=\"http://www.w3.org/2000/svg\">\n";

  m_out << "<desc>" << name << "</desc>\n";

  if (!m_web_font.empty()) {
    m_out << "<style type=\"text/css\">@import url('" + m_web_font +
                 "');</style>\n";
  }
  m_out << R"del(<script>
function tooltip(x,y,string,size,face) {
    var txtElem = document.createElementNS("http://www.w3.org/2000/svg", "text");
    txtElem.setAttributeNS(null,"id","tooltip");
    txtElem.setAttributeNS(null,"x",x);
    txtElem.setAttributeNS(null,"y",y);
    txtElem.setAttributeNS(null,"font-size",size);
    txtElem.setAttributeNS(null,"font-family",face);

    txtElem.appendChild(document.createTextNode(string))
    document.documentElement.appendChild(txtElem);
}
function remove_tooltip() {
    var txtElem = document.getElementById("tooltip");
    document.documentElement.removeChild(txtElem);
}
</script>
)del";
}

bool BackendSVG::is_interactive() { return false; }

bool BackendSVG::should_close() { return true; }

vfloat2_t BackendSVG::begin_frame() { return vfloat2_t(); }

void BackendSVG::end_frame() {}

vfloat2_t BackendSVG::get_mouse_pos() { return vfloat2_t(0, 0); }

float BackendSVG::get_time() { return 0.f; }

void BackendSVG::set_mouse_down(const vfloat2_t &mouse_pos) {}

void BackendSVG::set_mouse_up() {}

bool BackendSVG::mouse_dragging() { return false; }

vfloat2_t BackendSVG::mouse_drag_delta() { return vfloat2_t(); }

void BackendSVG::mouse_drag_reset_delta() {}

void BackendSVG::scissor(const bfloat2_t &x) {}

void BackendSVG::reset_scissor() {}

void BackendSVG::rotate(const float angle) { m_transform.rotate(angle); }
void BackendSVG::reset_transform() { m_transform.clear(); }
void BackendSVG::translate(const vfloat2_t &v) { m_transform.translate(v); }

void BackendSVG::finalise() noexcept {
  m_out << "</svg>\n";
  m_out.flush();
}

void BackendSVG::begin_animated_path() {
  if (m_animate_values.empty()) {
    m_animate_values.resize(1);
  }
  m_animate_times = "keyTimes=\"";
  m_animate_values[0] = "values=\"";
  begin_path();
}
void BackendSVG::add_animated_path(const float time) {

  // if its the first frame need to write the path element
  // TODO: not sure why need to add d attribute here, but neccessary for
  // single frame animations. Figure out how to remove this as it doubles the
  // required space
  if (m_animate_times.back() == '\"') {
    m_out << "<path " << m_line_color << ' ' << m_linewidth
          << " fill-opacity=\"0\" d=\"" << m_path << "\">\n ";
  }

  // all times are scaled by total time span (all times start at 0)
  m_animate_times += std::to_string(time / m_time_span) + ';';
  m_animate_values[0] += m_path + ';';
  m_path.clear();
}

void BackendSVG::end_animated_path(const float time) {
  add_animated_path(time);
  m_animate_times.back() = '\"';
  end_animate(m_animate_values[0], "d");
  end_animate_fill();
  end_animate_stroke();
  m_out << "</path>\n";
  m_animate_times.clear();
}

void BackendSVG::begin_path() { m_path.clear(); }

void BackendSVG::rounded_rect(const bfloat2_t &x, const float r) noexcept {
  rect(x, r);
}

void BackendSVG::add_animated_circle(const vfloat2_t &centre, float radius,
                                     float time) {
  // check if first circle
  if (m_animate_times.empty()) {

    circle_begin(centre, radius);

    if (m_animate_values.size() < 3) {
      m_animate_values.resize(3);
    }
    m_animate_times = "keyTimes=\"" + std::to_string(time / m_time_span) + ';';
    m_animate_values[0] = "values=\"" + std::to_string(centre[0]) + ';';
    m_animate_values[1] = "values=\"" + std::to_string(centre[1]) + ';';
    m_animate_values[2] = "values=\"" + std::to_string(radius) + ';';
  } else {
    m_animate_times += std::to_string(time / m_time_span) + ';';
    m_animate_values[0] += std::to_string(centre[0]) + ';';
    m_animate_values[1] += std::to_string(centre[1]) + ';';
    m_animate_values[2] += std::to_string(radius) + ';';
  }
}

void BackendSVG::end_animated_circle() {
  m_animate_times.back() = '\"';
  const std::string names[3] = {"cx", "cy", "r"};
  for (int i = 0; i < 3; ++i) {
    end_animate(m_animate_values[i], names[i]);
  }
  end_animate_fill();
  end_animate_stroke();
  circle_end();
  m_animate_times.clear();
}

void BackendSVG::circle_with_text(const vfloat2_t &centre, float radius,
                                  const char *string) {
  m_out << "<circle cx=\"" << centre[0] << "\" cy=\"" << centre[1] << "\" r=\""
        << radius << "\" " << m_fill_color << ' ' << m_line_color << ' '
        << m_linewidth
        << " onmouseover=\"evt.target.setAttribute('stroke-opacity','1.0');"
           "\" onmouseout=\"bob.setAttribute('stroke-opacity', '0.0');"
           "\"/>\n";
  auto text_pos = centre + 2.f * vfloat2_t(radius, -radius);
  m_out << "<text id=\"bob\" x=\"" << text_pos[0] << "\" y=\"" << text_pos[1]
        << "\" " << m_font_face << ' ' << m_font_size << ' ' << m_font_align
        << ' ' << m_fill_color << '>' << string << "</text>\n";
}

void BackendSVG::arc(const vfloat2_t &centre, const float radius,
                     const float angle0, const float angle1) {
  const vfloat2_t p0 =
      centre + radius * vfloat2_t(std::cos(angle0), std::sin(angle0));
  const vfloat2_t p1 =
      centre + radius * vfloat2_t(std::cos(angle1), std::sin(angle1));
  move_to(p0);
  m_path += " A " + std::to_string(radius) + ' ' + std::to_string(radius) +
            " 0 0 1 " + std::to_string(p1[0]) + ' ' + std::to_string(p1[1]);
}

void BackendSVG::move_to(const vfloat2_t &x) {
  m_path += " M " + std::to_string(x[0]) + ' ' + std::to_string(x[1]);
}
void BackendSVG::line_to(const vfloat2_t &x) {
  m_path += " L " + std::to_string(x[0]) + ' ' + std::to_string(x[1]);
}
void BackendSVG::close_path() { m_path += " Z"; }

void BackendSVG::stroke_color(const RGBA &color) {
  m_line_color = std::string("stroke=\"") + color.to_rgb_string() +
                 "\" stroke-opacity=\"" + std::to_string(color.a() / 255.0) +
                 '\"';
  m_onmouseover_stroke.clear();
  m_onmouseout_stroke.clear();
}

void BackendSVG::stroke_color(const RGBA &color, const RGBA &color_mouseover) {
  stroke_color(color);
  m_onmouseover_stroke = "evt.target.setAttribute('stroke', '" +
                         color_mouseover.to_rgb_string() +
                         "'); evt.target.setAttribute('stroke-opacity','" +
                         std::to_string(color_mouseover.a() / 255.0) + "');";
  m_onmouseout_stroke = "evt.target.setAttribute('stroke', '" +
                        color.to_rgb_string() +
                        "'); evt.target.setAttribute('stroke-opacity','" +
                        std::to_string(color.a() / 255.0) + "');";
}

void BackendSVG::tooltip(const vfloat2_t &x, const char *string) {
  m_onmouseover_tooltip = "tooltip(" + std::to_string(x[0]) + ',' +
                          std::to_string(x[1]) + ",'" + string + "'," +
                          m_font_size_base + ",'" + m_font_face_base + "');";
  m_onmouseout_tooltip = "remove_tooltip();";
}

void BackendSVG::clear_tooltip() {
  m_onmouseover_tooltip.clear();
  m_onmouseout_tooltip.clear();
}

void BackendSVG::stroke_width(const float lw) {
  m_linewidth =
      std::string("stroke-width=\"") + std::to_string(lw) + std::string("\"");
}
void BackendSVG::fill_color(const RGBA &color) {
  m_fill_color = "fill=\"" + color.to_rgb_string() + "\" fill-opacity=\"" +
                 std::to_string(color.a() / 255.0) + '\"';
}

void BackendSVG::stroke() {
  m_out << "<path d=\"" << m_path << "\" " << m_line_color << ' ' << m_linewidth
        << " fill-opacity=\"0\"";
  if (!m_transform.is_identity()) {
    m_out << ' ' << m_transform.to_string();
  }
  m_out << "/>\n";
}
void BackendSVG::fill() {
  m_out << "<path d=\"" << m_path << "\" " << m_fill_color << ' '
        << m_line_color << ' ' << m_linewidth;
  if (!m_transform.is_identity()) {
    m_out << ' ' << m_transform.to_string();
  }
  m_out << "/>\n";
  m_onmouseover_fill.clear();
  m_onmouseout_fill.clear();
}

void BackendSVG::fill_color(const RGBA &color, const RGBA &color_mouseover) {
  fill_color(color);
  m_onmouseover_fill = "evt.target.setAttribute('fill', '" +
                       color_mouseover.to_rgb_string() +
                       "'); evt.target.setAttribute('fill-opacity','" +
                       std::to_string(color_mouseover.a() / 255.0) + "');";
  m_onmouseout_fill = "evt.target.setAttribute('fill', '" +
                      color.to_rgb_string() +
                      "'); evt.target.setAttribute('fill-opacity','" +
                      std::to_string(color.a() / 255.0) + "');";
}

void BackendSVG::font_size(float size) {
  m_font_size_base = std::to_string(size);
  m_font_size = "font-size=\"" + std::to_string(size) + '\"';
}

void BackendSVG::font_face(const char *face) {
  m_font_face_base = std::string(face);
  m_font_face = "font-family=\"" + std::string(face) + '\"';
}

void BackendSVG::import_web_font(const std::string &url) { m_web_font = url; }

void BackendSVG::font_blur(const float blur) {}
void BackendSVG::text_align(const unsigned int align) {
  std::string align_text;
  if (align & ALIGN_LEFT) {
    align_text = "start";
  } else if (align & ALIGN_CENTER) {
    align_text = "middle";
  } else if (align & ALIGN_RIGHT) {
    align_text = "end";
  }
  std::string vert_align_text;
  if (align & ALIGN_TOP) {
    vert_align_text = "hanging";
  } else if (align & ALIGN_MIDDLE) {
    vert_align_text = "middle";
  } else if (align & ALIGN_BOTTOM) {
    vert_align_text = "baseline";
  }
  m_font_align = "text-anchor=\"" + align_text + "\" alignment-baseline=\"" +
                 vert_align_text + '\"';
}

void BackendSVG::text(const vfloat2_t &x, const char *string, const char *end) {
  m_out << "<text x=\"" << x[0] << "\" y=\"" << x[1] << "\" " << m_font_face
        << ' ' << m_font_size << ' ' << m_font_align << ' ' << m_fill_color;
  if (!m_transform.is_identity()) {
    m_out << ' ' << m_transform.to_string();
  }
  m_out << '>' << string << "</text>\n";
}

void BackendSVG::rect_begin(const bfloat2_t &x, float r) noexcept {

  const auto &delta = x.delta();
  vfloat2_t min = x.min();

  m_out << "<rect ";

  // Position, width and height
  m_out << m_att("x", min[0]) << m_att("y", min[1]) << m_att("width", delta[0])
        << m_att("height", delta[1]);

  // Rounding corners
  if (r > 0.f) {
    m_out << m_att("rx", r) << m_att("ry", r);
  }

  shape_styling();

  m_out << ">\n";
}

void BackendSVG::rect_end() noexcept { m_out << "</rect>\n"; }

void BackendSVG::rect(const bfloat2_t &x, const float r) noexcept {
  rect_begin(x, r);
  rect_end();
}

void BackendSVG::add_animated_rect(const bfloat2_t &x, float time) {

  const auto &delta = x.delta();
  vfloat2_t min = x.min();

  // check if first rect
  if (m_animate_times.empty()) {
    rect_begin(x, 0.f);

    if (m_animate_values.size() < 4) {
      m_animate_values.resize(4);
    }
    m_animate_times = "keyTimes=\"" + std::to_string(time / m_time_span) + ';';
    m_animate_values[0] = "values=\"" + std::to_string(min[0]) + ';';
    m_animate_values[1] = "values=\"" + std::to_string(min[1]) + ';';
    m_animate_values[2] = "values=\"" + std::to_string(delta[0]) + ';';
    m_animate_values[3] = "values=\"" + std::to_string(delta[1]) + ';';
  } else {
    m_animate_times += std::to_string(time / m_time_span) + ';';
    m_animate_values[0] += std::to_string(min[0]) + ';';
    m_animate_values[1] += std::to_string(min[1]) + ';';
    m_animate_values[2] += std::to_string(delta[0]) + ';';
    m_animate_values[3] += std::to_string(delta[1]) + ';';
  }
}

void BackendSVG::add_animated_stroke(const RGBA &color) {
  if (m_animate_stroke.empty()) {
    m_animate_stroke = "values=\"" + color.to_rgb_string() + ';';
    m_animate_stroke_opacity =
        "values=\"" + std::to_string(color.a() / 255.0) + ';';
  } else {
    m_animate_stroke += color.to_rgb_string() + ';';
    m_animate_stroke_opacity += std::to_string(color.a() / 255.0) + ';';
  }
}

void BackendSVG::end_animate(std::string &animate, const std::string &name) {
  animate.back() = '\"';
  m_out << "<animate attributeName=\"" + name +
               "\" repeatCount=\"indefinite\" begin =\"0s\" dur=\""
        << m_time_span << "s\" " << animate << ' ' << m_animate_times << "/>\n";

  animate.clear();
}

void BackendSVG::end_animate_stroke() {
  if (m_animate_stroke.empty()) {
    return;
  }
  end_animate(m_animate_stroke, "stroke");
  end_animate(m_animate_stroke_opacity, "stroke-opacity");
}

void BackendSVG::add_animated_fill(const RGBA &color) {
  if (m_animate_fill.empty()) {
    m_animate_fill = "values=\"" + color.to_rgb_string() + ';';
    m_animate_fill_opacity =
        "values=\"" + std::to_string(color.a() / 255.0) + ';';
  } else {
    m_animate_fill += color.to_rgb_string() + ';';
    m_animate_fill_opacity += std::to_string(color.a() / 255.0) + ';';
  }
}

void BackendSVG::end_animate_fill() {
  if (m_animate_fill.empty()) {
    return;
  }
  end_animate(m_animate_fill, "fill");
  end_animate(m_animate_fill_opacity, "fill-opacity");
}

void BackendSVG::end_animated_rect() {
  m_animate_times.back() = '\"';
  const std::string names[4] = {"x", "y", "width", "height"};
  for (int i = 0; i < 4; ++i) {
    end_animate(m_animate_values[i], names[i]);
  }
  end_animate_fill();
  end_animate_stroke();
  rect_end();
  m_animate_times.clear();
}

void BackendSVG::shape_styling() {
  // Styling
  m_out << m_fill_color << ' ' << m_line_color << ' ' << m_linewidth;

  if (mouseover()) {
    m_out << " onmouseover=\"" << m_onmouseover_fill << m_onmouseover_stroke
          << m_onmouseover_tooltip << '\"';
    m_out << " onmouseout=\"" << m_onmouseout_fill << m_onmouseout_stroke
          << m_onmouseout_tooltip << '\"';
  }
}

void BackendSVG::circle_begin(const vfloat2_t &centre, const float r) noexcept {

  m_out << "<circle ";
  m_out << m_att("cx", centre[0]) << m_att("cy", centre[1]) << m_att("r", r);

  shape_styling();

  m_out << ">\n";
}

void BackendSVG::circle_end() noexcept { m_out << "</circle>\n"; }

void BackendSVG::circle(const vfloat2_t &centre, float r) noexcept {
  circle_begin(centre, r);
  circle_end();
}

} // namespace trase
