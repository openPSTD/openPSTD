#!/usr/bin/env bash
set -e

ldd OpenPSTD-gui
ldd OpenPSTD-test

./OpenPSTD-test --log_format=XML --log_sink=results.xml --log_level=all --report_level=no
xsltproc build/report/TestReport.xsl results.xml
