#!/usr/bin/env bash
set -e



if [ ${TARGET} = "windows" ]; then
    CMAKE=i686-w64-mingw32.static-cmake
    PACKAGE=ZIP
else
    CMAKE=cmake
    if [ ${TRAVIS_OS_NAME} = "linux" ]; then
        QT5DIR=/opt/qt55
        PACKAGE=TGZ
    else
        QT5DIR=/usr/local/opt/qt5
        PACKAGE=Bundle
    fi
fi

${CMAKE} \
	-D EIGEN_INCLUDE:PATH=$PWD/eigen \
	-D Qt5_DIR:PATH=${QT5DIR} \
	-D FFTWF_LIBRARY:PATH=/usr/lib/x86_64-linux-gnu/libfftw3f.a \
	-D FFTWF_SHARED_OBJECT:PATH=/usr/lib/x86_64-linux-gnu/libfftw3f.so \
	-D HDF5_INCLUDE:PATH=$PWD/hdf5-1.8.16/hdf5/include \
	-D HDF5_LIBRARY:PATH=$PWD/hdf5-1.8.16/hdf5/lib/libhdf5.a \
	-D HDF5_HL_LIBRARY:PATH=$PWD/hdf5-1.8.16/hdf5/lib/libhdf5_hl.a \
	-D CPACK_GENERATOR=${PACKAGE} \
	-D OPENPSTD_VERSION_MAJOR=${OPENPSTD_MAJOR_VERSION} \
	-D OPENPSTD_VERSION_MINOR=${OPENPSTD_MINOR_VERSION} \
	-D OPENPSTD_VERSION_PATCH=${TRAVIS_BUILD_NUMBER} \
	-D OPENPSTD_SYSTEM_NAME=${TRAVIS_OS_NAME} \
	-G Unix\ Makefiles ./

make OpenPSTD-cli
make OpenPSTD-gui
make OpenPSTD-test
sudo cpack -V -G ${PACKAGE}
ls -alh
