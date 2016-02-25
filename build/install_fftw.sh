#!/usr/bin/env bash
set -e

if [ ${TARGET} = "windows" ]; then
    sudo apt-get -y install mxe-${WINTARGET}-fftw
else
    if [ ${TRAVIS_OS_NAME} = "linux" ]; then
        echo running on linux
        echo installing fftw
        sudo apt-get update
        sudo apt-get  --yes --force-yes install libfftw3-dev
    else
        echo running on mac os x
        echo fftw in homebrew is not compiled with float support
        echo compiling fftw from source with --enable-float
        wget http://www.fftw.org/fftw-3.3.4.tar.gz
        tar -xzf fftw-3.3.4.tar.gz
        cd fftw-3.3.4
        ./configure --enable-float --enable-sse --prefix=/usr
        make
        sudo make install
        echo clean up to install double version as well
        make distclean
        ./configure --enable-sse2 --prefix=/usr
        make
        sudo make install
        cd ..
    fi
fi