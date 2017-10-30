cmake_minimum_required(VERSION 3.8)
project(wacrana VERSION 0.1)

set(CMAKE_CXX_STANDARD 14)

# required for Qt
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

if (WIN32)
	set(Boost_USE_STATIC_LIBS        ON) # only find static libs
	set(Boost_USE_MULTITHREADED      ON)
	set(Boost_USE_STATIC_RUNTIME    OFF)
endif()

find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5WebEngine REQUIRED)
find_package(Qt5WebEngineWidgets REQUIRED)
find_package(Threads REQUIRED)
find_package(Boost REQUIRED COMPONENTS system filesystem)

find_package(Doxygen)

include_directories(
	${Qt5Core_INCLUDE_DIRS}
	${Boost_INCLUDE_DIRS}
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${CMAKE_CURRENT_SOURCE_DIR}/src
	${DLIB_DIR}
)

file(GLOB INCLUDE_FILES include/*.hpp)
file(GLOB SOURCE_FILES src/*.cc src/*.hh res/*.qrc BrightFuture/*.hh BrightFuture/executor/*.hh BrightFuture/executor/*.cc)
add_executable(wacrana ${SOURCE_FILES} ${INCLUDE_FILES})

target_link_libraries(
	wacrana
	Qt5::Widgets
	Qt5::WebEngine
	Qt5::WebEngineWidgets
	Boost::disable_autolinking
	Boost::filesystem
	Threads::Threads
	${CMAKE_DL_LIBS}
)

add_executable(headless headless/headless.cc)
target_link_libraries(
	headless
	Threads::Threads
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
add_library(SimpleHome SHARED plugins/SimpleHome.hh plugins/SimpleHome.cc plugins/ResourceLoader.hh ${INCLUDE_FILES})
target_link_libraries(SimpleHome Qt5::Core Qt5::Gui)
set_target_properties(SimpleHome PROPERTIES CXX_VISIBILITY_PRESET hidden)

file(GLOB BEETHOVEN_SRC plugins/beethoven/*.cc plugins/beethoven/*.hh plugins/beethoven/res/*.qrc)
add_library(Beethoven SHARED ${BEETHOVEN_SRC} ${INCLUDE_FILES})
target_link_libraries(Beethoven Qt5::Core Qt5::Gui)
set_target_properties(Beethoven PROPERTIES CXX_VISIBILITY_PRESET hidden)

# include BridgeFuture
add_subdirectory(BrightFuture)

set(CMAKE_CXX_STANDARD 11)
add_executable(rvm_test dlib/rvm_test.cc)
#target_link_libraries(rvm_test dlib)
