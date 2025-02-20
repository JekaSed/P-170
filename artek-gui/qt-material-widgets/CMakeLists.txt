cmake_minimum_required(VERSION 3.5)

project(QtMaterialWidgets LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)

file(GLOB_RECURSE SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE Q_RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.qrc)

add_library(QtMaterialWidgets STATIC ${SRC} ${Q_RESOURCES})

target_link_libraries(QtMaterialWidgets PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_include_directories(QtMaterialWidgets PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS  StateMachine)
    target_link_libraries(QtMaterialWidgets PRIVATE Qt${QT_VERSION_MAJOR}::StateMachine)
endif()
