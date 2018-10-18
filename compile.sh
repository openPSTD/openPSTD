#!/bin/bash

# Go to project directory
cd "${0%/*}"

# Create a build directory
mkdir build
cd build
mkdir tmp
cd tmp

# Build the project
qmake ../../openPSTD/openPSTD.pro
make

# Create an output directory
mv openPSTD ../

# Remove the tmp directory
cd ../
rm -r tmp/