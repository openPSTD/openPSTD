#!/usr/bin/env bash
set -e

if [ ${TARGET} = "win64" ]; then
    EXTENSION=zip
else
    if [ ${TRAVIS_OS_NAME} = "linux" ]; then
        EXTENSION=tar.gz
    else
        EXTENSION=dmg
    fi
fi

curl -T OpenPSTD-${OPENPSTD_MAJOR_VERSION}.${OPENPSTD_MINOR_VERSION}.${TRAVIS_BUILD_NUMBER}-${TARGET}.${EXTENSION} ${WEBDAV}
