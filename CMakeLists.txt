cmake_minimum_required(VERSION 3.8)
project(webhama)

set(CMAKE_CXX_STANDARD 14)

# required for Qt
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5Core)
find_package(Qt5Widgets)
find_package(Qt5WebEngine)
find_package(Qt5WebEngineWidgets)

file(GLOB SOURCE_FILES *.cc *.hh)
add_executable(webhama WIN32 ${SOURCE_FILES})

target_link_libraries(webhama Qt5::Widgets Qt5::WebEngine Qt5::WebEngineWidgets)
