cmake_minimum_required(VERSION 2.8)
project(OpenPSTD)

#------------------------------------
# Rapidjson
add_definitions(-DRAPIDJSON_HAS_STDSTRING=1)

#------------------------------------
# Compiling flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

#------------------------------------
# Compiling flags
set(SOURCE_FILES unqlite.c unqlite.h)

#------------------------------------
# Compiling flags
add_library(unqlite STATIC ${SOURCE_FILES})
