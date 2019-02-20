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

if(CMAKE_CXX_COMPILER MATCHES "/em\\+\\+(-[a-zA-Z0-9.])?$")
    message(" * C++ compiler: Emscripten")
    set(trase_Emscripten ON)
else()
    message(" * C++ cimpiler: ${CMAKE_CXX_COMPILER_ID}")
endif()

# Set global compiler warnings
if (MSVC)
    set(common_options /W3 /WX)
    set(debug_options ${common_options} /D_ITERATOR_DEBUG_LEVEL=0 /D_SCL_SECURE_NO_WARNINGS)
    set(not_debug_options ${common_options} /D_ITERATOR_DEBUG_LEVEL=0 /D_SCL_SECURE_NO_WARNINGS)

    add_compile_options(
        "$<$<CONFIG:Debug>:${debug_options}>$<$<NOT:$<CONFIG:Debug>>:${not_debug_options}>"
        )
else ()
    add_compile_options (-Wall -pedantic)
endif ()


option (trase_BUILD_OPENGL "Build OpenGL backend and interactive test" ON)

if (trase_BUILD_OPENGL)

    set (nanovg_dir third-party/nanovg)
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
        ${nanovg_dir}/nanovg.h
        )

    if (NOT trase_Emscripten)
        find_package (glfw3 3.2 REQUIRED)
        find_package (OpenGL REQUIRED)
    else()
        set(CMAKE_EXECUTABLE_SUFFIX ".html")
        set(OPENGL_gl_LIBRARY "")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s DISABLE_EXCEPTION_CATCHING=0 -s USE_WEBGL2=1 -s FULL_ES2=1  -s USE_GLFW=3 -s WASM=1 --preload-file ${trase_SOURCE_DIR}/font")
    endif()


    add_library (nanovg ${nanovg_source})
    target_include_directories (nanovg SYSTEM PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${nanovg_dir}>
        $<INSTALL_INTERFACE:include>)
    target_link_libraries (nanovg PUBLIC ${OPENGL_gl_LIBRARY} glfw dl)
endif (trase_BUILD_OPENGL)

if (WIN32)
    set (dirent_dir third-party/dirent)
    set (dirent_headers ${dirent_dir}/dirent.h)
    add_library (dirent INTERFACE)
    #target_sources(dirent INTERFACE dirent/dirent.h)
    target_include_directories (dirent INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${dirent_dir}>
        $<INSTALL_INTERFACE:include>)
endif ()




set (trase_headers
    src/trase.hpp
    src/backend/Backend.hpp
    src/backend/BackendSVG.hpp
    src/frontend/Axis.hpp
    src/frontend/Data.hpp
    src/frontend/Drawable.hpp
    src/frontend/Figure.hpp
    src/frontend/Geometry.hpp
    src/frontend/Transform.hpp
    src/frontend/Line.hpp
    src/frontend/Points.hpp
    src/frontend/Rectangle.hpp
    src/frontend/Histogram.hpp
    src/frontend/Legend.hpp
    src/util/ColumnIterator.hpp
    src/util/BBox.hpp
    src/util/Colors.hpp
    src/util/Exception.hpp
    src/util/Style.hpp
    src/util/Vector.hpp
    src/util/CSVDownloader.hpp
    )


set (trase_source
    src/backend/Backend.cpp
    src/backend/BackendSVG.cpp
    src/frontend/Axis.cpp
    src/frontend/Data.cpp
    src/frontend/Drawable.cpp
    src/frontend/Figure.cpp
    src/frontend/Geometry.cpp
    src/frontend/Legend.cpp
    src/frontend/Transform.cpp
    src/util/Colors.cpp
    src/util/Style.cpp
    src/util/CSVDownloader.cpp
    )

if (trase_BUILD_OPENGL)
    list(APPEND trase_headers src/backend/BackendGL.hpp)
    list(APPEND trase_source src/backend/BackendGL.cpp)
endif()


add_library (trase
    ${trase_source}
    ${trase_headers}
    )

target_include_directories (trase PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>)

if (WIN32)
    target_link_libraries (trase PUBLIC dirent)
endif ()


target_compile_definitions (trase PRIVATE TRASE_SOURCE_DIR="${trase_SOURCE_DIR}" TRASE_INSTALL_DIR="${CMAKE_INSTALL_PREFIX}")


if (trase_BUILD_OPENGL)
    target_compile_definitions (trase PUBLIC TRASE_BACKEND_GL)
    target_link_libraries (trase PUBLIC nanovg)
endif ()


find_package(CURL)
if (CURL_FOUND)
  target_compile_definitions (trase PUBLIC TRASE_HAVE_CURL)
  target_link_libraries (trase PUBLIC ${CURL_LIBRARIES})
  message(${CURL_INCLUDE_DIRS})
  target_include_directories(trase PUBLIC ${CURL_INCLUDE_DIRS})
endif (CURL_FOUND)


set (trase_fonts
    font/entypo.ttf
    font/NotoEmoji-Regular.ttf
    font/Roboto-Bold.ttf
    font/Roboto-Light.ttf
    font/Roboto-Regular.ttf
    )


enable_testing ()
add_subdirectory(tests)
add_subdirectory(examples)


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
set (install-targets trase)
if (WIN32)
    list (APPEND install-targets dirent)
endif ()
if (trase_BUILD_OPENGL)
    list (APPEND install-targets nanovg)
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
