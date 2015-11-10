#!/usr/bin/env bash
set -e

if [ ${TRAVIS_OS_NAME} = "linux" ]; then
    DEPENDENCY_CHECKER=ldd

    echo Dependencies for OpenPSTD-gui
    ${DEPENDENCY_CHECKER} OpenPSTD-gui

    echo Dependencies for OpenPSTD-test
    ${DEPENDENCY_CHECKER} OpenPSTD-test
else
    DEPENDENCY_CHECKER=otool -L
    #this should also work, but otool doesn't recognize -L, while it should
fi



./OpenPSTD-test --log_format=XML --log_sink=results.xml --log_level=all --report_level=no
xsltproc build/report/TestReport.xsl results.xml
