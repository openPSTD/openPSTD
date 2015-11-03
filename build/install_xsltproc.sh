#!/usr/bin/env bash
set -e

if [ $TRAVIS_OS_NAME = "linux" ]; then
    sudo apt-get -y install xsltproc
else
    xsltproc --version
fi
