#-D EIGEN_INCLUDE:PATH=/home/travis/build/micfort/openPSTD/rapidjson
#-D Qt5_DIR:PATH=/opt/qt55 \

cmake \
	-D RAPIDJSON_ROOT:PATH=rapidjson \
	-D UNQLITE_INCLUDE:PATH=unqlite \
	-D UNQLITE_LIB:PATH=unqlite/libunqlite.a \
	-D Qt5_DIR:PATH=/opt/qt55 \
	-G Unix\ Makefiles ./
make OpenPSTD-gui-test VERBOSE=1