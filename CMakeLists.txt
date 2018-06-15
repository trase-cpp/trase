# Copyright (c) 2018, University of Oxford.
# All rights reserved.
#
# University of Oxford means the Chancellor, Masters and Scholars of the
# University of Oxford, having an administrative office at Wellington
# Square, Oxford OX1 2JD, UK.
#
# This file is part of the Oxford RSE C++ Template project.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
# * Redistributions of source code must retain the above copyright notice, this
# list of conditions and the following disclaimer.
# * Redistributions in binary form must reproduce the above copyright notice,
# this list of conditions and the following disclaimer in the documentation
# and/or other materials provided with the distribution.
# * Neither the name of the copyright holder nor the names of its
# contributors may be used to endorse or promote products derived from
# this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

cmake_minimum_required(VERSION 3.6)
project(trase)

# ensure C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#export compiler flags for code completion engines
set( CMAKE_EXPORT_COMPILE_COMMANDS 1 )

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

# Set global compiler warnings
if(MSVC)
    add_compile_options(/W3 /WX)
else()
    add_compile_options(-Wall -pedantic)
endif()


option(trase_BUILD_OPENGL "Build OpenGL backend and interactive test" ON)

if (trase_BUILD_OPENGL)
    set(imgui_dir ${trase_SOURCE_DIR}/imgui)
    set(imgui_source
        ${imgui_dir}/imgui.cpp
        ${imgui_dir}/imgui.h
        ${imgui_dir}/imgui_draw.cpp
        ${imgui_dir}/imgui_demo.cpp
        ${imgui_dir}/imgui_internal.h
        ${imgui_dir}/imconfig.h
        ${imgui_dir}/stb_rect_pack.h
        ${imgui_dir}/stb_textedit.h
        ${imgui_dir}/stb_truetype.h
        ${imgui_dir}/imgui_impl_glfw_gl3.h
        ${imgui_dir}/imgui_impl_glfw_gl3.cpp
        )

    set(glext_dir ${trase_SOURCE_DIR}/glad)
    set(glext_source
        ${glext_dir}/glad.c
        ${glext_dir}/glad/glad.h
        )

    set(nanovg_dir ${trase_SOURCE_DIR}/nanovg)
    set(nanovg_source
        ${nanovg_dir}/fontstash.h
        ${nanovg_dir}/nanovg.c
        ${nanovg_dir}/nanovg_gl.h
        ${nanovg_dir}/nanovg_gl_utils.h
        ${nanovg_dir}/nanovg.h
        ${nanovg_dir}/stb_image.h
        ${nanovg_dir}/stb_truetype.h
        )


    find_package(glfw3 3.2 REQUIRED)
    find_package(OpenGL REQUIRED)

    add_library(glext ${glext_source})
    target_include_directories(glext SYSTEM PUBLIC ${glext_dir})
    target_link_libraries(glext PUBLIC ${OPENGL_gl_LIBRARY})

    add_library(imgui ${imgui_source})
    target_include_directories(imgui SYSTEM PUBLIC ${imgui_dir})
    target_link_libraries(imgui PUBLIC  glext glfw dl)

    add_library(nanovg ${nanovg_source})
    target_include_directories(nanovg SYSTEM PUBLIC ${nanovg_dir})
    target_link_libraries(nanovg PUBLIC ${OPENGL_gl_LIBRARY} glfw dl)
endif(trase_BUILD_OPENGL)

add_library(trase
    src/Drawable.cpp
    src/Drawable.hpp
    src/Figure.cpp
    src/Figure.hpp
    src/Axis.cpp
    src/Axis.hpp
    src/Plot1D.cpp
    src/Plot1D.hpp
    src/Exception.hpp)
target_include_directories(trase PUBLIC src)


if (trase_BUILD_OPENGL)
    add_library(backendGL
        src/DrawableDraw.hpp
        src/FigureDraw.hpp
        src/AxisDraw.hpp
        src/Plot1DDraw.hpp
        src/BackendGL.cpp
        src/BackendGL.hpp
        )

    target_include_directories(backendGL PUBLIC src)
    target_link_libraries(backendGL PUBLIC trase glext nanovg imgui)
endif()

add_library(backendSVG
    src/DrawableDraw.hpp
    src/FigureDraw.hpp
    src/AxisDraw.hpp
    src/Plot1DDraw.hpp
    src/BackendSVG.cpp
    src/BackendSVG.hpp
    )
target_include_directories(backendSVG PUBLIC src)
target_link_libraries(backendSVG PUBLIC trase)

enable_testing()

if (trase_BUILD_OPENGL)
    add_executable(interactive_test tests/TestInteractive.cpp)
    target_include_directories(interactive_test PRIVATE tests)
    target_link_libraries(interactive_test PRIVATE trase backendGL)
endif()

add_executable(trase_tests
    tests/TestFigure.cpp
    tests/TestPlot1D.cpp
    tests/TestBackendSVG.cpp
    )
target_include_directories(trase_tests PRIVATE tests)
target_link_libraries(trase_tests PRIVATE trase backendSVG)
add_test(tests trase_tests)


# Clang tidy as optional static analyzer
option(trase_USE_CLANG_TIDY "Use clang tidy for static analysis" OFF)
if (trase_USE_CLANG_TIDY)
    find_program(CLANG_TIDY_EXE NAMES "clang-tidy" "clang-tidy-6.0" "clang-tidy-5.0" "clang-tidy-4.0"
        DOC "Path to clang-tidy executable")
    if(NOT CLANG_TIDY_EXE)
        message(STATUS "clang-tidy not found.")
    else()
        message(STATUS "clang-tidy found: ${CLANG_TIDY_EXE}")
        set_target_properties(trase PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_EXE}")
    endif()
endif()

# LLVM AddressSanitizer to detecting memory errors
# Note that there are many other sanitizers in LLVM to help detect errors, see
# http://travistoptips.blogspot.co.uk/2015/11/sanitize-all-things.html
option(trase_MEMCHECK "Use LLVM AddressSanitizer to detecting memory errors" OFF)
if (trase_MEMCHECK)
    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        message(STATUS "Configuring with LLVM AddressSanitizer")
        set(trase_MEMCHECK_FLAGS -fno-optimize-sibling-calls
            -fsanitize=address
            -fsanitize-address-use-after-scope
            )
        target_compile_options(trase PUBLIC -O1 -g -fno-omit-frame-pointer ${trase_MEMCHECK_FLAGS})
        target_link_libraries(trase PUBLIC -g ${trase_MEMCHECK_FLAGS})
    else()
        message(FATAL_ERROR "clang compiler required with trase_MEMCHECK: found ${CMAKE_CXX_COMPILER_ID}")
    endif()
endif()

# Setup coverage testing for GCC or Clang
option(trase_ENABLE_COVERAGE "Enable coverage reporting for GCC or Clang" FALSE)
if (trase_ENABLE_COVERAGE)
    if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
        message(STATUS "Configuring with coverage")
        target_compile_options(trase PUBLIC --coverage -O0)
        target_link_libraries(trase PUBLIC --coverage)
    else()
        message(FATAL_ERROR "GCC or Clang required with trase_ENABLE_COVERAGE: found ${CMAKE_CXX_COMPILER_ID}")
    endif()
endif()
