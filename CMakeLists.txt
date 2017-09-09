cmake_minimum_required(VERSION 3.8)
project(wacrana)

set(CMAKE_CXX_STANDARD 14)

# required for Qt
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5Core)
find_package(Qt5Widgets)
find_package(Qt5WebEngine)
find_package(Qt5WebEngineWidgets)

# Require dot, treat the other components as optional
find_package(Doxygen
    REQUIRED dot
    OPTIONAL_COMPONENTS mscgen dia
)

include_directories(${Qt5Core_INCLUDE_DIRS})

file(GLOB SOURCE_FILES src/*.cc src/*.hh res/*.qrc include/*.hpp)
add_executable(wacrana WIN32 ${SOURCE_FILES})

target_link_libraries(wacrana Qt5::Widgets Qt5::WebEngine Qt5::WebEngineWidgets)

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
add_library(SimpleHome SHARED plugins/SimpleHome.hh plugins/SimpleHome.cc)
target_link_libraries(wacrana Qt5::Core)
