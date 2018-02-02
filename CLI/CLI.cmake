#------------------------------------
# UI CLI
set(SOURCE_FILES_CLI CLI/edit.cpp CLI/output.cpp CLI/exportCLI.cpp)
add_executable(OpenPSTD-cli CLI/main.cpp ${SOURCE_FILES_CLI})

target_include_directories(OpenPSTD-cli PUBLIC ${Qt5_INCLUDE_DIRS})
target_include_directories(OpenPSTD-cli PUBLIC unqlite)
target_include_directories(OpenPSTD-cli PUBLIC ${Boost_INCLUDE_DIR})

target_link_libraries(OpenPSTD-cli OpenPSTD)
target_link_libraries(OpenPSTD-cli OpenPSTD-shared)
target_link_libraries(OpenPSTD-cli unqlite)
target_link_libraries(OpenPSTD-cli ${Boost_LIBRARIES})
target_link_libraries(OpenPSTD-cli ${Qt5_LIBRARIES})
