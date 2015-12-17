#!/usr/bin/env bash
set -e

if [ ${TRAVIS_OS_NAME} = "linux" ]; then
    EXTENSION=tar.gz
else
    EXTENSION=dmg
fi

curl -T OpenPSTD-${OPENPSTD_MAJOR_VERSION}.${OPENPSTD_MINOR_VERSION}.${TRAVIS_BUILD_NUMBER}-${TRAVIS_OS_NAME}.${EXTENSION} ${WEBDAV}
