#!/usr/bin/env bash
if [ $TRAVIS_OS_NAME = "linux" ]; then
    sudo add-apt-repository --yes ppa:beineri/opt-qt55-trusty
    sudo apt-get update -qq
else
    sudo brew update
fi

