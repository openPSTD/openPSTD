#!/usr/bin/env bash
set -e

if [ ${TARGET} = "windows" ]; then
    sudo apt-get -y install mxe-${WINTARGET}-qtbase
else
if [ $TRAVIS_OS_NAME = "linux" ]; then
    echo running on linux
    echo installing Qt with apt-get
    sudo apt-get -y install qt553d qt55canvas3d
    sudo apt-get -y install qtcreator-latest
    #sudo apt-get -y install qt5-default
else
    echo running on mac os x
    echo installing Qt with homebrew
    sudo brew install qt5
fi
fi