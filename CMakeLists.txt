########################################################################
# This file is part of openPSTD.                                       #
#                                                                      #
# openPSTD is free software: you can redistribute it and/or modify     #
# it under the terms of the GNU General Public License as published by #
# the Free Software Foundation, either version 3 of the License, or    #
# (at your option) any later version.                                  #
#                                                                      #
# openPSTD is distributed in the hope that it will be useful,          #
# but WITHOUT ANY WARRANTY; without even the implied warranty of       #
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        #
# GNU General Public License for more details.                         #
#                                                                      #
# You should have received a copy of the GNU General Public License    #
# along with openPSTD.  If not, see <http://www.gnu.org/licenses/>.    #
#                                                                      #
########################################################################

########################################################################
#
# Date: 17-09-2015
#
#
# Authors: M. R. Fortuin
#
#
########################################################################

cmake_minimum_required(VERSION 3.2)
project(OpenPSTD)

#------------------------------------
# configuration options in the cache
set(BOOST_ROOT "" CACHE PATH "The boost root directory")
set(Qt5_DIR "" CACHE PATH "The directory to the Qt5 library")
set(RAPIDJSON_ROOT "" CACHE PATH "The root directory of rapidjson")
set(UNQLITE_INCLUDE "" CACHE PATH "The include directory of unqlite")
set(UNQLITE_LIB "" CACHE PATH "The lib of unqlite")
set(EIGEN_INCLUDE "" CACHE PATH "The lib of eigen")

#------------------------------------
# General
set(CMAKE_INCLUDE_CURRENT_DIR ON)

#------------------------------------
# BOOST
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_DEBUG_RUNTIME OFF)

message(STATUS "Boost root hint: ${BOOST_ROOT}")
find_package(Boost 1.54.0 REQUIRED program_options unit_test_framework)
message(STATUS "Boost found: ${Boost_FOUND}")
message(STATUS "Boost lib path: ${Boost_LIBRARY_DIRS}")
message(STATUS "Boost include path: ${Boost_INCLUDE_DIRS}")
message(STATUS "Boost version: ${Boost_VERSION}")
message(STATUS "Boost libs: ${Boost_LIBRARIES}")
message(STATUS "Boost program_options lib: ${Boost_PROGRAM_OPTIONS_LIBRARY}")
message(STATUS "Boost test lib: ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}")

if(NOT Boost_FOUND)
  message(FATAL_ERROR "Missing Boost Libs.")
endif()

#------------------------------------
# QT5
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

#function for easy loading Qt modules
set(Qt5_CMAKE "lib/cmake")
set(Qt5_FIRST 1)
set(Qt5_FOUND 0)
macro(loadQtPackage QtModule)
    set(${QtModule}_DIR "${Qt5_DIR}/${Qt5_CMAKE}/${QtModule}")
    message(STATUS "Qt5 ${QtModule} dir: ${${QtModule}_DIR}")
    find_package(${QtModule})
    if(Qt5_FIRST)
        set(Qt5_FIRST 0)
        set(Qt5_FOUND 1)
    endif()

    if(Qt5_FOUND AND ${${QtModule}_FOUND})
        set(Qt5_FOUND 1)
    else()
        set(Qt5_FOUND 0)
    endif()
    set(Qt5_LIBRARIES "${Qt5_LIBRARIES}${${QtModule}_LIBRARIES};")
    set(Qt5_INCLUDE_DIRS "${Qt5_INCLUDE_DIRS}${${QtModule}_INCLUDE_DIRS};")
    set(Qt5_VERSION_STRING "${${QtModule}_VERSION_STRING}")
endmacro(loadQtPackage)

loadQtPackage(Qt5Widgets)
loadQtPackage(Qt5Core)
loadQtPackage(Qt5OpenGL)

message(STATUS "Qt5 found: ${Qt5_FOUND}")
message(STATUS "Qt5 lib path: ${Qt5_LIBRARIES}")
message(STATUS "Qt5 include path: ${Qt5_INCLUDE_DIRS}")
message(STATUS "Qt5 version: ${Qt5_VERSION_STRING}")

if (Qt5_POSITION_INDEPENDENT_CODE)
    SET(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

if(NOT Qt5Widgets_FOUND)
  message( FATAL_ERROR "Missing Qt5 Libs." )
endif()

#------------------------------------
# Rapidjson
add_definitions(-DRAPIDJSON_HAS_STDSTRING=1)
set(RAPIDJSON_INCLUDE "${RAPIDJSON_ROOT}/include")
message(STATUS "Rapidjson include path: ${RAPIDJSON_INCLUDE}")

#------------------------------------
# Unqlite
message(STATUS "Unqlite include path: ${UNQLITE_INCLUDE}")
message(STATUS "Unqlite lib path: ${UNQLITE_LIB}")

#------------------------------------
# Eigen
message(STATUS "Eigen include path: ${EIGEN_INCLUDE}")

#------------------------------------
# OpenGL
find_package(OpenGL)
message(STATUS "OpenGL found: ${OPENGL_FOUND}")
message(STATUS "OpenGL xmesa found: ${OPENGL_XMESA_FOUND}")
message(STATUS "OpenGL glu found: ${OPENGL_GLU_FOUND}")
message(STATUS "OpenGL include path: ${OPENGL_INCLUDE_DIR}")
message(STATUS "OpenGL lib path: ${OPENGL_LIBRARIES}")

#------------------------------------
# include directories
include_directories(${Boost_INCLUDE_DIR})
include_directories(${Qt5_INCLUDE_DIRS})
include_directories(${RAPIDJSON_INCLUDE})
include_directories(${UNQLITE_INCLUDE})
include_directories(${OPENGL_INCLUDE_DIR})
include_directories(${EIGEN_INCLUDE})

#------------------------------------
# Compiling flags
# Making sure it is compiled with c++11
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
# boost test framework flag
add_definitions(-DBOOST_TEST_DYN_LINK)

#------------------------------------
# Kernel
set(SOURCE_FILES_CLI kernel/ConsoleOutput.cpp kernel/KernelFacade.cpp kernel/core/kernel_functions.cpp kernel/core/KernelDomain.cpp kernel/core/KernelDomain.h)
add_executable(OpenPSTD-cli main-cli.cpp ${SOURCE_FILES_CLI})

#------------------------------------
# UI
# CLI source files
# set(SOURCE_FILES_GUI ${SOURCE_FILES_CLI})
# Operations
set(SOURCE_FILES_GUI ${SOURCE_FILES_GUI} operations/BaseOperation.cpp operations/FileOperations.cpp operations/EditOperations.cpp)
set(SOURCE_FILES_GUI ${SOURCE_FILES_GUI} operations/ViewOperations.cpp operations/InitializationOperation.cpp)
set(SOURCE_FILES_GUI ${SOURCE_FILES_GUI} operations/MouseOperations.cpp operations/SelectionOperations.cpp)
# mouse handlers
set(SOURCE_FILES_GUI ${SOURCE_FILES_GUI} MouseHandlers.cpp)
# OpenGL viewer
set(SOURCE_FILES_GUI ${SOURCE_FILES_GUI} Model.cpp Viewer2D.cpp GridLayer.cpp SceneLayer.cpp InteractiveLayer.cpp IconLayer.cpp)
# UI files
set(SOURCE_FILES_GUI ${SOURCE_FILES_GUI} MainWindow.cpp DomainProperties.cpp DocumentSettings.cpp)
# Resource files
set(SOURCE_FILES_GUI ${SOURCE_FILES_GUI} Resources.qrc)
# Remaining
set(SOURCE_FILES_GUI ${SOURCE_FILES_GUI} Model.cpp PSTDFile.cpp Colors.cpp Snapping.cpp PstdAlgorithm.cpp InvalidationData.cpp Edges.cpp Edges.h)
add_executable(OpenPSTD-gui ${SOURCE_FILES_GUI} main-gui.cpp)

#------------------------------------
# Test
# GUI source files
set(SOURCE_FILES_TEST ${SOURCE_FILES_GUI})
# Remaining
set(SOURCE_FILES_TEST ${SOURCE_FILES_TEST} Edges-test.cpp)
add_executable(OpenPSTD-test ${SOURCE_FILES_TEST} main-test.cpp)

#------------------------------------
# link libraries
target_link_libraries(OpenPSTD-cli ${Boost_PROGRAM_OPTIONS_LIBRARY})

target_link_libraries(OpenPSTD-gui ${Boost_PROGRAM_OPTIONS_LIBRARY})
target_link_libraries(OpenPSTD-gui ${Qt5_LIBRARIES})
target_link_libraries(OpenPSTD-gui ${UNQLITE_LIB})
target_link_libraries(OpenPSTD-gui ${OPENGL_LIBRARIES})

target_link_libraries(OpenPSTD-test ${Boost_PROGRAM_OPTIONS_LIBRARY})
target_link_libraries(OpenPSTD-test ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
target_link_libraries(OpenPSTD-test ${Qt5_LIBRARIES})
target_link_libraries(OpenPSTD-test ${UNQLITE_LIB})
target_link_libraries(OpenPSTD-test ${OPENGL_LIBRARIES})
