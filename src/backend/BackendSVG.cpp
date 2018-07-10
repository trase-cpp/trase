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

void BackendSVG::init(const float width, const float height,
                      const float time_span, const char *name) {
  m_time_span = time_span;
  m_out << R"del(<?xml version="1.0" encoding="utf-8" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN"
  "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
)del";

  m_out << "<svg width=\"" << width << "px\" height=\"" << height
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

void BackendSVG::finalise() {
  m_out << "</svg>\n";
  m_out.flush();
}

} // namespace trase

#include "frontend/DrawableDraw.hpp"

namespace trase {
template void Drawable::serialise<BackendSVG>(BackendSVG &backend);
} // namespace trase

#include "frontend/FigureDraw.hpp"

namespace trase {
template void Figure::serialise<BackendSVG>(BackendSVG &backend);
} // namespace trase

#include "frontend/AxisDraw.hpp"

namespace trase {
template void Axis::serialise<BackendSVG>(BackendSVG &backend);
template void Axis::draw_common<BackendSVG>(BackendSVG &backend);
} // namespace trase

#include "frontend/Plot1DDraw.hpp"

namespace trase {
template void Plot1D::serialise<BackendSVG>(BackendSVG &backend);
} // namespace trase
