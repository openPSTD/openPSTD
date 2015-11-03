#!/usr/bin/env bash
if [ $TRAVIS_OS_NAME = "linux" ]; then
    sudo apt-get -y install xsltproc
else
    xsltproc -h
fi
