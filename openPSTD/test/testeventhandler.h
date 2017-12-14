#ifndef TESTEVENTHANDLER_H
#define TESTEVENTHANDLER_H

#include <QtTest/QtTest>
#include "window.h"
#include "eventhandler.h"

class TestEventHandler : public QObject {
    Q_OBJECT
public:
    TestEventHandler(Window* window);
private:
    Window* window;
private slots:
    // Tests for EventHandler::startSelection()
    void testStartSelection();
    
    // Tests for EventHandler::stopSelection()
    void testStopSelection();
    
    // Tests for EventHandler::deleteSelected()
    void testDeleteSelected();
    
    // Tests for EventHandler::clearSelection()
    void testClearSelection();
    
    // Tests for EventHandler::moveObject()
    void testMoveObjectDomain();
    void testMoveObjectWall();
    void testMoveObjectSource();
    void testMoveObjectReceiver();
    
    // Tests for EventHandler::setState()
    void testSetState();
    
    // Tests for EventHandler::setMousePos()
    void testSetMousePos();
};

#endif