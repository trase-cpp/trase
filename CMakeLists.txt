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

cmake_minimum_required (VERSION 3.6)
project (trase)

set (CMAKE_DEBUG_POSTFIX d)

# ensure C++14
set (CMAKE_CXX_STANDARD 14)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_EXTENSIONS OFF)

#export compiler flags for code completion engines
set (CMAKE_EXPORT_COMPILE_COMMANDS 1)

set (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

# Set global compiler warnings
if (MSVC)
    set(common_options /W3 /WX)
    set(debug_options ${common_options} /D_ITERATOR_DEBUG_LEVEL=2)
    set(not_debug_options ${common_options} /D_ITERATOR_DEBUG_LEVEL=0 /D_SCL_SECURE_NO_WARNINGS)

    add_compile_options(
        "$<$<CONFIG:Debug>:${debug_options}>$<$<NOT:$<CONFIG:Debug>>:${not_debug_options}>"
        )
else ()
    add_compile_options (-Wall -pedantic)
endif ()


option (trase_BUILD_OPENGL "Build OpenGL backend and interactive test" ON)

if (trase_BUILD_OPENGL)
    set (imgui_dir imgui)
    set (imgui_source
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

    set (glext_dir glad)
    set (glext_source
        ${glext_dir}/glad.c
        ${glext_dir}/glad.h
        )

    set (nanovg_dir nanovg)
    set (nanovg_source
        ${nanovg_dir}/fontstash.h
        ${nanovg_dir}/nanovg.c
        ${nanovg_dir}/nanovg_gl.h
        ${nanovg_dir}/nanovg_gl_utils.h
        ${nanovg_dir}/nanovg.h
        ${nanovg_dir}/stb_image.h
        ${nanovg_dir}/stb_truetype.h
        )

    set (trase_gl_ext_headers
        ${glext_dir}/glad.h
        ${imgui_dir}/imgui.h
        ${imgui_dir}/imconfig.h
        ${nanovg_dir}/nanovg.h
        )

    find_package (glfw3 3.2 REQUIRED)
    find_package (OpenGL REQUIRED)

    add_library (glext ${glext_source})
    target_include_directories (glext SYSTEM PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${glext_dir}>
        $<INSTALL_INTERFACE:include>)
    target_link_libraries (glext PUBLIC ${OPENGL_gl_LIBRARY})

    add_library (imgui ${imgui_source})
    target_include_directories (imgui SYSTEM PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${imgui_dir}>
        $<INSTALL_INTERFACE:include>)
    target_link_libraries (imgui PUBLIC glext glfw dl)

    add_library (nanovg ${nanovg_source})
    target_include_directories (nanovg SYSTEM PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${nanovg_dir}>
        $<INSTALL_INTERFACE:include>)
    target_link_libraries (nanovg PUBLIC ${OPENGL_gl_LIBRARY} glfw dl)
endif (trase_BUILD_OPENGL)

if (WIN32)
    set (dirent_headers dirent/dirent.h)
    add_library (dirent INTERFACE)
    #target_sources(dirent INTERFACE dirent/dirent.h)
    target_include_directories (dirent INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/dirent>
        $<INSTALL_INTERFACE:include>)
endif ()

set (trase_headers
    src/trase.hpp
    src/backend/Backend.hpp
    src/frontend/Axis.hpp
    src/frontend/Drawable.hpp
    src/frontend/Figure.hpp
    src/frontend/Plot1D.hpp
    src/frontend/Transform.hpp
    src/frontend/Data.hpp
    src/util/BBox.hpp
    src/util/Colors.hpp
    src/util/Exception.hpp
    src/util/Vector.hpp
    )

set (trase_source
    src/backend/Backend.cpp
    src/frontend/Axis.cpp
    src/frontend/Drawable.cpp
    src/frontend/Figure.cpp
    src/frontend/Plot1D.cpp
    src/frontend/Data.cpp
    )

set (trase_draw_headers
    src/frontend/AxisDraw.hpp
    src/frontend/DrawableDraw.hpp
    src/frontend/FigureDraw.hpp
    src/frontend/Plot1DDraw.hpp
    )


add_library (trase
    ${trase_source}
    ${trase_headers}
    )

target_include_directories (trase PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>)

if (WIN32)
    target_link_libraries (trase PUBLIC dirent)
endif ()

set (trase_fonts
    font/entypo.ttf
    font/NotoEmoji-Regular.ttf
    font/Roboto-Bold.ttf
    font/Roboto-Light.ttf
    font/Roboto-Regular.ttf
    )

target_compile_definitions (trase PRIVATE TRASE_SOURCE_DIR="${trase_SOURCE_DIR}" TRASE_INSTALL_DIR="${CMAKE_INSTALL_PREFIX}")


if (trase_BUILD_OPENGL)
    set (trase_gl_headers
        src/backend/BackendGL.hpp
        )
    add_library (backendGL
        ${trase_draw_headers}
        ${trase_gl_headers}
        src/backend/BackendGL.cpp
        )

    target_include_directories (backendGL PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<INSTALL_INTERFACE:include>)
    target_link_libraries (backendGL PUBLIC trase glext nanovg imgui)
endif ()

set (trase_svg_headers
    src/backend/BackendSVG.hpp
    )
add_library (backendSVG
    ${trase_draw_headers}
    ${trase_svg_headers}
    src/backend/BackendSVG.cpp
    )
target_include_directories (backendSVG PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src> $<INSTALL_INTERFACE:include>)
target_link_libraries (backendSVG PUBLIC trase)

enable_testing ()

if (trase_BUILD_OPENGL)
    add_executable (interactive_tst tests/TestInteractive.cpp)
    target_include_directories (interactive_tst PRIVATE tests)
    target_link_libraries (interactive_tst PRIVATE trase backendGL)
endif ()

add_executable (
    trase_tst
    tests/TestFigure.cpp
    tests/TestAxis.cpp
    tests/TestPlot1D.cpp
    tests/TestBackendSVG.cpp
    tests/TestBBox.cpp
    tests/TestVector.cpp
    tests/TestTransformMatrix.cpp
    tests/TestFontManager.cpp
    )
target_include_directories (trase_tst PRIVATE tests)
target_link_libraries (trase_tst PRIVATE trase backendSVG)
add_test (the_trase_tst trase_tst)


# Clang tidy as optional static analyzer
option (trase_USE_CLANG_TIDY "Use clang tidy for static analysis" OFF)
if (trase_USE_CLANG_TIDY)
    find_program (CLANG_TIDY_EXE NAMES "clang-tidy" "clang-tidy-6.0" "clang-tidy-5.0" "clang-tidy-4.0"
        DOC "Path to clang-tidy executable")
    if (NOT CLANG_TIDY_EXE)
        message (STATUS "clang-tidy not found.")
    else ()
        message (STATUS "clang-tidy found: ${CLANG_TIDY_EXE}")
        set_target_properties (trase PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_EXE}")
    endif ()
endif ()

# LLVM AddressSanitizer to detecting memory errors
# Note that there are many other sanitizers in LLVM to help detect errors, see
# http://travistoptips.blogspot.co.uk/2015/11/sanitize-all-things.html
option (trase_MEMCHECK "Use LLVM AddressSanitizer to detecting memory errors" OFF)
if (trase_MEMCHECK)
    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        message (STATUS "Configuring with LLVM AddressSanitizer")
        set (trase_MEMCHECK_FLAGS -fno-optimize-sibling-calls
            -fsanitize=address
            -fsanitize-address-use-after-scope
            )
        target_compile_options (trase PUBLIC -O1 -g -fno-omit-frame-pointer ${trase_MEMCHECK_FLAGS})
        target_link_libraries (trase PUBLIC -g ${trase_MEMCHECK_FLAGS})
    else ()
        message (FATAL_ERROR "clang compiler required with trase_MEMCHECK: found ${CMAKE_CXX_COMPILER_ID}")
    endif ()
endif ()

# Setup coverage testing for GCC or Clang
option (trase_ENABLE_COVERAGE "Enable coverage reporting for GCC or Clang" FALSE)
if (trase_ENABLE_COVERAGE)
    if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
        message (STATUS "Configuring with coverage")
        target_compile_options (trase PUBLIC --coverage -O0)
        target_link_libraries (trase PUBLIC --coverage)
    else ()
        message (FATAL_ERROR "GCC or Clang required with trase_ENABLE_COVERAGE: found ${CMAKE_CXX_COMPILER_ID}")
    endif ()
endif ()

# install stuff
set (install-targets trase backendSVG)
if (WIN32)
    list (APPEND install-targets dirent)
endif ()
if (trase_BUILD_OPENGL)
    list (APPEND install-targets backendGL glext nanovg imgui)
endif ()

set (install-headers ${trase_headers} ${trase_svg_headers})
if (trase_BUILD_OPENGL)
    list (APPEND install-headers ${trase_gl_headers} ${trase_gl_ext_headers})
endif ()


install (TARGETS ${install-targets}
    EXPORT trase-targets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    )

install (EXPORT trase-targets
    FILE trase-config.cmake
    DESTINATION lib/cmake/trase
    )

install (FILES ${install-headers} DESTINATION include)
install (FILES ${trase_fonts} DESTINATION font)
