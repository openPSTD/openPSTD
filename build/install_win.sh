#!/usr/bin/env bash

if [ ${TARGET} = "windows" ]; then
    echo install wine
    sudo apt-get install wine

    echo install mxe
    git clone https://github.com/mxe/mxe.git
    sudo apt-get install \
        autoconf automake autopoint bash bison bzip2 flex gettext\
        git g++ gperf intltool libffi-dev libgdk-pixbuf2.0-dev \
        libtool libltdl-dev libssl-dev libxml-parser-perl make \
        openssl p7zip-full patch perl pkg-config python ruby scons \
        sed unzip wget xz-utils
    #specific to 64 bit ubuntu
    sudo apt-get install g++-multilib libc6-dev-i386

    cd mxe
    make gcc
    cd ..
fi