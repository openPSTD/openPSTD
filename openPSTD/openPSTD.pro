#-------------------------------------------------
#
# Project created by QtCreator 2017-07-07T14:34:55
#
#-------------------------------------------------

QT	   += core gui testlib

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
CONFIG += silent

TARGET = openPSTD
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000	# disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
	main.cpp \
	receiver.cpp \
	renderer.cpp \
	modelmanager.cpp \
	frame.cpp \
	simulator.cpp \
	domain.cpp \
	settings.cpp \
	graphicsview.cpp \
	window.cpp \
	wall.cpp \
	coordinatedialog.cpp \
	absorptiondialog.cpp \
	source.cpp \
	eventhandler.cpp \
	test/testrobustness.cpp \
	test/testadddomain.cpp \
	test/testaddreceiver.cpp \
	test/testaddsource.cpp \
	test/testmaintoolbar.cpp \
	test/testmenuscene.cpp \
	test/testmenuview.cpp \
	test/testmodel.cpp \
	test/testundoredo.cpp

HEADERS += \
	receiver.h \
	frame.h \
	coordinatedialog.h \
	state.h \
	absorptiondialog.h \
	renderer.h \
	grid.h \
	graphicsview.h \
	modelmanager.h \
	renderer.h \
	grid.h \
	graphicsview.h \
	modelmanager.h \
	source.h \
	domain.h \
	wall.h \
	eventhandler.h \
	settings.h \
	mingw.thread.h \
	model.h \
	simulator.h \
	window.h \
    test/testadddomain.h \
    test/testaddreceiver.h \
    test/testaddsource.h \
    test/testmaintoolbar.h \
    test/testmenuscene.h \
    test/testmenuview.h \
    test/testmodel.h \
    test/testrobustness.h \
    test/testundoredo.h

FORMS += \
	window.ui \
	absorptiondialog.ui \
	coordinatedialog.ui

RESOURCES += \
	resources.qrc \

QMAKE_CXXFLAGS += -std=c++0x -pthread
LIBS += -pthread