#!/bin/bash

# Go to project directory
cd "${0%/*}"

# Create a build directory
mkdir build
cd build

# Build the project
qmake ../openPSTD/openPSTD.pro
make