# trase

[![Ubuntu unit](https://github.com/trase-cpp/trase/workflows/Ubuntu%20unit/badge.svg)](https://github.com/trase-cpp/trase/actions)
[![macOS unit](https://github.com/trase-cpp/trase/workflows/macOS%20unit/badge.svg)](https://github.com/trase-cpp/trase/actions)
[![Windows unit](https://github.com/trase-cpp/trase/workflows/Windows%20unit/badge.svg)](https://github.com/trase-cpp/trase/actions)

[![codecov](https://codecov.io/gh/trase-cpp/trase/branch/master/graph/badge.svg)](https://codecov.io/gh/trase-cpp/trase)
[![BCH compliance](https://bettercodehub.com/edge/badge/trase-cpp/trase?branch=master)](https://bettercodehub.com/)
[![codedocs.xyz](https://codedocs.xyz/trase-cpp/trase.svg)](https://codedocs.xyz/trase-cpp/trase/)

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
  auto get_ysinx = [&](const float amplitude, const float freq) {
    for (int i = 0; i < n; ++i) {
      y[i] = amplitude * std::sin(6.28f * freq * x[i]);
    }
    return create_data().x(x).y(y);
  };

  // create a static sin(x) function
  auto static_plot = ax->line(get_ysinx(1.f, 2.f));
  static_plot->set_label("static");

  // create a moving sin(x) function with varying amplitude
  auto moving_plot = ax->line(get_ysinx(1.f, 5.f));
  moving_plot->set_label("moving");

  for (int i = 1; i <= nframes; ++i) {
    const float nf = static_cast<float>(nframes);
    const float amplitude = 1.f - 0.5f * std::sin(6.28f * i / nf);
    moving_plot->add_frame(get_ysinx(amplitude, 5.f), 3.f * i / nf);
  }

  // set label axes
  ax->xlabel("x");
  ax->ylabel("y");
  ax->title("the svg test");
  ax->legend();

  // output to svg
  std::ofstream out;
  out.open("readme.svg");
  BackendSVG backend(out);
  fig->draw(backend);
  out.close();
```

## Contributing to Trase

We welcome community pull requests for Trase, please see the guidelines
[here](CONTRIBUTING.md). Potential areas for changes are:
- Bug-fixes. Please add a relevant test in `tests/` demonstrating the fixed bug.
- New geometry, aesthetic or transform. The design of Trase roughly follows the
  Grammer of Graphics, so looking at existing GOG packages such as
  [ggplot](https://ggplot2.tidyverse.org/reference) could give you some good
  ideas
- New example showing how to use Trase. All the examples shown in the documentation
  are in `examples/`.
- New drawing backend. There are two main types of backend. Classes derived from
  `Backend` are "pure" drawing classes in the same vein as
  [NanoVG](https://github.com/memononen/nanovg), while those derived from
  `AnimatedBackend` can also draw shapes or paths that have properties which
  animate over time.
- Or consult the [issues](https://github.com/trase-cpp/trase/issues) for more
  ideas

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
   file like so:

```cmake
cmake_minimum_required(VERSION 2.8.12)
project(test)

find_package(trase REQUIRED)

add_executable(myexe test.cpp)
target_link_libraries(myexe trase)
```

4. When you build your project, you can tell CMake where you installed Trase 
   using the `CMAKE_PREFIX_PATH` variable

```bash
$ cmake -DCMAKE_PREFIX_PATH=$HOME/trase ..
$ make
```

## Acknowledgments

Trase uses [NanoVG](https://github.com/memononen/nanovg) for the OpenGL
backend. The [Dirent port](https://github.com/tronkko/dirent) for Windows is
used for finding local font files.
