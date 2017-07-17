#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H

#include "model.h"
#include "state.h"
#include "settings.h"

/**
 * Event handling class.
 * Receives mouse and keyboard events from Renderer,
 * and determines what to do.
 */
class EventHandler {
public:
    // Constructor
    EventHandler(Model* model);
    
    // Public event handling methods
    void mousePress(int x, int y, Qt::MouseButton button);
    void mouseRelease(int x, int y, Qt::MouseButton button);
    void mouseDrag(int x, int y);
    
    // Get method for current mouse position
    QPoint getMousePos() { return QPoint(mouseX, mouseY); }
private:
    // Reference to the Model
    Model* model;
    
    // State variables
    time_t startAddDomainTime;
    bool addingDomain;
    int mouseX;
    int mouseY;
    
    // Private event handling methods
    void addDomainStart(int x, int y);
    void addDomainStop(int x, int y);
};

#endif