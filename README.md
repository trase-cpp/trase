# Template C++ Project

[![codecov](https://codecov.io/gh/OxfordRSE/template-project-cpp/branch/master/graph/badge.svg)](https://codecov.io/gh/OxfordRSE/template-project-cpp)
[![Build
Status](https://travis-ci.org/OxfordRSE/template-project-cpp.svg?branch=master)](https://travis-ci.org/OxfordRSE/template-project-cpp)
[![Build status](https://ci.appveyor.com/api/projects/status/5245a76r7ikhaass/branch/master?svg=true)](https://ci.appveyor.com/project/fcooper8472/template-project-cpp/branch/master)

##### WARNING: still under construction

## Overview

This repository is a template to help get you set up quickly when starting a new C++ project.
We have tried to conform to common software engineering 'best practices', and to that end this repository features the following:

- C++14
- [CMake](https://cmake.org/) build script for cross-platform configuration
- [Catch](https://github.com/catchorg/Catch2) unit testing framework
- [Travis](https://travis-ci.org/) integration for automated testing on Linux and macOS
- [AppVeyor](https://www.appveyor.com/) integration for automated testing on Windows
- [Codecov](https://codecov.io/) integration for automated coverage testing
- [ClangFormat](https://clang.llvm.org/docs/ClangFormat.html) for automated source code formatting
- [Clang-Tidy](http://clang.llvm.org/extra/clang-tidy/) for static analysis to catch coding errors
- [AddressSanitizer](https://clang.llvm.org/docs/AddressSanitizer.html) for catching memory-related issues


In addition, the project is set up to use the [boost libraries](https://www.boost.org/), and we will hopefully add other common libraries to the configuration soon.


## Getting started

We strongly recommend that you use `git` to keep your copy of this project under version control.

Feel free to:
- [Fork this repository](https://github.com/OxfordRSE/template-project-cpp#fork-destination-box), or
- [Download a zip](https://github.com/OxfordRSE/template-project-cpp/archive/master.zip), and then initialise it as a git repository

## Where does it run?

We expect this project to be usable on all modern systems
It is regularly tested on the following operating systems:
- Linux: Ubuntu 14.04
- masOS: High Sierra
- Windows: Windows Server 2012

and with the following C++ compilers/toolchains:
- GCC 5.x, 6.x, 7.x
- Clang 4.x, 5.x
- Xcode 9.3
- Visual Studio 2015

## First run

### Linux

- Install [CMake](https://cmake.org/download/) and a C++ compiler. E.g. for 
  Ubuntu you can use the command-line and type:

```bash
$ sudo apt install build-essential cmake
```

- Make a build directory and configure the project. E.g. Assuming you are in a 
  directory containing the template source code:

```bash
$ mkdir build
$ cd build
$ cmake ..
```

- Compile the project

```bash
$ make
```

- Run all the tests using CTest (this is bundled with CMake)

```bash
$ ctest --output-on-failure
```

### macOS

### Windows

- Install [Visual Studio Community](https://www.visualstudio.com/downloads/)
- Install [CMake](https://cmake.org/download/)
- Install and configure [vcpkg](https://github.com/Microsoft/vcpkg)
- Install boost using vcpkg
- Open this repository in Visual Studio
- Build

## Documentation

### MyLibrary.{hpp,cpp}

This template C++ project implements a single function, `get_nth_prime`, within 
the namespace `cpp_template`. This function returns the $i^{th}$ prime number 
$p_i$, where both $i$ and $p_i$ are of type `int`. 

- *Note1*: It is good practice in C++ to put all your code within a namespace, 
  so that definitions in separate projects do not conflict with each other.

- *Note2*: `get_nth_prime` simply wraps the Boost [prime number 
  implementation](https://www.boost.org/doc/libs/1_64_0/libs/math/doc/html/math_toolkit/number_series/primes.html)(https://www.boost.org/doc/libs/1_64_0/libs/math/doc/html/math_toolkit/number_series/primes.html)

- *Note3*: To start using this template in your own projects, simply replace 
  these files with your own, and make sure to update the filenames in 
  `CMakeLists.txt`.

### Exception.h

The $i$ argument to `get_nth_prime` must be greater than zero, and less than 
`boost::math::max_prime`. If either of these conditions are not satisfied the 
function throws an exception, which is defined in this file.

- *Note1*: Throwing an 
  [exception](http://www.cplusplus.com/doc/tutorial/exceptions/) when an error 
  is detected allows programs to react to run-time errors. This is in contrast 
  [asserts](http://www.cplusplus.com/reference/cassert/assert/), which 
  immediately halt program execution.

### MyTests.cpp

### .travis.yml

### appveyor.yml

### codecov

### .clang-format

### .clang-tidy

### AddressSanitizer

There is an optional component enabled via CMake that can use the [LLVM AddressSanitizer](https://clang.llvm.org/docs/AddressSanitizer.html) to detect memory errors.
This is turned on by default for the clang builds on Travis, so you will see any errors on there if it's configured.

You can also run it yourself, provided you are using the clang compiler, by using the `Template_MEMCHECK` option when running CMake.
Simply enable the option, then configure, build, and test:

```bash
cmake -DTemplate_MEMCHECK=TRUE /path/to/project
make
ctest
```
The test will fail at the first error.
It is unlikely that you will encounter a false positive when using the address sanitizer, so if you do see an error, best not to ignore it!

## Feedback and suggestions

If you have any feedback or suggestions about this project, please get in touch or open an issue.
