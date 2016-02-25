#!/usr/bin/env bash
set -e

#if [ ${TRAVIS_OS_NAME} = "linux" ]; then
#    DEPENDENCY_CHECKER=ldd
#else
#    DEPENDENCY_CHECKER=otool\ -L
#fi

#echo Dependencies for OpenPSTD-gui
#${DEPENDENCY_CHECKER} OpenPSTD-gui

#echo Dependencies for OpenPSTD-test
#${DEPENDENCY_CHECKER} OpenPSTD-test

if [ ${TARGET} = "windows" ]; then
    wine OpenPSTD-test.exe --log_format=XML --log_sink=results.xml --log_level=all --report_level=no
else
    ./OpenPSTD-test --log_format=XML --log_sink=results.xml --log_level=all --report_level=no
fi
xsltproc build/report/TestReport.xsl results.xml
