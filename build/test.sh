#!/usr/bin/env bash
set -e

if [ ${TRAVIS_OS_NAME} = "linux" ]; then
    DEPENDENCY_CHECKER=otool -L
else
    DEPENDENCY_CHECKER=ldd
fi

echo Dependencies for OpenPSTD-gui
${DEPENDENCY_CHECKER} OpenPSTD-gui

echo Dependencies for OpenPSTD-test
${DEPENDENCY_CHECKER} OpenPSTD-test

./OpenPSTD-test --log_format=XML --log_sink=results.xml --log_level=all --report_level=no
xsltproc build/report/TestReport.xsl results.xml
