#------------------------------------
# UI CLI
set(SOURCE_FILES_CLI CLI/edit.cpp CLI/output.cpp)
add_executable(OpenPSTD-cli CLI/main.cpp ${SOURCE_FILES_CLI})

target_link_libraries(OpenPSTD-cli OpenPSTD)
target_link_libraries(OpenPSTD-cli OpenPSTD-shared)
target_link_libraries(OpenPSTD-cli unqlite)
target_link_libraries(OpenPSTD-cli ${Boost_LIBRARIES})
target_link_libraries(OpenPSTD-cli ${Qt5_LIBRARIES})