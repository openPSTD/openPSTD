#!/usr/bin/env bash
set -e

if [ ${TRAVIS_OS_NAME} = "linux" ]; then
    echo running on linux
    echo installing fftw
    sudo apt-get update
    sudo apt-get  --yes --force-yes install libfftw3-dev
else
    echo running on mac os x
    echo installing fftw with homebrew
    sudo brew install fftw
fi
