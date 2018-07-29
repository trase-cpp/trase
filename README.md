# trase

[![codecov](https://codecov.io/gh/martinjrobins/trase/branch/master/graph/badge.svg)](https://codecov.io/gh/martinjrobins/trase)
[![Build
Status](https://travis-ci.org/martinjrobins/trase.svg?branch=master)](https://travis-ci.org/martinjrobins/trase)
[![Build 
status](https://ci.appveyor.com/api/projects/status/kfm43tg6qltyjsyl/branch/master?svg=true)](https://ci.appveyor.com/project/martinjrobins/trase/branch/master)
[![BCH 
compliance](https://bettercodehub.com/edge/badge/trase-cpp/trase?branch=master)](https://bettercodehub.com/)
[![codedocs.xyz](https://codedocs.xyz/martinjrobins/trase.svg)](https://codedocs.xyz/martinjrobins/trase/)

Trase is a lightweight scientific plotting library for C++ with animation 
support. It enables you to construct plots and write them out to animated svg 
files, or display them in an OpenGL window. The main library and svg backend 
have no dependencies other than the standard library. The OpenGL backend 
requires [GLFW](http://www.glfw.org/).

<p align="center">
  <img width="600" 
  src="https://rawgit.com/martinjrobins/trase/master/test_figure.svg">
</p>


For example, the above svg image was generated with the following code.

```cpp
  auto fig = figure();
  auto ax = fig->axis();
  const int n = 100;
  const int nframes = 10;
  std::vector<float> x(n);
  std::vector<float> y(n);

  // define x points
  for (int i = 0; i < n; ++i) {
    x[i] = static_cast<float>(i) / n;
  }

  // define y = sin(x) with given amplitude and frequency
  auto write_y = [&](const float amplitude, const float freq) {
    for (int i = 0; i < n; ++i) {
      y[i] = amplitude * std::sin(6.28f * freq * x[i]);
    }
  };

  // create a static sin(x) function
  write_y(1.f, 2.f);
  auto static_plot = ax->plot(x, y, "static");

  // create a moving sin(x) function with varying amplitude
  write_y(1.f, 5.f);
  auto moving_plot = ax->plot(x, y, "moving");

  for (int i = 1; i <= nframes; ++i) {
    const float nf = static_cast<float>(nframes);
    const float amplitude = 1.f - 0.5f * std::sin(6.28 * i / nf);
    write_y(amplitude, 5.f);
    moving_plot->add_frame(x, y, 3.f * i / nf);
  }

  // set label axes
  ax->xlabel("x");
  ax->ylabel("y");
  ax->title("the svg test");
  ax->legend();

  // output to svg
  std::ofstream out;
  out.open("test_figure.svg");
  BackendSVG backend(out);
  fig->serialise(backend);
  out.close();
```

## Usage

1. Clone this repository

```bash
$ git clone https://github.com/martinjrobins/trase
$ cd trase
```

2. Build and install Trase. This uses an install dir of `$HOME/trase`, change 
   this to wherever you like. This also builds the OpenGL backend of Trase 
   (requires GLFW installed), switch this to `OFF` if you only want the svg 
   backend.

```bash
$ mkdir build
$ cd build
$ cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$HOME/trase -Dtrase_BUILD_OPENGL=ON ..
$ make install
```

2. (alternate) If you are using the Xcode or Visual Studio generator, you need 
   to specify the configuration in build time

```bash
$ mkdir build
$ cd build
$ cmake -DCMAKE_INSTALL_PREFIX=$HOME/trase -Dtrase_BUILD_OPENGL=ON ..
$ cmake --build . --target install --config Release
```

3. In your C++ project, you might link against Trase with a `CMakeLists.txt` 
   file like so (remove `backendGL` if you don't build it):

```cmake
cmake_minimum_required(VERSION 2.8.12)
project(test)

find_package(trase REQUIRED)

add_executable(myexe test.cpp)
target_link_libraries(myexe trase backendGL backendSVG)
```

4. When you build your project, you can tell CMake where you installed Trase 
   using the `CMAKE_PREFIX_PATH` variable

```bash
$ cmake -DCMAKE_PREFIX_PATH=$HOME/trase ..
$ make
```

## Acknowledgments

Trase uses [Dear ImGui](https://github.com/ocornut/imgui) and 
[NanoVG](https://github.com/memononen/nanovg) for the OpenGL backend. The 
[Dirent port](https://github.com/tronkko/dirent) for Windows is used for finding 
local font files.
