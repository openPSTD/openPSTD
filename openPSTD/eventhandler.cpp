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
    selecting = false;
}

/**
 * Public event handling method: Mouse press.
 * 
 * @param x  The x coordinate of the mouse
 * @param y  The y coordinate of the mouse
 * @param button  The button that was pressed
 */
void EventHandler::mousePress(int x, int y, Qt::MouseButton button) {
    // Check if selecting objects
    if (button == Qt::LeftButton && model->state == SELECT) {
        // Save the start coordinates
        selectStart.first = x;
        selectStart.second = y;
        selectEnd.first = x;
        selectEnd.second = y;
        
        // Select any objects at the clicked position
        select();
    }
    
    // Check if moving the scene
    if (button == Qt::LeftButton && model->state == MOVE) {
        // Save the start coordinates
        moveSceneX = x;
        moveSceneY = y;
    }
    
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
    // Check if selecting objects
    if (button == Qt::LeftButton && model->state == SELECT) {
        selecting = false;
    }
    
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
 * @param drag  Whether or not the left mouse button is pressed
 * @param modifiers  The KeyboardModifiers of the mouse drag event
 */
void EventHandler::mouseDrag(int x, int y, bool drag, Qt::KeyboardModifiers modifiers) {
    // Update mouse position variables
    mouseX = x;
    mouseY = y;
    
    // Check if selecting objects
    if (model->state == SELECT && drag && modifiers == Qt::CTRL) {
        selectEnd.first = x;
        selectEnd.second = y;
        selecting = true;
        select();
    }
    
    // Check if moving objects
    if (model->state == SELECT && drag && modifiers == Qt::NoModifier) {
        // Compute the position of the mouse
        QPoint newPos = Grid::clampGrid(x, y, model, settings);
        
        // Loop through all selected sources
        for (unsigned int i = 0; i < selectedSources.size(); i++) {
            model->sources[selectedSources[i]].setX(newPos.x() / model->zoom);
            model->sources[selectedSources[i]].setY(newPos.y() / model->zoom);
        }
        
        // Loop through all selected receivers
        for (unsigned int i = 0; i < selectedReceivers.size(); i++) {
            model->receivers[selectedReceivers[i]].setX(newPos.x() / model->zoom);
            model->receivers[selectedReceivers[i]].setY(newPos.y() / model->zoom);
        }
    }
    
    // Check if moving the scene
    if (model->state == MOVE && drag) {
        // Update the scene offset
        model->offsetX -= (moveSceneX - x);
        model->offsetY -= (moveSceneY - y);
        
        // Save the new mouse position
        moveSceneX = x;
        moveSceneY = y;
    }
    
    // Check if adding a new domain
    if (model->state == ADDDOMAIN && addingDomain) {
        // Update the new domain
        addDomainStop(x, y);
    }
}

/**
 * Checks if a source with given index in the sources vector (in model)
 * is selected.
 * 
 * @param i  The index of the source in the sources vector
 * @return  Whether or not the source is selected
 */
bool EventHandler::isSourceSelected(unsigned int i) {
    return std::find(selectedSources.begin(), selectedSources.end(), i) != selectedSources.end();
}

/**
 * Checks if a receiver with given index in the receivers vector (in model)
 * is selected.
 * 
 * @param i  The index of the receiver in the receivers vector
 * @return  Whether or not the receiver is selected
 */
bool EventHandler::isReceiverSelected(unsigned int i) {
    return std::find(selectedReceivers.begin(), selectedReceivers.end(), i) != selectedReceivers.end();
}

/**
 * Checks which of a domain's walls are selected.
 * 
 * @param domainID  The index of the domain in the domains vector
 * @return  A vector of indices in the domain's walls vector of selected walls
 */
std::vector<unsigned int> EventHandler::getSelectedWalls(unsigned int domainID) {
    std::vector<unsigned int> result;
    for (unsigned int i = 0; i < selectedWalls.size(); i++) {
        if (selectedWalls[i].first == domainID) result.push_back(selectedWalls[i].second);
    }
    return result;
}

/**
 * Draws the selection rectangle.
 * 
 * @param pixels  A reference to the QImage to draw on
 */
void EventHandler::drawSelection(QImage* pixels) {
    // Do nothing if not selecting
    if (!selecting) return;
    
    // Get selecting rectangle coordinates
    int x0 = std::min(selectStart.first, selectEnd.first);
    int x1 = std::max(selectStart.first, selectEnd.first);
    int y0 = std::min(selectStart.second, selectEnd.second);
    int y1 = std::max(selectStart.second, selectEnd.second);
    
    // Draw the four edges of the rectangle
    for (int x = x0; x < x1; x++) {
        pixels->setPixel(x, y0, qRgb(100, 100, 100));
        pixels->setPixel(x, y1, qRgb(100, 100, 100));
    }
    for (int y = y0; y < y1; y++) {
        pixels->setPixel(x0, y, qRgb(100, 100, 100));
        pixels->setPixel(x1, y, qRgb(100, 100, 100));
    }
}

/**
 * Deletes all selected objects.
 */
void EventHandler::deleteSelected() {
    // Save the current state (for undo-redo)
    modelmanager->saveState();
    
    // Loop through all domains to be deleted
    for (unsigned int i = 0; i < selectedWalls.size(); i++) {
        // Delete this domain
        int domainID = selectedWalls[i].first;
        model->domains.erase(model->domains.begin() + domainID);
        
        // Update the indices of all other domains to be deleted
        for (unsigned int j = 0; j < selectedWalls.size(); j++) {
            if (selectedWalls[j].first > i) {
                selectedWalls[j].first--;
            }
            if (selectedWalls[j].first == i) {
                selectedWalls.erase(selectedWalls.begin() + j);
                j--;
            }
        }
    }
    
    // Loop through all sources to be deleted
    for (unsigned int i = 0; i < selectedSources.size(); i++) {
        // Delete this source
        int sourceID = selectedSources[i];
        model->sources.erase(model->sources.begin() + sourceID);
        
        // Update the indices of all other domains to be deleted
        for (unsigned int j = 0; j < selectedSources.size(); j++) {
            if (selectedSources[j] > i) {
                selectedSources[j]--;
            }
        }
    }
    
    // Loop through all receivers to be deleted
    for (unsigned int i = 0; i < selectedReceivers.size(); i++) {
        // Delete this receiver
        int receiverID = selectedReceivers[i];
        model->receivers.erase(model->receivers.begin() + receiverID);
        
        // Update the indices of all other domains to be deleted
        for (unsigned int j = 0; j < selectedReceivers.size(); j++) {
            if (selectedReceivers[j] > i) {
                selectedReceivers[j]--;
            }
        }
    }
    
    // Reset selected vectors
    selectedWalls.clear();
    selectedSources.clear();
    selectedReceivers.clear();
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

/**
 * Selects all walls, sources, and receivers within the selection rectangle.
 */
void EventHandler::select() {
    // Get selecting rectangle coordinates
    int x0 = std::min(selectStart.first, selectEnd.first);
    int x1 = std::max(selectStart.first, selectEnd.first);
    int y0 = std::min(selectStart.second, selectEnd.second);
    int y1 = std::max(selectStart.second, selectEnd.second);
    
    // Check for point selection
    if (x0 == x1 && y0 == y1) {
        // Add margin around the selection point
        int d = 3;
        x0 -= d;
        x1 += d;
        y0 -= d;
        y1 += d;
    }
    
    // Clear the previous select results
    selectedWalls.clear();
    selectedSources.clear();
    selectedReceivers.clear();
    
    // Loop through all walls
    for (unsigned int i = 0; i < model->domains.size(); i++) {
        std::vector<Wall*>* walls = model->domains[i].getWalls();
        for (unsigned int j = 0; j < walls->size(); j++) {
            Wall* wall = walls->at(j);
            
            // Get the position of the wall
            int wx0 = std::min(wall->getX0(), wall->getX1());
            int wx1 = std::max(wall->getX0(), wall->getX1());
            int wy0 = std::min(wall->getY0(), wall->getY1());
            int wy1 = std::max(wall->getY0(), wall->getY1());
            Side side = wall->getSide();
            
            // Update coordinates according to zoom level and scene offset
            wx0 = model->zoom * wx0 + model->offsetX;
            wx1 = model->zoom * wx1 + model->offsetX;
            wy0 = model->zoom * wy0 + model->offsetY;
            wy1 = model->zoom * wy1 + model->offsetY;
            
            // Check if this wall is in the selecting rectangle
            if (side == LEFT || side == RIGHT) {
                if (x0 <= wx0 && wx0 <= x1 && y0 <= wy1 && wy0 <= y1) {
                    selectedWalls.push_back(std::make_pair(i, j));
                }
            } else {
                if (y0 <= wy0 && wy0 <= y1 && x0 <= wx1 && wx0 <= x1) {
                    selectedWalls.push_back(std::make_pair(i, j));
                }
            }
        }
    }
    
    // Loop through all sources
    for (unsigned int i = 0; i < model->sources.size(); i++) {
        // Get the position of the source
        int x = model->sources[i].getX();
        int y = model->sources[i].getY();
        
        // Update coordinates according to zoom level and scene offset
        int xx = model->zoom * x + model->offsetX;
        int yy = model->zoom * y + model->offsetY;
        
        // Check if this source is in the selecting rectangle
        if (x0 <= xx && xx <= x1 && y0 <= yy && yy <= y1) {
            // Select this source
            selectedSources.push_back(i);
        }
    }
    
    // Loop through all receivers
    for (unsigned int i = 0; i < model->receivers.size(); i++) {
        // Get the position of the source
        int x = model->receivers[i].getX();
        int y = model->receivers[i].getY();
        
        // Update coordinates according to zoom level and scene offset
        int xx = model->zoom * x + model->offsetX;
        int yy = model->zoom * y + model->offsetY;
        
        // Check if this source is in the selecting rectangle
        if (x0 <= xx && xx <= x1 && y0 <= yy && yy <= y1) {
            // Select this receiver
            selectedReceivers.push_back(i);
        }
    }
}