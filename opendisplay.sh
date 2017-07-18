#!/bin/bash

# Ref: https://docs.travis-ci.com/user/gui-and-headless-browsers
export DISPLAY=:99.0
sh -e /etc/init.d/xvfb start
sleep 3