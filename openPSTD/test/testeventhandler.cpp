#include "testeventhandler.h"

/**
 * Constructor.
 * 
 * @param window  A reference to the main window
 */
TestEventHandler::TestEventHandler(Window* window) {
    // Save reference to the main window locally
    this->window = window;
}

/**
 * Verifies that EventHandler::startSelection() works correctly.
 */
void TestEventHandler::testStartSelection() {
    // Get a reference to the EventHandler
    EventHandler* eh = window->renderer->eventhandler;
    
    // Call EventHandler::startSelection()
    eh->startSelection(QPoint(5, 10));
    
    // Get the selection rectangle
    QRect selection = eh->getSelection();
    
    // Verify that the selection rectangle is correct
    QVERIFY(selection.top() == 10);
    QVERIFY(selection.bottom() == 10);
    QVERIFY(selection.left() == 5);
    QVERIFY(selection.right() == 5);
}

/**
 * Verifies that EventHandler::stopSelection() works correctly.
 */
void TestEventHandler::testStopSelection() {
    // Get a reference to the EventHandler
    EventHandler* eh = window->renderer->eventhandler;
    
    // Call EventHandler::startSelection()
    eh->startSelection(QPoint(5, 10));
    
    // Call EventHandler::stopSelection()
    eh->stopSelection(QPoint(20, 0));
    
    // Get the selection rectangle
    QRect selection = eh->getSelection();
    
    // Verify that the selection rectangle is correct
    QVERIFY(selection.top() == 0);
    QVERIFY(selection.bottom() == 10);
    QVERIFY(selection.left() == 5);
    QVERIFY(selection.right() == 20);
}

/**
 * Verifies that EventHandler::deleteSelected() works correctly.
 */
void TestEventHandler::testDeleteSelected() {
    // Get a reference to the EventHandler
    EventHandler* eh = window->renderer->eventhandler;
    
    // Add a new source
    Source s(QPoint(10, 10));
    Model::getInstance()->sources.push_back(&s);
    int numSources = Model::getInstance()->sources.size();
    
    // Select the source
    eh->startSelection(QPoint(0, 5));
    eh->stopSelection(QPoint(15, 20));
    
    // Call EventHandler::deleteSelected()
    eh->deleteSelected();
    
    // Verify that the source has been deleted
    QVERIFY(Model::getInstance()->sources.size() == numSources - 1);
}

/**
 * Verifies that EventHandler::clearSelection() works correctly.
 */
void TestEventHandler::testClearSelection() {
    // Get a reference to the EventHandler
    EventHandler* eh = window->renderer->eventhandler;
    
    // Select an area
    eh->startSelection(QPoint(0, 5));
    eh->stopSelection(QPoint(15, 20));
    
    // Clear the selection
    eh->clearSelection();
    
    // Get the selection rectangle
    QRect selection = eh->getSelection();
    
    // Verify that the output is correct
    QVERIFY(selection.top() == 0);
    QVERIFY(selection.bottom() == 0);
    QVERIFY(selection.left() == 0);
    QVERIFY(selection.right() == 0);
}

/**
 * Verifies that EventHandler::moveObject() works correctly on a domain.
 */
void TestEventHandler::testMoveObjectDomain() {
    // Get a reference to the EventHandler
    EventHandler* eh = window->renderer->eventhandler;
    
    // Create a new domain
    Domain domain(QRect(QPoint(5, 5), QPoint(15, 20)));
    Model::getInstance()->domains.push_back(&domain);
    
    // Update the state
    Model::getInstance()->state = SELECTDOMAIN;
    
    // Select the domain
    eh->startSelection(QPoint(0, 0));
    eh->stopSelection(QPoint(10, 10));
    
    // Set the mouse position
    eh->setMousePos(QPoint(5, 10));
    
    // Call EventHandler::moveObject()
    eh->moveObject(QPoint(10, 20));
    
    // Verify that the position of the domain has been updated correctly
    Domain* d = Model::getInstance()->domains.back();
    QRect pos = d->getRect();
    QVERIFY(pos.top() == 10);
    QVERIFY(pos.bottom() == 20);
    QVERIFY(pos.left() == 15);
    QVERIFY(pos.right() == 30);
}

/**
 * Verifies that EventHandler::moveObject() works correctly on a wall.
 */
void TestEventHandler::testMoveObjectWall() {
    // Get a reference to the EventHandler
    EventHandler* eh = window->renderer->eventhandler;
    
    // Create a new domain
    Domain domain(QRect(QPoint(5, 5), QPoint(15, 20)));
    Model::getInstance()->domains.push_back(&domain);
    
    // Update the state
    Model::getInstance()->state = SELECT;
    
    // Select the domain
    eh->startSelection(QPoint(0, 6));
    eh->stopSelection(QPoint(10, 9));
    
    // Set the mouse position
    eh->setMousePos(QPoint(5, 10));
    
    // Call EventHandler::moveObject()
    eh->moveObject(QPoint(10, 20));
    
    // Verify that the position of the wall has been updated correctly
    Domain* d = Model::getInstance()->domains.back();
    QRect pos = d->getRect();
    QVERIFY(pos.top() == 15);
    QVERIFY(pos.bottom() == 30);
    QVERIFY(pos.left() == 10);
    QVERIFY(pos.right() == 15);
}

/**
 * Verifies that EventHandler::moveObject() works correctly on a source.
 */
void TestEventHandler::testMoveObjectSource() {
    // Get a reference to the EventHandler
    EventHandler* eh = window->renderer->eventhandler;
    
    // Create a new source
    Source source(QPoint(5, 5));
    Model::getInstance()->sources.push_back(&source);
    
    // Update the state
    Model::getInstance()->state = SELECT;
    
    // Select the source
    eh->startSelection(QPoint(0, 0));
    eh->stopSelection(QPoint(10, 10));
    
    // Set the mouse position
    eh->setMousePos(QPoint(5, 5));
    
    // Call EventHandler::moveObject()
    eh->moveObject(QPoint(10, 15));
    
    // Verify that the position of the source has been updated correctly
    Source* s = Model::getInstance()->sources.back();
    QPoint pos = s->getPos();
    QVERIFY(pos.x() == 10);
    QVERIFY(pos.y() == 15);
}

/**
 * Verifies that EventHandler::moveObject() works correctly on a receiver.
 */
void TestEventHandler::testMoveObjectReceiver() {
    // Get a reference to the EventHandler
    EventHandler* eh = window->renderer->eventhandler;
    
    // Create a new receiver
    Receiver receiver(QPoint(5, 5));
    Model::getInstance()->receivers.push_back(&receiver);
    
    // Update the state
    Model::getInstance()->state = SELECT;
    
    // Select the receiver
    eh->startSelection(QPoint(0, 0));
    eh->stopSelection(QPoint(10, 10));
    
    // Set the mouse position
    eh->setMousePos(QPoint(5, 5));
    
    // Call EventHandler::moveObject()
    eh->moveObject(QPoint(10, 15));
    
    // Verify that the position of the receiver has been updated correctly
    Receiver* r = Model::getInstance()->receivers.back();
    QPoint pos = r->getPos();
    QVERIFY(pos.x() == 10);
    QVERIFY(pos.y() == 15);
}

/**
 * Verifies that EventHandler::setState() works correctly.
 */
void TestEventHandler::testSetState() {
    // Get a reference to the EventHandler
    EventHandler* eh = window->renderer->eventhandler;
    
    // Call EventHandler::setState()
    eh->setState(MOVESCENE);
    
    // Verify that the state has been updated correctly
    QVERIFY(Model::getInstance()->state == MOVESCENE);
}

/**
 * Verifies that EventHandler::setMousePos() works correctly.
 */
void TestEventHandler::testSetMousePos() {
    // Get a reference to the EventHandler
    EventHandler* eh = window->renderer->eventhandler;
    
    // Call EventHandler::setMousePos()
    eh->setMousePos(QPoint(10, 15));
    
    // Get the mouse position
    QPoint pos = eh->getMousePos();
    
    // Verify that the mouse position has been updated correctly
    QVERIFY(pos.x() == 10);
    QVERIFY(pos.y() == 15);
}