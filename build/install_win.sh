#!/usr/bin/env bash

if [ ${TARGET} = "win64" ]; then
    echo install wine
    sudo apt-get install wine
fi