#------------------------------------
# Test
# Source files
set(SOURCE_FILES_TEST ${SOURCE_FILES_TEST} ${SOURCE_FILES_GUI})
# GUI test files
set(SOURCE_FILES_TEST ${SOURCE_FILES_TEST} test/GUI/Edges-test.cpp)
# Kernel test files
set(SOURCE_FILES_TEST ${SOURCE_FILES_TEST} test/Kernel/kernel_functions.cpp
        test/Kernel/Speaker.cpp test/Kernel/Geometry.cpp test/Kernel/Domain.cpp
        test/Kernel/wave_numbers.cpp)
add_executable(OpenPSTD-test ${SOURCE_FILES_TEST} test/main.cpp)

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