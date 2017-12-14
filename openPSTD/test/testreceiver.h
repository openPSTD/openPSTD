#ifndef TESTRECEIVER_H
#define TESTRECEIVER_H

#include <stdexcept>
#include <QtTest/QtTest>
#include "window.h"
#include "receiver.h"

class TestReceiver : public QObject {
    Q_OBJECT
public:
    TestReceiver(Window* window);
private:
    Window* window;
private slots:
    // Tests for Receiver::draw()
    void testDraw1();
    void testDraw2();
    
    // Tests for Receiver::getPos()
    void testGetPos();
    
    // Tests for Receiver::setPos()
    void testSetPos();
};

#endif