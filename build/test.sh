#!/usr/bin/env bash
set -e

if [ ${TARGET} != "win64" ]; then
    if [ ${TRAVIS_OS_NAME} = "linux" ]; then
        DEPENDENCY_CHECKER=ldd
    else
        DEPENDENCY_CHECKER=otool\ -L
    fi

    echo Dependencies for OpenPSTD-gui
    ${DEPENDENCY_CHECKER} OpenPSTD-gui

    echo Dependencies for OpenPSTD-test
    ${DEPENDENCY_CHECKER} OpenPSTD-test

    ./OpenPSTD-test --log_format=XML --log_sink=results.xml --log_level=all --report_level=no
    cat results.xml
    xsltproc build/report/TestReport.xsl results.xml
fi

