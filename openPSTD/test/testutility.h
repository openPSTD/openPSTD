#ifndef TESTUTILITY_H
#define TESTUTILITY_H

#include <QtTest/QtTest>
#include "window.h"
#include "utility.h"

class TestUtility : public QObject {
    Q_OBJECT
public:
    TestUtility(Window* window);
private:
    Window* window;
private slots:
    // Tests for Utility::drawPixel()
    void testDrawPixel1();
    void testDrawPixel2();
    
    // Tests for Utility::drawLine()
    void testDrawLine1();
    void testDrawLine2();
    
    // Tests for Utility::drawRect()
    void testDrawRect1();
    void testDrawRect2();
    
    // Tests for Utility::scr2obj()
    void testScr2obj();
    
    // Tests for Utility::obj2scr()
    void testObj2scr();
};

#endif