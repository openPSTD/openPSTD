#ifndef TESTSOURCE_H
#define TESTSOURCE_H

#include <stdexcept>
#include <QtTest/QtTest>
#include "window.h"
#include "source.h"

class TestSource : public QObject {
    Q_OBJECT
public:
    TestSource(Window* window);
private:
    Window* window;
private slots:
    // Tests for Source::draw()
    void testDraw1();
    void testDraw2();
    
    // Tests for Source::getPos()
    void testGetPos();
    
    // Tests for Source::setPos()
    void testSetPos();
};

#endif