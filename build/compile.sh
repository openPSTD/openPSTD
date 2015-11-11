#!/usr/bin/env bash
set -e

if [ ${TRAVIS_OS_NAME} = "linux" ]; then
    QT5DIR=/opt/qt55
    PACKAGE=STGZ;TGZ
else
    QT5DIR=/usr/local/opt/qt5
    PACKAGE=Bundle
fi

cmake \
	-D RAPIDJSON_ROOT:PATH=$PWD/rapidjson \
	-D UNQLITE_INCLUDE:PATH=$PWD/unqlite \
	-D UNQLITE_LIB:PATH=$PWD/unqlite/libunqlite.a \
	-D EIGEN_INCLUDE:PATH=$PWD/eigen \
	-D Qt5_DIR:PATH=${QT5DIR} \
	-D CPACK_GENERATOR=${PACKAGE} \
	-G Unix\ Makefiles ./

make OpenPSTD-gui
make OpenPSTD-test
make package
ls -al
