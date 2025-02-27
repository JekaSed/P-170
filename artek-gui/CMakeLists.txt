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
add_subdirectory(llhttp)
add_subdirectory(rtp)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Widgets Network Multimedia)

set(qt_libs Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Widgets
  Qt${QT_VERSION_MAJOR}::Network
  Qt${QT_VERSION_MAJOR}::Multimedia
  )

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
  find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core5Compat
    StateMachine)
  list(APPEND qt_libs Qt${QT_VERSION_MAJOR}::Core5Compat
    Qt${QT_VERSION_MAJOR}::StateMachine)
endif()

file(GLOB_RECURSE SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE H ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h)
file(GLOB_RECURSE Q_RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.qrc)

set(SOURCES artek-widget.h artek-widget.cpp artek-editor.h artek-editor.cpp)
add_library(${PROJECT_NAME} STATIC ${SRC} ${H} ${Q_RESOURCES} ${SOURCES})

enablewarnings(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PRIVATE ${qt_libs} QtMaterialWidgets llhttp::llhttp rtp)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
