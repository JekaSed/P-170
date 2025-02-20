cmake_minimum_required(VERSION 3.5)

project(artek-widget VERSION 0.2 LANGUAGES CXX)

function(EnableWarnings target)
  if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    # target_compile_options(${target} PRIVATE /WX)
  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL
                                                 "Clang")
    target_compile_options(
      ${target}
      PRIVATE -Wall
              -Wextra
              -Werror
              -Wno-conversion
              -Wno-sign-compare
              -pedantic-errors
              -Wno-implicit-fallthrough)
  endif()
endfunction()

add_subdirectory(qt-material-widgets)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
#find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

#file(GLOB_RECURSE INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

file(GLOB_RECURSE SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE H ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h)
file(GLOB_RECURSE Q_RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.qrc)

set(SOURCES artek-widget.h artek-widget.cpp)
#add_library(tdr84-widget STATIC ${SRC} ${Q_RESOURCES})
add_library(${PROJECT_NAME} STATIC ${SRC} ${H} ${Q_RESOURCES} ${SOURCES})

enablewarnings(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets QtMaterialWidgets)
#target_link_libraries(tdr84-widget PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
#target_include_directories(tdr84-widget PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
#target_include_directories(tdr84-widget PUBLIC INCLUDES)
