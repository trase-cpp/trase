# list of examples
set (examples
  Line.cpp
  AnimatedLine.cpp
  Histogram.cpp
  AnimatedPoints.cpp
  LineStyle.cpp
  Rectangle.cpp
  )


# list of examples that need curl
find_package(CURL)
if (CURL_FOUND)
  list(APPEND examples
    Points.cpp
    FacetPoints.cpp
    )
endif(CURL_FOUND)

foreach (file IN LISTS examples)
  get_filename_component (name ${file} NAME_WE)
  string(TOLOWER ${name} name)

  add_executable (${name} ${file})
  target_link_libraries (${name} PRIVATE trase)
  target_compile_definitions (${name} PRIVATE TRASE_EXAMPLES_SVG_BACKEND)
  if (CURL_FOUND)
    target_link_libraries (${name} PRIVATE ${CURL_LIBRARIES})
    target_include_directories(${name} PRIVATE ${CURL_INCLUDE_DIRS})
  endif (CURL_FOUND)
  add_test (example_${name} ${name})

  if (trase_BUILD_OPENGL)
    add_executable (${name}_gl ${file})
    target_link_libraries (${name}_gl PRIVATE trase)
    target_compile_definitions (${name}_gl PRIVATE TRASE_EXAMPLES_GL_BACKEND)
  endif ()
endforeach(file)

