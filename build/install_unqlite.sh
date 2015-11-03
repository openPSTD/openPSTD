#!/usr/bin/env bash
set -e

cd unqlite
wget http://unqlite.org/db/unqlite-db-116.zip
unzip unqlite-db-116.zip
wget http://unqlite.org/db/g++421patch.diff
patch < g++421patch.diff
cmake -G Unix\ Makefiles
make
cd ..