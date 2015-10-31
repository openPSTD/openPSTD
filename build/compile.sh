CMAKE_OPTIONS=\-D RAPIDJSON_ROOT:PATH=/home/travis/build/micfort/openPSTD/rapidjson
CMAKE_OPTIONS=${CMAKE_OPTIONS} \-D UNQLITE_INCLUDE:PATH=/home/travis/build/micfort/openPSTD/unqlite
CMAKE_OPTIONS=${CMAKE_OPTIONS} \-D UNQLITE_LIB:PATH=/home/travis/build/micfort/openPSTD/unqlite/libunqlite.a
#CMAKE_OPTIONS=${CMAKE_OPTIONS} -D EIGEN_INCLUDE:PATH=/home/travis/build/micfort/openPSTD/rapidjson
CMAKE_OPTIONS=${CMAKE_OPTIONS} \-D Qt5_DIR:PATH=/opt/qt55

cmake ${CMAKE_OPTIONS} -G Unix\ Makefiles --debug-output
make OpenPSTD-gui VERBOSE=1