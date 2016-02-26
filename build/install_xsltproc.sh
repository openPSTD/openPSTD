#!/usr/bin/env bash
set -e

if [ $TRAVIS_OS_NAME = "win64" ]; then
    sudo apt-get -y install xsltproc
else
    xsltproc --version
fi
