#!/usr/bin/env bash
#-D EIGEN_INCLUDE:PATH=/home/travis/build/micfort/openPSTD/rapidjson
#-D Qt5_DIR:PATH=/opt/qt55 \

cmake \
	-D RAPIDJSON_ROOT:PATH=$PWD/rapidjson \
	-D UNQLITE_INCLUDE:PATH=$PWD/unqlite \
	-D UNQLITE_LIB:PATH=$PWD/unqlite/libunqlite.a \
	-D Qt5_DIR:PATH=/opt/qt55 \
	-G Unix\ Makefiles ./

make OpenPSTD-gui
make OpenPSTD-test
