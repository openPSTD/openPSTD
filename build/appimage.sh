#!/bin/bash
if [ $TRAVIS_OS_NAME = "linux" ]; then

export ARCH=$(arch)

APP=openPSTD
LOWERAPP=${APP,,}

cd ~
mkdir -p $APP/$APP.AppDir/usr/
cd $APP

wget -q https://github.com/probonopd/AppImages/raw/master/functions.sh
sudo chmod +x functions.sh
./functions.sh

cd $APP.AppDir

sudo chown -R $USER /app/
cp -r /app/* ./usr/
BINARY=./usr/bin/OpenPSTD-gui

get_icon
get_apprun
get_desktop

copy_deps

# Delete dangerous libraries; see
# https://github.com/probonopd/AppImages/blob/master/excludelist
delete_blacklisted

get_desktopintegration $LOWERAPP

GLIBC_NEEDED=$(glibc_needed)
VERSION=${RELEASE_VERSION}-glibc$GLIBC_NEEDED
echo $VERSION > ../VERSION

# Patch away absolute paths 
find usr/ -type f -exec sed -i -e 's|/usr|././|g' {} \;
find usr/ -type f -exec sed -i -e 's@././/bin/env@/usr/bin/env@g' {} \;

cd ..
mkdir -p ../out/
generate_type2_appimage

# Upload the AppDir 
transfer ../out/*
echo "AppImage has been uploaded to the URL above; later use GitHub Releases for permanent storage"

fi #}if linux
