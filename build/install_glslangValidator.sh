#!/usr/bin/env bash

mkdir glslangValidator
cd glslangValidator

git clone git@github.com:KhronosGroup/glslang.git
cmake -G Unix\ Makefiles ./
make glslangValidator

ls -al

cp glslangValidator ../

cd ..

ls -al
