#!/usr/bin/env bash
./OpenPSTD-gui-test --log_format=XML --log_sink=results.xml --log_level=all --report_level=no
xsltproc build/report/TestReport.xsl results.xml
