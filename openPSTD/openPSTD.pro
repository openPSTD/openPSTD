#-------------------------------------------------
#
# Project created by QtCreator 2017-07-07T14:34:55
#
#-------------------------------------------------

QT       += core gui testlib

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

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
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp \
        window.cpp \
    graphicsview.cpp \
    test/testgrid.cpp \
    renderer.cpp \
    model.cpp \
    settings.cpp \
    eventhandler.cpp \
    eventlistener.cpp \
    grid.cpp \
    domain.cpp \
    wall.cpp \
    source.cpp \
    test/testsource.cpp \
    utility.cpp \
    test/testutility.cpp \
    test/testreceiver.cpp \
    receiver.cpp \
    test/testwall.cpp \
    modelmanager.cpp \
    test/testmodelmanager.cpp \
    test/testdomain.cpp \
    test/testeventhandler.cpp

HEADERS += \
        window.h \
    graphicsview.h \
    eventlistener.h \
    eventhandler.h \
    modelmanager.h \
    model.h \
    domain.h \
    source.h \
    receiver.h \
    wall.h \
    renderer.h \
    settings.h \
    grid.h \
    test/testgrid.h \
    test/testsource.h \
    utility.h \
    test/testutility.h \
    test/testreceiver.h \
    test/testwall.h \
    test/testmodelmanager.h \
    test/testdomain.h \
    test/testeventhandler.h

FORMS += \
        window.ui

RESOURCES += \
    resources.qrc
