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

cmake_minimum_required(VERSION 3.0)
project(OpenPSTD)

#------------------------------------
# configuration options in the cache
set(BOOST_ROOT "" CACHE PATH "The boost root directory")
set(Qt5_DIR "" CACHE PATH "The directory to the Qt5 library")
set(EIGEN_INCLUDE "" CACHE PATH "The lib of eigen")
set(FFTWF_LIBRARY "" CACHE PATH "The lib of fftw3f")
set(FFTWF_INCLUDE_DIR "" CACHE PATH "The include dirs of fftw3f")
set(FFTWF_SHARED_OBJECT "" CACHE PATH "The dll/so file of fftw3f")

SET(OPENPSTD_VERSION_MAJOR 0 CACHE STRING "Major version number")
SET(OPENPSTD_VERSION_MINOR 0 CACHE STRING "Minor version number")
SET(OPENPSTD_VERSION_PATCH 0 CACHE STRING "Patch version number")
SET(OPENPSTD_SYSTEM_NAME "linux" CACHE STRING "platform in the packaged filename")

set(OPENPSTD_DEPENDENCIES_SEARCH_PATH "" CACHE STRING "(ADVANCED) List of paths (delimiter ;) where the prerequisites can be found")

#------------------------------------
# Version numbering
message(STATUS "Building version: ${OPENPSTD_VERSION_MAJOR}.${OPENPSTD_VERSION_MINOR}.${OPENPSTD_VERSION_PATCH}")

#------------------------------------
# General
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include(dependencies.cmake)

#------------------------------------
# Compiling flags
# Making sure it is compiled with c++11
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
# boost test framework flag
add_definitions(-DBOOST_TEST_DYN_LINK)

#------------------------------------
# include directories
include_directories(${Boost_INCLUDE_DIR})
include_directories(${Qt5_INCLUDE_DIRS})
include_directories(unqlite)
include_directories(${OPENGL_INCLUDE_DIR})
include_directories(${EIGEN_INCLUDE})
include_directories(${FFTWF_INCLUDE_DIR})

#------------------------------------
# parts
include(unqlite/unqlite.cmake)
include(kernel/kernel.cmake)
include(shared/shared.cmake)
include(CLI/CLI.cmake)
include(GUI/GUI.cmake)
include(test/test.cmake)
include(install.cmake)

