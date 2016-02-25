#!/usr/bin/env bash
set -e

if [ ${TARGET} = "windows" ]; then
    echo "deb http://pkg.mxe.cc/repos/apt/debian jessie main" | sudo tee /etc/apt/sources.list.d/mxeapt.list
    sudo apt-key adv --keyserver x-hkp://keys.gnupg.net --recv-keys D43A795B73B16ABE9643FE1AFD8FFF16DB45C6AB
    sudo apt-get update
else
    if [ $TRAVIS_OS_NAME = "linux" ]; then
        echo ============================================================================
        sudo add-apt-repository --yes ppa:beineri/opt-qt551-trusty
        echo ============================================================================
        sudo apt-get update -qq
    else
        sudo brew update >/dev/null
    fi
fi