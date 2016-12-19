#!/usr/bin/env bash

git clone https://github.com/KhronosGroup/glslang.git

cd glslang

cmake -G Unix\ Makefiles ./
make glslangValidator

ls -al
ls -al StandAlone

cp glslangValidator ../

cd ..

ls -al
