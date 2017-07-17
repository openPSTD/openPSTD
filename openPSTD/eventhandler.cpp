#include "eventhandler.h"

/**
 * Constructor.
 * 
 * @param model  A reference to the Model
 */
EventHandler::EventHandler(Model* model) {
    // Save reference to Model locally
    this->model = model;
    
    // Set initial state variable values
    addingDomain = false;
}

/**
 * Public event handling method: Mouse press.
 * 
 * @param x  The x coordinate of the mouse
 * @param y  The y coordinate of the mouse
 * @param button  The button that was pressed
 */
void EventHandler::mousePress(int x, int y, Qt::MouseButton button) {
    // Start adding a new domain
    if (button == Qt::LeftButton && model->state == ADDDOMAIN) {
        // Check if a domain is already being added
        if (addingDomain) {
            // Finish this domain
            addDomainStop(x, y);
            addingDomain = false;
        } else {
            // Start adding a new domain
            addDomainStart(x, y);
            addingDomain = true;
            time(&startAddDomainTime);
        }
    }
}

/**
 * Public event handling method: Mouse release.
 * 
 * @param x  The x coordinate of the mouse
 * @param y  The y coordinate of the mouse
 * @param button  The button that was pressed
 */
void EventHandler::mouseRelease(int x, int y, Qt::MouseButton button) {
    // Finish the new domain
    if (button == Qt::LeftButton && model->state == ADDDOMAIN) {
        // Check for click and immediate release
        time_t t1;
        time(&t1);
        if (difftime(t1, startAddDomainTime) > 0.2 && addingDomain) {
            // Stop the domain here
            addDomainStop(x, y);
            addingDomain = false;
        }
    }
}

/**
 * Public event handling method: Mouse drag.
 * 
 * @param x  The x coordinate of the mouse
 * @param y  The y coordinate of the mouse
 */
void EventHandler::mouseDrag(int x, int y) {
    // Update mouse position variables
    mouseX = x;
    mouseY = y;
    
    // Update the new domain
    if (model->state == ADDDOMAIN && addingDomain) {
        addDomainStop(x, y);
    }
}

/**
 * Private event handling method: Start adding a new domain.
 * 
 * @param x  The x coordinate of the first corner of the domain
 * @param y  The y coordinate of the first corner of the domain
 */
void EventHandler::addDomainStart(int x, int y) {
    // Clamp the coordinates to the grid
    int px, py;
    QPoint gridPoint = model->clampGrid(x, y);
    int dx = gridPoint.x() - x;
    int dy = gridPoint.y() - y;
    int dsqrd = dx * dx + dy * dy;
    if (dsqrd < clampDist * clampDist) {
        px = gridPoint.x();
        py = gridPoint.y();
    } else {
        px = x;
        py = y;
    }
    
    // Create a new Domain instance
    Domain d(px, py, px, py);
    
    // Add the new domain to the model
    model->domains.push_back(d);
}

/**
 * Private event handling method: Update the second corner of
 * the newest domain.
 * 
 * @param x  The x coordinate of the second corner of the domain
 * @param y  The y coordinate of the second corner of the domain
 */
void EventHandler::addDomainStop(int x, int y) {
    // Clamp the coordinates to the grid
    int px, py;
    QPoint gridPoint = model->clampGrid(x, y);
    int dx = gridPoint.x() - x;
    int dy = gridPoint.y() - y;
    int dsqrd = dx * dx + dy * dy;
    if (dsqrd < clampDist * clampDist) {
        px = gridPoint.x();
        py = gridPoint.y();
    } else {
        px = x;
        py = y;
    }
    
    // Update the end coordinates of the newest model
    model->lastDomain()->setX1(px);
    model->lastDomain()->setY1(py);
}