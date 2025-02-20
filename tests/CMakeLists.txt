project(artek-test LANGUAGES CXX)

find_package(Qt6 COMPONENTS Test Widgets)
if(NOT Qt6_FOUND)
  find_package(Qt5 REQUIRED COMPONENTS Test Widgets)
endif()
find_package(
  Qt${QT_VERSION_MAJOR}
  COMPONENTS Test
  REQUIRED)
if(${QT_VERSION_MAJOR} LESS 6)
  find_package(Qt5Test REQUIRED)
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

file(GLOB_RECURSE SRC ${CMAKE_CURRENT_LIST_DIR}/main.cpp)
file(GLOB_RECURSE Q_RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.qrc)

add_executable(${PROJECT_NAME} ${SRC} ${Q_RESOURCES})
add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PRIVATE artek-widget
                                             Qt${QT_VERSION_MAJOR}::Test
                                             Qt${QT_VERSION_MAJOR}::Widgets)
