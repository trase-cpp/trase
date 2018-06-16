# trase

[![codecov](https://codecov.io/gh/martinjrobins/trase/branch/master/graph/badge.svg)](https://codecov.io/gh/martinjrobins/trase)
[![Build
Status](https://travis-ci.org/martinjrobins/trase.svg?branch=master)](https://travis-ci.org/martinjrobins/trase)
[![Build 
status](https://ci.appveyor.com/api/projects/status/kfm43tg6qltyjsyl/branch/master?svg=true)](https://ci.appveyor.com/project/martinjrobins/trase/branch/master)

Trase is a lightweight scientific plotting library for C++ with animation 
support. It enables you to construct plots, and display them via the OpenGL 
interface, or write them out to animated svg files. The svg backend has no dependencies other than the standard library. The OpenGL backend requires [GLFW](http://www.glfw.org/), and uses [Dear ImGui](https://github.com/ocornut/imgui) and [NanoVG](https://github.com/memononen/nanovg).


<p align="center">
  <img width="600" 
  src="https://cdn.rawgit.com/martinjrobins/trase/svg/test_figure.svg">
</p>


For example, the above svg image was generated with the following code.

```cpp
  auto fig = figure();
  auto ax = fig->axis();
  const int n = 100;
  std::vector<float> x(n);
  std::vector<float> y(n);
  for (int i = 0; i < n; ++i) {
    x[i] = static_cast<float>(i) * 6.28 / n;
    y[i] = std::sin(x[i]);
  }
  auto static_plot = ax->plot(x, y);
  auto moving_plot = ax->plot(x, y);
  float time = 0.0;

  auto do_plot = [&](const float theta) {
    for (int i = 0; i < n; ++i) {
      y[i] = std::sin(theta * x[i]);
    }
    time += 0.3;
    moving_plot->add_frame(x, y, time);
  };

  for (int i = 1; i < 6; ++i) {
    do_plot(i);
  }
  for (int i = 5; i >= 1; --i) {
    do_plot(i);
  }

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
$ cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=$HOME/trase ..
$ make
```
