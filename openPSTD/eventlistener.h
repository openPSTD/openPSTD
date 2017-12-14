#ifndef EVENTLISTENER_H
#define EVENTLISTENER_H

#include <iostream>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>
#include "eventhandler.h"

// Action enum
enum Action {
    A_SELECT,
    A_MOVESCENE,
    A_ADDDOMAIN,
    A_ADDSOURCE,
    A_ADDRECEIVER,
    A_MEASURE,
    A_UNDO,
    A_REDO,
    A_CENTERSCENE
};

/**
 * Matches events to a corresponding method in EventHandler.
 */
class EventListener {
public:
    // Constructor, destructor
    EventListener();
    ~EventListener();
    
    // Callback methods for events
    void mousePress(QMouseEvent* event);
    void mouseMove(QMouseEvent* event);
    void mouseRelease(QMouseEvent* event);
    void doubleClick(QMouseEvent* event);
    void mouseWheel(QWheelEvent* event);
    void keyPressed(QKeyEvent* event);
    void action(Action action);
    
    // Class instance variables
    EventHandler* eventhandler;
    
    // State variables
    Action currentaction;
    bool addingDomain;
};

#endif