#!/usr/bin/env bash

if [ ${TARGET} = "windows" ]; then
    echo install wine
    sudo apt-get install wine
fi