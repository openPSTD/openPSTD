#!/usr/bin/env bash
set -e

wget http://bitbucket.org/eigen/eigen/get/3.2.6.tar.bz2 -O eigen-v3.2.6.tar.bz2
tar -xjf eigen-v3.2.6.tar.bz2
mv eigen-eigen-c58038c56923 eigen