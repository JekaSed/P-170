cmake_minimum_required(VERSION 3.5)

project(artek VERSION 0.2 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(ARTEK_TESTS "Enable tests" ON)
option(GUI_COVERAGE_ENABLED "enable coverage compiler flags for GUI" OFF)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
)

add_subdirectory(artek-gui)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(${PROJECT_NAME}
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    add_executable(audioplayer
        ${PROJECT_SOURCES}
    )
endif()

target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    artek-widget)

file(GLOB_RECURSE Q_RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.qrc)
target_sources(${PROJECT_NAME} PRIVATE ${Q_RESOURCES})

set_target_properties(${PROJECT_NAME} PROPERTIES
    WIN32_EXECUTABLE TRUE
)

if(GUI_COVERAGE_ENABLED)
  message(STATUS "gui enabling coverage")
  include(cmake/CodeCoverage.cmake)
  append_coverage_compiler_flags()
  # cmake-format: off
  setup_target_for_coverage_gcovr_html(
    NAME gui_coverage
    EXECUTABLE ctest -V --output-junit junit-report.xml
    EXCLUDE "bin" "tests" "build" ".deps"
    )
  # cmake-format: on
endif()

if(ARTEK_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(${PROJECT_NAME})
endif()
