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

### Exception.h

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
