#!/usr/bin/env bash
set -e

if [ $TRAVIS_OS_NAME = "linux" ]; then
    echo ============================================================================
    sudo add-apt-repository --yes ppa:beineri/opt-qt55-trusty
    echo ============================================================================
    sudo apt-get update -qq
else
    sudo brew update >/dev/null
fi

