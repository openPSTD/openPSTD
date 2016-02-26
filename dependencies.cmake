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
# HDF5
message(STATUS "HDF5 include path: ${HDF5_INCLUDE}")
message(STATUS "HDF5 lib path: ${HDF5_LIBRARY}")
message(STATUS "HDF5 HL lib path: ${HDF5_HL_LIBRARY}")

