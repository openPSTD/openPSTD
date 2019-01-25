cmake_minimum_required(VERSION 2.8)

project(OpenPSTD-cli)

set(SOURCES
	"main.cpp"
	"argumentparser.cpp"
	"fileio.cpp"
	"createcommand.cpp"
	"listcommand.cpp"
	"editcommand.cpp"
	"runcommand.cpp"
	"solver.cpp"
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
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})