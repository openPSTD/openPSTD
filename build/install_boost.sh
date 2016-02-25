#!/usr/bin/env bash
set -e

if [ ${TARGET} = "windows" ]; then
    cd mxe
    make boost
    cd ..
else
    if [ ${TRAVIS_OS_NAME} = "linux" ]; then
        echo running on linux
        echo installing Boost with source
        wget http://sourceforge.net/projects/boost/files/boost/1.59.0/boost_1_59_0.tar.bz2/download -O boost_1_59_0.tar.bz2
        tar -xjf boost_1_59_0.tar.bz2
        cd boost_1_59_0/
        ./bootstrap.sh --with-libraries=program_options,test,regex
        sudo ./b2 -d0 link=shared install
        cd ../
    else
        echo running on mac os x
        echo installing Boost with homebrew
        sudo brew upgrade boost
    fi
fi