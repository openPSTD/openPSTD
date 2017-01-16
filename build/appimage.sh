#!/bin/bash

export ARCH=$(arch)

APP=openPSTD
LOWERAPP=${APP,,}

cd ~
mkdir -p $APP/$APP.AppDir/usr/
cd $APP

wget -q https://github.com/probonopd/AppImages/raw/master/functions.sh
./functions.sh

cd $APP.AppDir

sudo chown -R $USER /app/
cp -r /app/* ./usr/

get_icon
get_apprun
get_desktop

copy_deps

get_desktopintegration $LOWERAPP

GLIBC_NEEDED=$(glibc_needed)
VERSION=${RELEASE_VERSION}-glibc$GLIBC_NEEDED

# Patch away absolute paths 
find usr/ -type f -exec sed -i -e 's|/usr|././|g' {} \;
find usr/ -type f -exec sed -i -e 's@././/bin/env@/usr/bin/env@g' {} \;

cd ..
mkdir -p ../out/
generate_type2_appimage

transfer ../out/*
echo "AppImage has been uploaded to the URL above; later use GitHub Releases for permanent storage"
