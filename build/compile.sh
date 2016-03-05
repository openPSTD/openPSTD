#!/usr/bin/env bash
set -e



if [ ${TARGET} = "win64" ]; then
    MXEDIR=/usr/lib/mxe
    CMAKE=${MXEDIR}/usr/bin/${WINTARGET_PATH}-cmake
    PACKAGE=ZIP
    FFTLIBPATH=${MXEDIR}/usr/${WINTARGET_PATH}/bin/libfftw3f-3.dll
    FFTSOPATH=
    HDF5LIBPATH=${MXEDIR}/usr/${WINTARGET_PATH}/bin/libhdf5-8.dll
    HDF5HLLIBPATH=${MXEDIR}/usr/${WINTARGET_PATH}/bin/libhdf5_hl-8.dll
    HDF5INCLUDEPATH=${MXEDIR}/usr/${WINTARGET_PATH}/include/
else
    CMAKE=cmake
    if [ ${TRAVIS_OS_NAME} = "linux" ]; then
        QT5DIR=/opt/qt55
        PACKAGE=TGZ

        FFTLIBPATH=/usr/lib/x86_64-linux-gnu/libfftw3f.a
        FFTSOPATH=/usr/lib/x86_64-linux-gnu/libfftw3f.so

        HDF5LIBPATH=$PWD/hdf5-1.8.16/hdf5/lib/libhdf5.a
        HDF5HLLIBPATH=$PWD/hdf5-1.8.16/hdf5/lib/libhdf5_hl.a
        HDF5INCLUDEPATH=$PWD/hdf5-1.8.16/hdf5/include
    else
        QT5DIR=/usr/local/opt/qt5
        PACKAGE=Bundle

        FFTLIBPATH=/usr/local/lib/libfftw3f.a
        FFTSOPATH=""

        HDF5LIBPATH=/Users/travis/build/openPSTD/openPSTD/hdf5-1.8.16/hdf5/lib/libhdf5.a
        HDF5HLLIBPATH=/Users/travis/build/openPSTD/openPSTD/hdf5-1.8.16/hdf5/lib/libhdf5_hl.a
        HDF5INCLUDEPATH=/Users/travis/build/openPSTD/openPSTD/hdf5-1.8.16/hdf5/include
    fi
fi

${CMAKE} \
	-D EIGEN_INCLUDE:PATH=$PWD/eigen \
	-D Qt5_DIR:PATH=${QT5DIR} \
	-D FFTWF_LIBRARY:PATH=${FFTLIBPATH} \
	-D FFTWF_SHARED_OBJECT:PATH=${FFTSOPATH} \
	-D HDF5_INCLUDE:PATH=${HDF5INCLUDEPATH} \
	-D HDF5_LIBRARY:PATH=${HDF5LIBPATH} \
	-D HDF5_HL_LIBRARY:PATH=${HDF5HLLIBPATH} \
	-D CPACK_GENERATOR=${PACKAGE} \
	-D OPENPSTD_VERSION_MAJOR=${OPENPSTD_MAJOR_VERSION} \
	-D OPENPSTD_VERSION_MINOR=${OPENPSTD_MINOR_VERSION} \
	-D OPENPSTD_VERSION_PATCH=${TRAVIS_BUILD_NUMBER} \
	-D OPENPSTD_SYSTEM_NAME=${TARGET} \
	-G Unix\ Makefiles ./

make OpenPSTD-cli
make OpenPSTD-gui
make OpenPSTD-test
sudo cpack -V -G ${PACKAGE}
ls -alh
