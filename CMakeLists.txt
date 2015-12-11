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
set(RAPIDJSON_ROOT "" CACHE PATH "The root directory of rapidjson")
set(UNQLITE_INCLUDE "" CACHE PATH "The include directory of unqlite")
set(UNQLITE_LIB "" CACHE PATH "The lib of unqlite")
set(EIGEN_INCLUDE "" CACHE PATH "The lib of eigen")

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

if ("${OPENPSTD_DEPENDENCIES_SEARCH_PATH}" STREQUAL "")
    SET(OPENPSTD_DEPENDENCIES_SEARCH_PATH "${Boost_LIBRARY_DIRS}")
else()
    SET(OPENPSTD_DEPENDENCIES_SEARCH_PATH "${OPENPSTD_DEPENDENCIES_SEARCH_PATH};${Boost_LIBRARY_DIRS}")
endif()

if (NOT Boost_FOUND)
    message(FATAL_ERROR "Missing Boost Libs.")
endif ()

#------------------------------------
# QT5
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

#function for easy loading Qt modules
set(Qt5_LIBS "lib")
set(Qt5_CMAKE "${Qt5_LIBS}/cmake")
set(Qt5_FIRST 1)
set(Qt5_FOUND 0)
macro(loadQtPackage QtModule)
    # set the correct path
    set(${QtModule}_DIR "${Qt5_DIR}/${Qt5_CMAKE}/${QtModule}")
    # message about location
    message(STATUS "Qt5 ${QtModule} dir: ${${QtModule}_DIR}")
    # load package
    find_package(${QtModule})

    #checks if it's found
    if (Qt5_FIRST)
        set(Qt5_FIRST 0)
        set(Qt5_FOUND 1)
    endif ()
    if (Qt5_FOUND AND ${${QtModule}_FOUND})
        set(Qt5_FOUND 1)
    else()
        set(Qt5_FOUND 0)
    endif ()

    # sets the target
    set(Qt5_LIBRARIES "${Qt5_LIBRARIES}${${QtModule}_LIBRARIES};")

    # gets the location of the binary
    get_target_property(Qt5_${QtModule}_location ${${QtModule}_LIBRARIES} LOCATION)
    # messages about the target
    message(STATUS "Qt5 ${QtModule} location: ${Qt5_${QtModule}_location}")
    set(Qt5_LIBRARIES_LOCATIONS "${Qt5_LIBRARIES_LOCATIONS}${Qt5_${QtModule}_location};")

    # gets the include path
    set(Qt5_INCLUDE_DIRS "${Qt5_INCLUDE_DIRS}${${QtModule}_INCLUDE_DIRS};")

    # gets the version and checks if Qt5 is PIC
    set(Qt5_VERSION_STRING "${${QtModule}_VERSION_STRING}")
    if (Qt5_POSITION_INDEPENDENT_CODE)
        SET(CMAKE_POSITION_INDEPENDENT_CODE ON)
    endif ()
endmacro(loadQtPackage)

loadQtPackage(Qt5Widgets)
loadQtPackage(Qt5Core)
loadQtPackage(Qt5OpenGL)

SET(OPENPSTD_DEPENDENCIES_SEARCH_PATH "${OPENPSTD_DEPENDENCIES_SEARCH_PATH};${Qt5_DIR}/${Qt5_LIBS}")

message(STATUS "Qt5 found: ${Qt5_FOUND}")
message(STATUS "Qt5 libs: ${Qt5_LIBRARIES}")
message(STATUS "Qt5 lib path: ${Qt5_LIBRARIES_LOCATIONS}")
message(STATUS "Qt5 include path: ${Qt5_INCLUDE_DIRS}")
message(STATUS "Qt5 version: ${Qt5_VERSION_STRING}")
message(STATUS "Qt5 locations: ${Qt5_LIBRARIES_LOCATIONS}")

if(NOT Qt5_FOUND)
  message( FATAL_ERROR "Missing Qt5 Libs." )
endif()

#------------------------------------
# Rapidjson
add_definitions(-DRAPIDJSON_HAS_STDSTRING=1)
set(RAPIDJSON_INCLUDE "${RAPIDJSON_ROOT}/include")
message(STATUS "Rapidjson include path: ${RAPIDJSON_INCLUDE}")

#------------------------------------
# Unqlite
get_filename_component(UNQLITE_LIB_DIR ${UNQLITE_LIB} DIRECTORY)

message(STATUS "Unqlite include path: ${UNQLITE_INCLUDE}")
message(STATUS "Unqlite lib directory: ${UNQLITE_LIB_DIR}")
message(STATUS "Unqlite lib path: ${UNQLITE_LIB}")

SET(OPENPSTD_DEPENDENCIES_SEARCH_PATH "${OPENPSTD_DEPENDENCIES_SEARCH_PATH};${UNQLITE_LIB_DIR}")

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
set(SOURCE_FILES_CLI kernel/ConsoleOutput.cpp kernel/KernelFacade.cpp
        kernel/core/kernel_functions.cpp kernel/core/Domain.cpp kernel/core/Domain.h
        kernel/core/Speaker.cpp kernel/core/Speaker.h kernel/core/Scene.cpp kernel/core/Scene.h
        kernel/core/Receiver.cpp kernel/core/Receiver.h kernel/core/Boundary.cpp kernel/core/Boundary.h
        kernel/Solver.cpp kernel/Solver.h
        kernel/core/Geometry.cpp kernel/core/Geometry.h PSTDFile.h PSTDFile.cpp
        kernel/core/wave_numbers.cpp kernel/core/wave_numbers.h InvalidationData.cpp)
add_executable(OpenPSTD-cli main-cli.cpp ${SOURCE_FILES_CLI})

#------------------------------------
# UI
# CLI source files
set(SOURCE_FILES_GUI ${SOURCE_FILES_CLI})
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
target_link_libraries(OpenPSTD-cli ${Boost_LIBRARIES})
target_link_libraries(OpenPSTD-cli ${UNQLITE_LIB})

target_link_libraries(OpenPSTD-gui ${Boost_LIBRARIES})
target_link_libraries(OpenPSTD-gui ${Qt5_LIBRARIES})
target_link_libraries(OpenPSTD-gui ${UNQLITE_LIB})
target_link_libraries(OpenPSTD-gui ${OPENGL_LIBRARIES})

target_link_libraries(OpenPSTD-test ${Boost_LIBRARIES})
target_link_libraries(OpenPSTD-test ${Qt5_LIBRARIES})
target_link_libraries(OpenPSTD-test ${UNQLITE_LIB})
target_link_libraries(OpenPSTD-test ${OPENGL_LIBRARIES})

#------------------------------------
# INSTALL
install(TARGETS OpenPSTD-gui DESTINATION .)
#install(TARGETS OpenPSTD-cli DESTINATION .)

if(APPLE)
    # paths to executables
    set(APPS "${CMAKE_SOURCE_DIR}/_CPack_Packages/${OPENPSTD_SYSTEM_NAME}/Bundle/OpenPSTD-${OPENPSTD_VERSION_MAJOR}.${OPENPSTD_VERSION_MINOR}.${OPENPSTD_VERSION_PATCH}-${OPENPSTD_SYSTEM_NAME}/OpenPSTD.app/Contents/Resources/OpenPSTD-gui")
    set(LIBS "/usr/local/lib/libboost_system-mt.dylib;/usr/local/lib/libboost_timer-mt.dylib;/usr/local/lib/libboost_chrono-mt.dylib")

    message(STATUS "Mac OS X app: ${APPS}")
    message(STATUS "Mac OS X dependencies: ${OPENPSTD_DEPENDENCIES_SEARCH_PATH}")

    INSTALL(CODE "
	   include(BundleUtilities)
	   fixup_bundle(\"${APPS}\"   \"${LIBS}\"   \"${OPENPSTD_DEPENDENCIES_SEARCH_PATH}\")
	   " COMPONENT Runtime)
endif()

set(CPACK_BUNDLE_NAME OpenPSTD)
set(CPACK_BUNDLE_ICON OpenPSTD.icns)
set(CPACK_BUNDLE_PLIST Info.plist)
set(CPACK_BUNDLE_STARTUP_COMMAND OpenPSTD-gui)

set(CPACK_PACKAGE_VERSION_MAJOR ${OPENPSTD_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${OPENPSTD_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${OPENPSTD_VERSION_PATCH})
set(CPACK_SYSTEM_NAME ${OPENPSTD_SYSTEM_NAME})

include(CPack)
