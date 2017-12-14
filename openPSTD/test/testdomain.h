#ifndef TESTDOMAIN_H
#define TESTDOMAIN_H

#include <QtTest/QtTest>
#include "window.h"
#include "domain.h"

class TestDomain : public QObject {
    Q_OBJECT
public:
    TestDomain(Window* window);
private:
    Window* window;
private slots:
    // Tests for Domain::draw()
    void testDraw1();
    void testDraw2();
    
    // Tests for Domain::getRect()
    void testGetRect();
    
    // Tests for Domain::getWalls()
    void testGetWalls();
};

#endif