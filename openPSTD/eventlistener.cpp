#include "eventlistener.h"

EventListener::EventListener() {
    currentaction = A_SELECT;
    addingDomain = false;
}

EventListener::~EventListener() {}

void EventListener::mousePress(QMouseEvent* event) {
    // Get the position of the mouse
    QPoint pos = event->pos();
    
    // Check if adding a new domain
    if (currentaction == A_ADDDOMAIN) {
        // Start adding a new domain
        addingDomain = true;
        eventhandler->startDomain(pos);
        std::cout << "Starting a new domain" << std::endl;
    }
}

void EventListener::mouseMove(QMouseEvent* event) {
    // Get the position of the mouse
    QPoint pos = event->pos();
    
    // Check if adding a new domain
    if (currentaction == A_ADDDOMAIN && addingDomain) {
        // Update the new domain
        eventhandler->stopDomain(pos);
    }
}

void EventListener::mouseRelease(QMouseEvent* event) {
    // Get the position of the mouse
    QPoint pos = event->pos();
    
    // Check if adding a new domain
    if (currentaction == A_ADDDOMAIN && addingDomain) {
        // Update the new domain
        addingDomain = false;
        eventhandler->stopDomain(pos);
        std::cout << "Finishing new domain" << std::endl;
    }
}

void EventListener::doubleClick(QMouseEvent* event) {}
void EventListener::mouseWheel(QWheelEvent* event) {}
void EventListener::keyPressed(QKeyEvent* event) {}

void EventListener::action(Action act) {
    std::cout << "action: " << act << std::endl;
    currentaction = act;
}