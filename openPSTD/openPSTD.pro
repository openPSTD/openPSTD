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
	absorptiondialog.cpp \
	coordinatedialog.cpp \
	domain.cpp \
	frame.cpp \
	graphicsview.cpp \
	modelmanager.cpp \
	receiver.cpp \
	renderer.cpp \
	settings.cpp \
	simulator.cpp \
	source.cpp \
	wall.cpp \
	window.cpp \
	eventhandler/domainhandler.cpp \
	eventhandler/eventhandler.cpp \
	eventhandler/measurehandler.cpp \
	eventhandler/receiverhandler.cpp \
	eventhandler/sourcehandler.cpp \
    point.cpp \
    simulator2.cpp \
    simulatoroutput.cpp \
    sidebar.cpp \
    simulatorbutton.cpp

HEADERS += \
	absorptiondialog.h \
	coordinatedialog.h \
	domain.h \
	frame.h \
	graphicsview.h \
	grid.h \
	model.h \
	modelmanager.h \
	receiver.h \
	renderer.h \
	settings.h \
	simulator.h \
	source.h \
	wall.h \
	window.h \
	eventhandler/domainhandler.h \
	eventhandler/eventhandler.h \
	eventhandler/measurehandler.h \
	eventhandler/receiverhandler.h \
	eventhandler/sourcehandler.h \
    point.h \
    simulator2.h \
    simulatoroutput.h \
    sidebar.h \
    simulatorbutton.h

FORMS += \
	window.ui \
	absorptiondialog.ui \
	coordinatedialog.ui

RESOURCES += \
	resources.qrc \

QMAKE_CXXFLAGS += -std=c++0x -pthread
LIBS += -pthread