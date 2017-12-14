#ifndef TESTWALL_H
#define TESTWALL_H

#include <QtTest/QtTest>
#include "window.h"
#include "wall.h"

class TestWall : public QObject {
    Q_OBJECT
public:
    TestWall(Window* window);
private:
    Window* window;
private slots:
    // Tests for Wall::draw()
    void testDraw1();
    void testDraw2();
    
    // Tests for Wall::getLine()
    void testGetLine();
    
    // Tests for Wall::getSide()
    void testGetSide();
    
    // Tests for Wall::isHorizontal()
    void testIsHorizontal();
    
    // Tests for Wall::getAbsorption()
    void testGetAbsorption();
    
    // Tests for Wall::setTL()
    //void testSetTL();
    
    // Tests for Wall::setBR()
    //void testSetBR();
    
    // Tests for Wall::setAbsorption()
    void testSetAbsorption();
};

#endif