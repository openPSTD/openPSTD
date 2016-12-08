#------------------------------------
# Kernel library

# Interface

# PSTD
SET(SOURCE_FILES_LIB
        kernel/PSTDKernel.cpp
        kernel/core/kernel_functions.cpp
        kernel/core/Domain.cpp
        kernel/core/Speaker.cpp
        kernel/core/Scene.cpp
        kernel/core/Receiver.cpp
        kernel/core/Boundary.cpp
        kernel/Solver.cpp
        kernel/core/Geometry.cpp
        kernel/core/WisdomCache.cpp
        kernel/KernelInterface.cpp)

# DG
SET(SOURCE_FILES_LIB ${SOURCE_FILES_LIB}
        kernel/DG/Advec.cpp
        kernel/DG/Coordinates.cpp
        kernel/DG/DG.cpp
        kernel/DG/DG2D.cpp
        kernel/DG/DG2DBuilders.cpp
        kernel/DG/DMatrix.cpp
        kernel/DG/GeometricFactors.cpp
        kernel/DG/Grad.cpp
        kernel/DG/Jacobi.cpp
        kernel/DG/LEE.cpp
        kernel/DG/LEE2D.cpp
        kernel/DG/Lift.cpp
        kernel/DG/Maxwell.cpp
        kernel/DG/Nodes.cpp
        kernel/DG/RK.cpp
        kernel/DG/Simplex.cpp
        kernel/DG/Vandermonde.cpp
        kernel/DG/WarpFactor.cpp
        )

# Mock
SET(SOURCE_FILES_LIB ${SOURCE_FILES_LIB}
        kernel/MockKernel.cpp)

add_library(OpenPSTD SHARED ${SOURCE_FILES_LIB})

target_include_directories(OpenPSTD PUBLIC ${Qt5_INCLUDE_DIRS})
target_include_directories(OpenPSTD PUBLIC ${EIGEN_INCLUDE})
target_include_directories(OpenPSTD PUBLIC ${Boost_INCLUDE_DIR})
target_include_directories(OpenPSTD PUBLIC ${FFTWF_INCLUDE_DIR})

target_link_libraries(OpenPSTD ${Boost_LIBRARIES})
target_link_libraries(OpenPSTD ${Qt5_LIBRARIES})
target_link_libraries(OpenPSTD ${FFTWF_LIBRARY})