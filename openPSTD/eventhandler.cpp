#include "eventhandler.h"

/**
 * Constructor.
 */
EventHandler::EventHandler() {
    // Initialize state variables
    mousePos = QPoint(0, 0);
    selection = QRect(0, 0, 0, 0);
    selecting = false;
    measure = QLine(0, 0, 0, 0);
    measuring = false;
}

/**
 * Starts adding a new domain at a given position.
 * 
 * @param pos  The first corner of the new domain
 */
void EventHandler::startDomain(QPoint pos) {
    // Create a new Domain instance
    Domain* domain = new Domain(QRect(pos, pos));
    
    // Add this domain to the model
    Model::getInstance()->domains.push_back(domain);
}

/**
 * Updates the second corner of the new domain.
 * 
 * @param pos  The new second corner for the domain
 */
void EventHandler::stopDomain(QPoint pos) {
    // Update the second corner of the new domain
    Domain* domain = Model::getInstance()->domains.back();
    domain->setP2(pos);
}

/**
 * Starts selecting objects at a given position.
 * 
 * @param pos  The start position of the selection
 */
void EventHandler::startSelection(QPoint pos) {
    // Save the start position of the selection
    selection = QRect(pos, pos);
    selecting = true;
}

/**
 * Updates the end position of the selection rectangle.
 * 
 * @param pos  The end position of the selection
 */
void EventHandler::stopSelection(QPoint pos) {
    // Update the selection rectangle's coordinates
    int top = std::min(selection.top(), pos.y());
    int bottom = std::max(selection.bottom(), pos.y());
    int left = std::min(selection.left(), pos.x());
    int right = std::max(selection.right(), pos.x());
    selection = QRect(left, top, right - left, bottom - top);
    
    // Get a reference to the current Model instance
    Model* model = Model::getInstance();
    
    // Clear the selectedObjects vectors
    selectedDomains.clear();
    selectedWalls.clear();
    selectedSources.clear();
    selectedReceivers.clear();
    
    // Search for domains in the selection rectangle
    for (unsigned int i = 0; i < model->domains.size(); i++) {
        // Get the QRect of this domain
        QRect rect = model->domains[i]->getRect();
        
        // Check if the domain overlaps the selection rectangle
        if (rect.intersects(selection)) {
            // Select this domain
            selectedDomains.push_back(i);
        }
    }
    
    // Search for walls in the selection rectangle
    for (unsigned int i = 0; i < model->domains.size(); i++) {
        for (unsigned int j = 0; j < model->domains[i]->getWalls()->size(); j++) {
            Wall wall = model->domains[i]->getWalls()->at(j);
            
            // Get the QLine of this wall
            QLine* line = wall.getLine();
            
            // Check if the wall intersects the selection rectangle
            QRect wallrect(
                line->x1(),
                line->y1(),
                line->x2() - line->x1(),
                line->y2() - line->y1()
            );
            if (wallrect.intersects(selection)) {
                // Select this wall
                selectedWalls.push_back(std::make_pair(i, j));
            }
        }
    }
    
    // Search for sources in the selection rectangle
    for (unsigned int i = 0; i < model->sources.size(); i++) {
        // Get the QPoint of this source
        QPoint pos = model->sources[i]->getPos();
        
        // Check if the source is inside the selection rectangle
        QRect sourcerect(pos, pos);
        if (sourcerect.intersects(selection)) {
            // Select this source
            selectedSources.push_back(i);
        }
    }
    
    // Search for receivers in the selection rectangle
    for (unsigned int i = 0; i < model->receivers.size(); i++) {
        // Get the QPoint of this receiver
        QPoint pos = model->receivers[i]->getPos();
        
        // Check if the receiver is inside the selection rectangle
        QRect receiverrect(pos, pos);
        if (receiverrect.intersects(selection)) {
            // Select this receiver
            selectedReceivers.push_back(i);
        }
    }
}

/**
 * Deletes all selected objects.
 */
void EventHandler::deleteSelected() {
    // Get a reference to the current model instance
    Model* model = Model::getInstance();
    
    // Loop through all domains to be deleted
    for (unsigned int i = 0; i < selectedDomains.size(); i++) {
        // Delete this domain
        int id = selectedDomains[i];
        model->domains.erase(model->domains.begin() + id);
        
        // Update the selectedDomains vector
        for (unsigned int j = i + 1; j < selectedDomains.size(); j++) {
            if (selectedDomains[j] >= id) {
                selectedDomains[j]--;
            }
        }
        
        // Update the selectedWalls vector
        for (unsigned int j = 0; j < selectedWalls.size(); j++) {
            if (selectedWalls[j].first >= id) {
                selectedWalls[j].first--;
            }
        }
    }
    
    // Loop through all sources
    for (unsigned int i = 0; i < selectedSources.size(); i++) {
        // Delete this source
        int id = selectedSources[i];
        model->sources.erase(model->sources.begin() + id);
        
        // Update the selectedSources vector
        for (unsigned int j = i + 1; j < selectedSources.size(); j++) {
            if (selectedSources[j] >= id) {
                selectedSources[j]--;
            }
        }
    }
    
    // Loop through all receivers
    for (unsigned int i = 0; i < selectedReceivers.size(); i++) {
        // Delete this receiver
        int id = selectedReceivers[i];
        model->receivers.erase(model->receivers.begin() + id);
        
        // Update the selectedReceivers vector
        for (unsigned int j = i + 1; j < selectedReceivers.size(); j++) {
            if (selectedReceivers[j] >= id) {
                selectedReceivers[j]--;
            }
        }
    }
}

/**
 * Clears the selection rectangle.
 */
void EventHandler::clearSelection() {
    // Clear the selection rectangle
    selectedDomains.clear();
    selectedWalls.clear();
    selectedSources.clear();
    selectedReceivers.clear();
}

/**
 * Moves the selected objects to the given position.
 * 
 * @param pos  The new position for the objects
 */
void EventHandler::moveObject(QPoint pos) {}

/**
 * Opens the edit properties popup for the clicked object.
 * 
 * @param pos  The position of the object to edit
 */
void EventHandler::editProperties(QPoint pos) {}

/**
 * Updates the state in the model to the given state.
 * 
 * @param state  The new state
 */
void EventHandler::setState(State state) {}

/**
 * Returns the current position of the mouse.
 * 
 * @return  The current position of the mouse
 */
QPoint EventHandler::getMousePos() {}

/**
 * Returns the current selection rectangle.
 * 
 * @return  The current selection rectangle
 */
QRect EventHandler::getSelection() {}

/**
 * Returns the current measure line.
 * 
 * @return  The current measure line
 */
QLine EventHandler::getMeasure() {}

/**
 * Set method for the position of the mouse.
 * 
 * @param pos  The new position of the mouse
 */
void EventHandler::setMousePos(QPoint pos) {}

/**
 * Moves a given domain to a given position.
 * 
 * @param domain  The domain to move
 * @param pos  The new position of the domain, relative to mousePos
 */
void EventHandler::moveDomain(Domain* domain, QPoint pos) {}

/**
 * Moves a given wall to a given position.
 * 
 * @param wall  The wall to move
 * @param pos  The new position of the wall, relative to mousePos
 */
void EventHandler::moveWall(Wall* wall, QPoint pos) {}

/**
 * Moves a given source to a given position.
 * 
 * @param source  The source to move
 * @param pos  The new position of the source
 */
void EventHandler::moveSource(Source* source, QPoint pos) {}

/**
 * Moves a give receiver to a given position.
 * 
 * @param receiver  The receiver to move
 * @param pos  The new position of the receiver
 */
void EventHandler::moveReceiver(Receiver* receiver, QPoint pos) {}