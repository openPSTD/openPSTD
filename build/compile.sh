#!/usr/bin/env bash
set -e

if [ ${TRAVIS_OS_NAME} = "linux" ]; then
    QT5DIR=/opt/qt55
else
    QT5DIR=/usr/local/opt/qt5
fi

cmake \
	-D RAPIDJSON_ROOT:PATH=$PWD/rapidjson \
	-D UNQLITE_INCLUDE:PATH=$PWD/unqlite \
	-D UNQLITE_LIB:PATH=$PWD/unqlite/libunqlite.a \
	-D EIGEN_INCLUDE:PATH=$PWD/eigen \
	-D Qt5_DIR:PATH=${QT5DIR} \
	-G Unix\ Makefiles ./

make OpenPSTD-gui
make OpenPSTD-test VERBOSE=1
