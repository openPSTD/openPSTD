#!/bin/bash

cd /home/jeroen/programming/openPSTD/
export PATH="/home/jeroen/programming/openPSTD/mxe/usr/bin/:$PATH"
./mxe/user/i686-w64-mingw32.static/qt5/bin/qmake openPSTD/openPSTD.pro
make