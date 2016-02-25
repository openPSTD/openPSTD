#!/usr/bin/env bash
set -e

if [ ${TRAVIS_OS_NAME} = "linux" ]; then
    QT5DIR=/opt/qt55
    PACKAGE=TGZ

    FFTWFLIB=/usr/lib/x86_64-linux-gnu/libfftw3f.a
    FFTWFSHAREDOBJ=/usr/lib/x86_64-linux-gnu/libfftw3f.so

    HDF5LIB=$PWD/hdf5-1.8.16/hdf5/lib/libhdf5.a
    HDF5HLLIB=$PWD/hdf5-1.8.16/hdf5/lib/libhdf5_hl.a
else
    QT5DIR=/usr/local/opt/qt5
    PACKAGE=Bundle

    FFTWFLIB=/usr/local/lib/libfftw3f.a
    FFTWFSHAREDOBJ=""

    HDF5LIB=$PWD/hdf5-1.8.16/hdf5/lib/libhdf5.a
    HDF5HLLIB=$PWD/hdf5-1.8.16/hdf5/lib/libhdf5_hl.a
fi

cmake \
	-D EIGEN_INCLUDE:PATH=$PWD/eigen \
	-D Qt5_DIR:PATH=${QT5DIR} \
	-D FFTWF_LIBRARY:PATH=${FFTWFLIB} \
	-D FFTWF_SHARED_OBJECT:PATH=${FFTWFSHAREDOBJ} \
	-D HDF5_INCLUDE:PATH=$PWD/hdf5-1.8.16/hdf5/include \
	-D HDF5_LIBRARY:PATH=${HDF5LIB} \
	-D HDF5_HL_LIBRARY:PATH=${HDF5HLLIB} \
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
