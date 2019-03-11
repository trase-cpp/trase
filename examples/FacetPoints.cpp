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

/// \page example_facet_points Facet
///  This is an example of faceting a data set for an animation
///
/// \image html
/// https://cdn.jsdelivr.net/gh/trase-cpp/generated_files@master/examples/example_facet_points.svg
/// "Output" \snippet examples/FacetPoints.cpp example facet points

/// [example facet points]
#include "trase.hpp"
#include <fstream>
#include <random>

using namespace trase;

int main() {
  CSVDownloader dl;
  dl.set_delim('\t');
  auto csv = dl.download("https://www.stat.ubc.ca/~jenny/notOcto/STAT545A/"
                         "examples/gapminder/data/gapminderDataFiveYear.txt");

  auto fig = figure();
  auto ax = fig->axis();
  auto data =
      create_data().x(csv["gdpPercap"]).y(csv["lifeExp"]).size(csv["pop"]).color(csv["continent"]);

  data.x(0, 4e4).y(20, 84);

  auto faceted_data = data.facet(csv["year"]);

  auto facet = faceted_data.begin();
  auto points = ax->points(facet->second);

  int i = 0;
  for (;facet != faceted_data.end(); ++facet, ++i) {
    points->add_frame(facet->second, i);
  }

  ax->xlabel("gdpPercap");
  ax->ylabel("lifeExp");

// output to chosen backend
#ifdef TRASE_EXAMPLES_SVG_BACKEND
  std::ofstream out;
  out.open("example_facet_points.svg");
  BackendSVG backend(out);
  fig->draw(backend);
  out.close();
#endif
#ifdef TRASE_EXAMPLES_GL_BACKEND
  BackendGL backend;
  fig->show(backend);
#endif
}
/// [example facet points]
