#-D EIGEN_INCLUDE:PATH=/home/travis/build/micfort/openPSTD/rapidjson
#-D Qt5_DIR:PATH=/opt/qt55 \

cmake \
	-D RAPIDJSON_ROOT:PATH=/home/travis/build/micfort/openPSTD/rapidjson \
	-D UNQLITE_INCLUDE:PATH=/home/travis/build/micfort/openPSTD/unqlite \
	-D UNQLITE_LIB:PATH=/home/travis/build/micfort/openPSTD/unqlite/libunqlite.a \
	-D Qt5_DIR:PATH=/opt/qt55 \
	-G Unix\ Makefiles ./
make OpenPSTD-gui VERBOSE=1