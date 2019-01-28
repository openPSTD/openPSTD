#!/bin/bash

# Compile the program
dir=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
cd $dir
cd /home/jeroen/programming/openPSTD/Backend_new_git/
[ -d build_testing ] && rm -r build_testing/
mkdir build_testing
cd build_testing
cmake ../OpenPSTD-Backend/
make

# Run the tests
./OpenPSTD-cli test || exit 1

# Compute the code coverage
cd CMakeFiles/OpenPSTD-cli.dir/
gcov main.cpp.o
lcov --capture --directory . --output-file coverage.info
lcov --remove coverage.info '/usr/include/*' -o filtered-coverage.info
genhtml filtered-coverage.info --output-directory out