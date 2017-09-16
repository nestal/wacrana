cmake_minimum_required(VERSION 3.8)
project(wacrana VERSION 0.1)

set(CMAKE_CXX_STANDARD 14)

# required for Qt
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5WebEngine REQUIRED)
find_package(Qt5WebEngineWidgets REQUIRED)
find_package(Threads REQUIRED)
find_package(Boost REQUIRED)

find_package(Doxygen)

include_directories(${Qt5Core_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/include)

file(GLOB SOURCE_FILES src/*.cc src/*.hh res/*.qrc include/*.hpp)
add_executable(wacrana WIN32 ${SOURCE_FILES})

target_link_libraries(
	wacrana
	Threads::Threads
	Qt5::Widgets
	Qt5::WebEngine
	Qt5::WebEngineWidgets
)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/wacrana.json.in ${CMAKE_CURRENT_BINARY_DIR}/wacrana.json @ONLY)

# documentations
if (DOXYGEN_FOUND)
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
	add_custom_target(doc
		${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		COMMENT "Generating API documentation with Doxygen" VERBATIM
	)
endif()

# plugins
# need to include Qt5 core only in windows
add_library(SimpleHome SHARED plugins/SimpleHome.hh plugins/SimpleHome.cc)
target_link_libraries(SimpleHome Qt5::Core)
set_target_properties(SimpleHome PROPERTIES CXX_VISIBILITY_PRESET hidden)

add_library(Beethoven SHARED plugins/Beethoven.hh plugins/Beethoven.cc plugins/Beethoven.qrc)
target_link_libraries(Beethoven Qt5::Core)
set_target_properties(Beethoven PROPERTIES CXX_VISIBILITY_PRESET hidden)
