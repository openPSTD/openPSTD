#!/usr/bin/env bash
set -e

if [ ${TRAVIS_OS_NAME} = "linux" ]; then
    TOOLSET=gcc
    wget http://sourceforge.net/projects/boost/files/boost/1.59.0/boost_1_59_0.tar.bz2/download -O boost_1_59_0.tar.bz2
    tar -xjf boost_1_59_0.tar.bz2
    cd boost_1_59_0/
    ./bootstrap.sh --with-libraries=program_options,test
    sudo ./b2 -d0 link=shared toolset=${TOOLSET} install
    cd ../
else
    TOOLSET=darwin
fi