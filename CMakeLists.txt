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

#------------------------------------
# BOOST
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_DEBUG_RUNTIME OFF)

message(STATUS "Boost root hint: ${BOOST_ROOT}")
find_package(Boost 1.54.0 REQUIRED program_options unit_test_framework chrono system timer regex)
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
# FFTW3
message(STATUS "FFTW3F include path: ${FFTWF_INCLUDE_DIR}")
message(STATUS "FFTW3F lib path: ${FFTWF_LIBRARY}")

#------------------------------------
# Rapidjson
add_definitions(-DRAPIDJSON_HAS_STDSTRING=1)
set(RAPIDJSON_INCLUDE "${RAPIDJSON_ROOT}/include")
message(STATUS "Rapidjson include path: ${RAPIDJSON_INCLUDE}")

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
include_directories(unqlite)
include_directories(${OPENGL_INCLUDE_DIR})
include_directories(${EIGEN_INCLUDE})
include_directories(${FFTWF_INCLUDE_DIR})

#------------------------------------
# Compiling flags
# Making sure it is compiled with c++11
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
# boost test framework flag
add_definitions(-DBOOST_TEST_DYN_LINK)

#------------------------------------
# Unqlite library
add_library(unqlite SHARED unqlite/unqlite.c)

#------------------------------------
# Kernel library
SET(SOURCE_FILES_LIB kernel/PSTDKernel.cpp
        kernel/core/kernel_functions.cpp kernel/core/Domain.cpp
        kernel/core/Speaker.cpp kernel/core/Scene.cpp
        kernel/core/Receiver.cpp kernel/core/Boundary.cpp
        kernel/Solver.cpp
        kernel/core/Geometry.cpp PSTDFile.cpp
        kernel/core/wave_numbers.cpp InvalidationData.cpp
        kernel/KernelInterface.cpp kernel/MockKernel.cpp)
add_library(OpenPSTD SHARED ${SOURCE_FILES_LIB})

target_link_libraries(OpenPSTD ${Boost_LIBRARIES})
target_link_libraries(OpenPSTD unqlite)
target_link_libraries(OpenPSTD ${Qt5_LIBRARIES})
target_link_libraries(OpenPSTD ${FFTWF_LIBRARY})

#------------------------------------
# UI CLI
set(SOURCE_FILES_CLI kernel/ConsoleOutput.cpp edit-cli.cpp output-cli.cpp output-cli.h)
add_executable(OpenPSTD-cli main-cli.cpp ${SOURCE_FILES_CLI})

target_link_libraries(OpenPSTD-cli OpenPSTD)
target_link_libraries(OpenPSTD-cli unqlite)

#------------------------------------
# UI Graphical
# Operations
set(SOURCE_FILES_GUI ${SOURCE_FILES_GUI} operations/BaseOperation.cpp operations/FileOperations.cpp
        operations/EditOperations.cpp operations/ViewOperations.cpp
        operations/InitializationOperation.cpp operations/MouseOperations.cpp
        operations/SelectionOperations.cpp)
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

# create extra Qt5 code
set_target_properties(OpenPSTD-gui
        PROPERTIES AUTOMOC ON
        AUTOUIC ON
        AUTORCC ON)

target_link_libraries(OpenPSTD-gui OpenPSTD)
target_link_libraries(OpenPSTD-gui ${Boost_LIBRARIES})
target_link_libraries(OpenPSTD-gui ${Qt5_LIBRARIES})
target_link_libraries(OpenPSTD-gui unqlite)
target_link_libraries(OpenPSTD-gui ${OPENGL_LIBRARIES})

#------------------------------------
# Test
# Source files
set(SOURCE_FILES_TEST ${SOURCE_FILES_TEST} ${SOURCE_FILES_GUI})
# GUI test files
set(SOURCE_FILES_TEST ${SOURCE_FILES_TEST} Edges-test.cpp)
# Kernel test files
set(SOURCE_FILES_TEST ${SOURCE_FILES_TEST} kernel/tests/kernel_functions.cpp
        kernel/tests/Speaker.cpp kernel/tests/Geometry.cpp kernel/tests/Domain.cpp
        kernel/tests/wave_numbers.cpp)
add_executable(OpenPSTD-test ${SOURCE_FILES_TEST} main-test.cpp)

# create extra Qt5 code
set_target_properties(OpenPSTD-test
        PROPERTIES AUTOMOC ON
        AUTOUIC ON
        AUTORCC ON)

target_link_libraries(OpenPSTD-test OpenPSTD)
target_link_libraries(OpenPSTD-test ${Boost_LIBRARIES})
target_link_libraries(OpenPSTD-test ${Qt5_LIBRARIES})
target_link_libraries(OpenPSTD-test unqlite)
target_link_libraries(OpenPSTD-test ${OPENGL_LIBRARIES})

#------------------------------------
# INSTALL
if(APPLE)
    install(TARGETS OpenPSTD-gui DESTINATION .)
    #install(TARGETS OpenPSTD-cli DESTINATION .)

    # paths to executables
    set(APPS "${CMAKE_SOURCE_DIR}/_CPack_Packages/${OPENPSTD_SYSTEM_NAME}/Bundle/OpenPSTD-${OPENPSTD_VERSION_MAJOR}.${OPENPSTD_VERSION_MINOR}.${OPENPSTD_VERSION_PATCH}-${OPENPSTD_SYSTEM_NAME}/OpenPSTD.app/Contents/Resources/OpenPSTD-gui")

    message(STATUS "Mac OS X app: ${APPS}")
    message(STATUS "Mac OS X dependencies: ${OPENPSTD_DEPENDENCIES_SEARCH_PATH}")

    INSTALL(CODE "
	   include(BundleUtilities)
	   fixup_bundle(\"${APPS}\"   \"\"   \"${OPENPSTD_DEPENDENCIES_SEARCH_PATH}\")
	   " COMPONENT Runtime)
elseif(WIN32)
    install(TARGETS OpenPSTD DESTINATION OpenPSTD)
    install(TARGETS OpenPSTD-gui DESTINATION OpenPSTD)
    #install(TARGETS OpenPSTD-cli DESTINATION OpenPSTD)
    install(TARGETS unqlite DESTINATION OpenPSTD)

    install(FILES ${Boost_LIBRARIES} DESTINATION OpenPSTD)
    install(FILES ${QtCore_location} DESTINATION OpenPSTD)
    install(FILES ${QtWidgets_location} DESTINATION OpenPSTD)
    install(FILES ${QtOpenGL_location} DESTINATION OpenPSTD)
    install(FILES ${Qt5_LIBRARIES_LOCATIONS} DESTINATION OpenPSTD)
    install(FILES ${FFTWF_SHARED_OBJECT} DESTINATION OpenPSTD)

elseif(UNIX)
    install(TARGETS OpenPSTD DESTINATION lib)
    install(TARGETS OpenPSTD-gui DESTINATION bin)
    #install(TARGETS OpenPSTD-cli DESTINATION bin)
    install(TARGETS unqlite DESTINATION lib)

    install(FILES ${Boost_LIBRARIES} DESTINATION lib)
    install(FILES ${QtCore_location} DESTINATION lib)
    install(FILES ${QtWidgets_location} DESTINATION lib)
    install(FILES ${QtOpenGL_location} DESTINATION lib)
    install(FILES ${Qt5_LIBRARIES_LOCATIONS} DESTINATION lib)
    install(FILES ${FFTWF_SHARED_OBJECT} DESTINATION OpenPSTD)
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
