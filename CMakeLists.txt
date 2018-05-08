cmake_minimum_required(VERSION 3.6)
project(Template)

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

add_library(mylib src/MyLibrary.cpp)
target_include_directories(mylib PUBLIC src)

enable_testing()

add_executable(mytest tests/MyTests.cpp)
target_include_directories(mylib PRIVATE tests)
target_link_libraries(mytest PRIVATE mylib)
add_test(test1 mytest)

# boost as a required dependency for mylib
find_package(Boost REQUIRED)
target_link_libraries(mylib PRIVATE "${Boost_LIBRARIES}")
target_include_directories(mylib PRIVATE "${Boost_INCLUDE_DIR}")

# OpenMP as an optional component for tests
option(Template_USE_OPENMP "Use OpenMP for shared memory parallism" OFF)
if (Template_USE_OPENMP)
    find_package(OpenMP REQUIRED)
    target_compile_options(mytest PRIVATE "${OpenMP_CXX_FLAGS}")
    target_link_libraries(mytest PRIVATE "${OpenMP_EXE_LINKER_FLAGS}")
endif()

# Clang tidy as optional static analyzer
option(Template_USE_CLANG_TIDY "Use clang tidy for static analysis" OFF)
if (Template_USE_CLANG_TIDY)
    find_program(CLANG_TIDY_EXE NAMES "clang-tidy" "clang-tidy-6.0" "clang-tidy-5.0" "clang-tidy-4.0"
                 DOC "Path to clang-tidy executable")
    if(NOT CLANG_TIDY_EXE)
      message(STATUS "clang-tidy not found.")
    else()
      message(STATUS "clang-tidy found: ${CLANG_TIDY_EXE}")
      set_target_properties(mylib PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_EXE}")
    endif()
endif()

# setup coverage for gcc or clang
if (CMAKE_COMPILER_IS_GNUCC)
  option(ENABLE_COVERAGE "Enable coverage reporting for gcc/clang" FALSE)
  if (ENABLE_COVERAGE)
    target_compile_options(mylib PUBLIC --coverage -O0)
    target_link_libraries(mylib PUBLIC --coverage)
  endif()
endif()
