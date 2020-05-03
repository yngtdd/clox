## Build the clox application and its test suite.
##
cmake_minimum_required(VERSION 3.12)

project(clox
    VERSION 0.1.0.0
    LANGUAGES C
    DESCRIPTION "Clox interpreter"
)

set(CMAKE_BINARY_DIR
    ${CMAKE_SOURCE_DIR}/build
)

set(EXECUTABLE_OUTPUT_PATH
    ${CMAKE_SOURCE_DIR}/bin
)

set(LIBRARY_OUTPUT_PATH
    ${CMAKE_SOURCE_DIR}/bin
)

include_directories(${PROJECT_SOURCE_DIR}/include/)
include_directories(${CMAKE_BINARY_DIR})
file(GLOB_RECURSE CLOX_SRC
	"${PROJECT_SOURCE_DIR}/include/*.h"
	"${PROJECT_SOURCE_DIR}/src/*.c"
)

add_executable(clox ${CLOX_SRC})

