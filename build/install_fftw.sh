#!/usr/bin/env bash
set -e

if [ ${TARGET} = "win64" ]; then
    sudo apt-get -y install mxe-${WINTARGET}-fftw
else
    if [ ${TRAVIS_OS_NAME} = "linux" ]; then
        echo running on linux
        echo fftw in repo is compiled with --enable-debug,
        echo which breaks thread safety.
        echo compiling fftw from source with --enable-float
        wget http://www.fftw.org/fftw-3.3.5.tar.gz
        tar -xzf fftw-3.3.5.tar.gz
        cd fftw-3.3.5
        ./configure --enable-shared --enable-float --enable-sse --enable-sse2 --enable-avx --enable-avx2 --enable-generic-simd128 CFLAGS="-fPIC" CPPFLAGS="-fPIC" > /dev/null
        make > /dev/null
        sudo make install
    else
        echo running on mac os x
        echo fftw in homebrew is not compiled with float support
        echo compiling fftw from source with --enable-float
        wget http://www.fftw.org/fftw-3.3.5.tar.gz
        tar -xzf fftw-3.3.5.tar.gz
        cd fftw-3.3.5
        ./configure --enable-shared --enable-float --enable-sse --enable-sse2 --enable-avx --enable-avx2 --enable-generic-simd128 CXX="g++ -stdlib=libstdc++ -arch x86_64 -fPIC" > /dev/null
        make > /dev/null
        sudo make install
    fi
fi
