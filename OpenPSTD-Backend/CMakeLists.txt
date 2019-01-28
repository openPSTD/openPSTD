cmake_minimum_required(VERSION 2.8)

project(OpenPSTD-cli)

enable_testing()
find_package(GTest REQUIRED)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 --coverage -fprofile-arcs -ftest-coverage")

set(SOURCES
	"main.cpp"
	"argumentparser.cpp"
	"fileio.cpp"
	"createcommand.cpp"
	"listcommand.cpp"
	"editcommand.cpp"
	"runcommand.cpp"
	"solver.cpp"
	"test/testcommand.cpp"
	"test/testargumentparser.cpp"
	"test/testcreatecommand.cpp"
	"test/testeditcommand.cpp"
	"test/testlistcommand.cpp"
	"test/testfileio.cpp"
	"test/testruncommand.cpp"
)

set(HEADERS
	"argumentparser.h"
	"configuration.h"
	"fileio.h"
	"createcommand.h"
	"listcommand.h"
	"editcommand.h"
	"settings.h"
	"runcommand.h"
	"solver.h"
	"test/testcommand.h"
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_link_libraries(${PROJECT_NAME} gcov)
target_link_libraries(${PROJECT_NAME} gtest gtest_main)
add_test(${PROJECT_NAME} ${PROJECT_NAME})