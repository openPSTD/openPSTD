#include "eventhandler.h"

/**
 * Constructor.
 * 
 * @param model  A reference to the Model
 * @param settings  A reference to the Settings instance
 * @param modelmanager  A reference to the ModelManager instance
 */
EventHandler::EventHandler(Model* model, Settings* settings, ModelManager* modelmanager) {
    // Save reference variables locally
    this->model = model;
    this->settings = settings;
    this->modelmanager = modelmanager;
    
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
    // Check if adding a new domain
    if (button == Qt::LeftButton && model->state == ADDDOMAIN) {
        // Check if a domain is already being added
        if (addingDomain) {
            // Finish this domain
            addDomainStop(x, y);
            addingDomain = false;
        } else {
            // Save the old state
            modelmanager->saveState();
            
            // Start adding a new domain
            addDomainStart(x, y);
            addingDomain = true;
        }
    }
    
    // Check if adding a source
    if (button == Qt::LeftButton && model->state == ADDSOURCE) {
        // Save the new state
        modelmanager->saveState();
        
        // Add a new source
        addSource(x, y);
    }
    
    // Check if adding a receiver
    if (button == Qt::LeftButton && model->state == ADDRECEIVER) {
        // Save the new state
        modelmanager->saveState();
        
        // Add a new receiver
        addReceiver(x, y);
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
    // Check if adding a new domain
    if (button == Qt::LeftButton && model->state == ADDDOMAIN) {
        // Check for click and immediate release
        int dx = std::abs(x - model->lastDomain()->getX0() * model->zoom);
        int dy = std::abs(y - model->lastDomain()->getY0() * model->zoom);
        int dist2 = dx * dx + dy * dy;
        if (dist2 > 100 && addingDomain) {
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
    
    // Check if adding a new domain
    if (model->state == ADDDOMAIN && addingDomain) {
        // Update the new domain
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
    QPoint gridPoint = Grid::clampGrid(x, y, model, settings);
    if (gridPoint == QPoint(-1, -1)) {
        px = x;
        py = y;
    } else {
        px = gridPoint.x();
        py = gridPoint.y();
    }
    
    // Create a new Domain instance
    Domain d(
        px / model->zoom,
        py / model->zoom,
        px / model->zoom,
        py / model->zoom
    );
    
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
    // Reset all merged walls to the original four walls per domain
    for (unsigned int i = 0; i < model->domains.size(); i++) {
        model->domains[i].resetWalls();
    }
    
    // Clamp the coordinates to the grid
    int px, py;
    QPoint gridPoint = Grid::clampGrid(x, y, model, settings);
    if (gridPoint == QPoint(-1, -1)) {
        px = x;
        py = y;
    } else {
        px = gridPoint.x();
        py = gridPoint.y();
    }
    
    // Update the end coordinates of the newest model
    model->lastDomain()->setX1(px / model->zoom);
    model->lastDomain()->setY1(py / model->zoom);
    
    // Re-merge all intersecting walls
    for (unsigned int i = 0; i < model->domains.size(); i++) {
        model->domains[i].mergeDomains(&model->domains, i);
    }
}

/**
 * Private event handling method: Adds a new source.
 * 
 * @param x  The x coordinate at which to add the new source
 * @param y  The y coordinate at which to add the new source
 */
void EventHandler::addSource(int x, int y) {
    // Clamp the coordinates to the grid
    int px, py;
    QPoint gridPoint = Grid::clampGrid(x, y, model, settings);
    if (gridPoint == QPoint(-1, -1)) {
        px = x;
        py = y;
    } else {
        px = gridPoint.x();
        py = gridPoint.y();
    }
    
    // Add a new source
    model->sources.push_back(Source(
        px / model->zoom,
        py / model->zoom,
        settings
    ));
}

/**
 * Private event handling method: Adds a new receiver.
 * 
 * @param x  The x coordinate at which to add the new receiver
 * @param y  The y coordinate at which to add the new receiver
 */
void EventHandler::addReceiver(int x, int y) {
    // Clamp the coordinates to the grid
    int px, py;
    QPoint gridPoint = Grid::clampGrid(x, y, model, settings);
    if (gridPoint == QPoint(-1, -1)) {
        px = x;
        py = y;
    } else {
        px = gridPoint.x();
        py = gridPoint.y();
    }
    
    // Add a new receiver
    model->receivers.push_back(Receiver(
        px / model->zoom,
        py / model->zoom,
        settings
    ));
}
