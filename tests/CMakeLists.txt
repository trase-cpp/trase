
if (trase_BUILD_OPENGL)
    add_executable (interactive_test TestInteractive.cpp)
    target_link_libraries (interactive_test PRIVATE trase)
endif ()

add_executable (
    trase_test
    DummyDraw.hpp
    DummyDraw.cpp
    TestAxis.cpp
    TestData.cpp
    TestBackendSVG.cpp
    TestBBox.cpp
    TestColors.cpp
    TestFigure.cpp
    TestFontManager.cpp
    TestGeometry.cpp
    TestHistogram.cpp
    TestPoints.cpp
    TestRectangle.cpp
    TestUserConcepts.cpp
    TestStyle.cpp
    TestTransformMatrix.cpp
    TestVector.cpp
    TestLegend.cpp
)
if (CURL_FOUND)
    target_sources(trase_test PRIVATE TestCSVDownloader.cpp)
endif (CURL_FOUND)

target_include_directories (trase_test PRIVATE tests)
target_link_libraries (trase_test PRIVATE trase)
add_test (the_trase_test trase_test)


