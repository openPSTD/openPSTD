#include "eventhandler.h"

/**
 * Constructor.
 * 
 * @param model  A reference to the Model
 * @param settings  A reference to the Settings instance
 * @param modelmanager  A reference to the ModelManager instance
 * @param parent  A reference to the main window
 */
EventHandler::EventHandler(Model* model, Settings* settings, ModelManager* modelmanager, QWidget* parent, QAction* changeabsorption) {
    // Save reference variables locally
    this->model = model;
    this->settings = settings;
    this->modelmanager = modelmanager;
    this->parent = parent;
    this->changeabsorption = changeabsorption;
    
    // Set initial state variable values
    addingDomain = false;
    selecting = false;
    measuring = false;
    overlap = false;
    zerowall = false;
}

/**
 * Public event handling method: Mouse press.
 * 
 * @param x  The x coordinate of the mouse
 * @param y  The y coordinate of the mouse
 * @param button  The button that was pressed
 * @param modifiers  The KeyboardModifiers of the mouse press event
 */
void EventHandler::mousePress(int x, int y, Qt::MouseButton button, Qt::KeyboardModifiers modifiers) {
    // Remove the measuring tool
    measuring = false;
    
    // Check if selecting domains
    if (button == Qt::LeftButton && model->state == SELECTDOMAIN) {
        // Save the start coordinates
        selectStart.first = x;
        selectStart.second = y;
        selectEnd.first = x;
        selectEnd.second = y;
        
        // Select the domain at the clicked position
        selectDomains(modifiers == Qt::CTRL, true);
    }
    
    // Check if selecting objects
    if (button == Qt::LeftButton && model->state == SELECT) {
        // Save the start coordinates
        selectStart.first = x;
        selectStart.second = y;
        selectEnd.first = x;
        selectEnd.second = y;
        
        // Select any objects at the clicked position
        select(modifiers == Qt::CTRL, true);
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
            if (!overlap && !zerowall) {
                addDomainStop(x, y);
                addingDomain = false;
            }
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
    
    // Check if measuring
    if (button == Qt::LeftButton && model->state == MEASURE) {
        // Save the start coordinates
        bool clamped;
        QPoint p = Grid::clampGrid(x, y, model, settings, &clamped);
        measureStart.first = p.x();
        measureStart.second = p.y();
        measureEnd.first = p.x();
        measureEnd.second = p.y();
        measuring = true;
    }
    
    // Check if a domain is selected
    changeabsorption->setEnabled(selectedWalls.size() > 0);
}

/**
 * Public event handling method: Mouse release.
 * 
 * @param x  The x coordinate of the mouse
 * @param y  The y coordinate of the mouse
 * @param button  The button that was pressed
 */
void EventHandler::mouseRelease(int x, int y, Qt::MouseButton button) {
    // Check if selecting domains
    if (button == Qt::LeftButton && model->state == SELECTDOMAIN) {
        selecting = false;
    }
    
    // Check if selecting objects
    if (button == Qt::LeftButton && model->state == SELECT) {
        selecting = false;
    }
    
    // Check if adding a new domain
    if (button == Qt::LeftButton && model->state == ADDDOMAIN) {
        // Check for click and immediate release
        if (!addingDomain) return;
        int dx = std::abs(x - model->lastDomain()->getX0() * model->zoom);
        int dy = std::abs(y - model->lastDomain()->getY0() * model->zoom);
        int dist2 = dx * dx + dy * dy;
        if (dist2 > 100 && !overlap && !zerowall) {
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
    
    // Check if selecting domains
    if (model->state == SELECTDOMAIN && drag && modifiers == Qt::CTRL) {
        selectEnd.first = x;
        selectEnd.second = y;
        selecting = true;
        selectDomains(modifiers == Qt::CTRL, false);
    }
    
    // Check if selecting objects
    if (model->state == SELECT && drag && modifiers == Qt::CTRL) {
        selectEnd.first = x;
        selectEnd.second = y;
        selecting = true;
        select(modifiers == Qt::CTRL, false);
    }
    
    // Check if moving objects
    if ((model->state == SELECT || model->state == SELECTDOMAIN) && drag && modifiers == Qt::NoModifier) {
        // Compute the position of the mouse
        bool clamped;
        QPoint newPos = Grid::clampGrid(x, y, model, settings, &clamped);
        
        // Loop through all selected walls
        for (unsigned int i = 0; i < selectedWalls.size(); i++) {
            // Get the domainID and wallID to move
            int domainID = selectedWalls[i].first;
            int wallID = selectedWalls[i].second;
            
            // Check if the wall is horizontal or vertical
            std::vector<Wall*>* walls = model->domains[domainID].getWalls();
            Wall* wall = walls->at(wallID);
            if (wall->getSide() == LEFT || wall->getSide() == RIGHT) {
                // Get the current position of the wall
                int x = wall->getX0();
                
                // Update the corner coordinates of the wall's domain
                if (model->domains[domainID].getTrueX0() == x) {
                    model->domains[domainID].setX0(newPos.x() / model->zoom);
                }
                if (model->domains[domainID].getTrueX1() == x) {
                    model->domains[domainID].setX1(newPos.x() / model->zoom);
                }
                
                // Reset and re-merge all walls
                for (unsigned int j = 0; j < model->domains.size(); j++) {
                    model->domains[j].resetWalls();
                }
                for (unsigned int j = 0; j < model->domains.size(); j++) {
                    model->domains[j].mergeDomains(&model->domains, j);
                }
            } else {
                // Get the current position of the wall
                int y = wall->getY0();
                
                // Update the corner coordinates of the wall's domain
                if (model->domains[domainID].getTrueY0() == y) {
                    model->domains[domainID].setY0(newPos.y() / model->zoom);
                }
                if (model->domains[domainID].getTrueY1() == y) {
                    model->domains[domainID].setY1(newPos.y() / model->zoom);
                }
                
                // Reset and re-merge all walls
                for (unsigned int j = 0; j < model->domains.size(); j++) {
                    model->domains[j].resetWalls();
                }
                for (unsigned int j = 0; j < model->domains.size(); j++) {
                    model->domains[j].mergeDomains(&model->domains, j);
                }
            }
        }
        
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
        
        // Update the wall text states
        updateWallTextState();
    }
    
    // Check if moving an entire domain
    if (drag && ((model->state == SELECT && modifiers == Qt::SHIFT) || model->state == SELECTDOMAIN)) {
        // Compute the position of the mouse
        bool clamped;
        QPoint newPos = Grid::clampGrid(x, y, model, settings, &clamped);
        
        // Loop through all selected walls
        std::vector<int> selectedDomains;
        for (unsigned int i = 0; i < selectedWalls.size(); i++) {
            // Get the domainID to move
            int domainID = selectedWalls[i].first;
            
            // Add this to selectedDomains
            if (std::find(selectedDomains.begin(), selectedDomains.end(), domainID) == selectedDomains.end()) {
                selectedDomains.push_back(domainID);
            }
        }
        
        // Loop through all selected domains
        for (unsigned int i = 0; i < selectedDomains.size(); i++) {
            // Get a reference to the domain to move
            Domain* domain = &model->domains[selectedDomains[i]];
            
            // Compute the width and height of the domain
            int width = domain->getX1() - domain->getX0();
            int height = domain->getY1() - domain->getY0();
            
            // Set the top-left corner of the domain to be the mouse position
            domain->setX0(newPos.x() / model->zoom);
            domain->setY0(newPos.y() / model->zoom);
            domain->setX1(newPos.x() / model->zoom + width);
            domain->setY1(newPos.y() / model->zoom + height);
        }
        
        // Reset and re-merge all walls
        for (unsigned int j = 0; j < model->domains.size(); j++) {
            model->domains[j].resetWalls();
        }
        for (unsigned int j = 0; j < model->domains.size(); j++) {
            model->domains[j].mergeDomains(&model->domains, j);
        }
        
        // Update the wall text states
        updateWallTextState();
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
    
    // Check if measuring
    if (model->state == MEASURE && drag) {
        bool clamped;
        QPoint p = Grid::clampGrid(x, y, model, settings, &clamped);
        measureEnd.first = p.x();
        measureEnd.second = p.y();
    }
}

/**
 * Public event handling method: Mouse double click.
 * 
 * @param x  The x coordinate of the mouse
 * @param y  The y coordinate of the mouse
 * @param button  The button that was pressed
 */
void EventHandler::doubleClick(int x, int y, Qt::MouseButton button) {
    // Check if selecting an object
    if (model->state == SELECT && button == Qt::LeftButton) {
        // Check if double clicking on a wall
        if (selectedWalls.size() == 1) {
            // Check which wall to edit
            int domainID = selectedWalls[0].first;
            int wallID = selectedWalls[0].second;
            Wall* wall = model->domains[domainID].getWalls()->at(wallID);
            
            // Use a QInputDialog to request a new absorption coefficient
            double absorption = QInputDialog::getDouble(
                parent,
                "Change absorption coefficient",
                "Absorption coefficient",
                wall->getAbsorption(),
                0,
                1,
                3
            );
            
            // Save the new absorption coefficient
            wall->setAbsorption(absorption);
            
            // Unselect the wall
            clearSelection();
        }
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
 * Draws the measure tool.
 * 
 * @param pixels  A reference to the QImage to draw on
 */
void EventHandler::drawMeasure(QImage* pixels) {
    // Do nothing if not measuring
    if (!measuring) return;
    
    // Get the measure line coordinates
    int x0 = measureStart.first  + model->offsetX;
    int x1 = measureEnd.first    + model->offsetX;
    int y0 = measureStart.second + model->offsetY;
    int y1 = measureEnd.second   + model->offsetY;
    
    // Loop through all points on the line
    for (int t = 0; t < 1000; t++) {
        double tt = (double) t / 1000;
        
        int x = tt * x0 + (1 - tt) * x1;
        int y = tt * y0 + (1 - tt) * y1;
        
        pixels->setPixel(x, y, qRgb(0, 255, 255));
    }
    
    // Compute the length of the measure
    int minx = x0 / model->zoom;
    int maxx = x1 / model->zoom;
    int miny = y0 / model->zoom;
    int maxy = y1 / model->zoom;
    double length = std::sqrt((maxx-minx)*(maxx-minx) + (maxy-miny)*(maxy-miny));
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(1) << length;
    std::string lengthtext = oss.str();
    
    // Compute the midpoint of the measure
    int midx = (x0 + x1) / 2;
    int midy = (y0 + y1) / 2;
    
    // Draw the measure length text
    drawText(
        lengthtext,
        midx,
        midy,
        14,
        qRgb(0, 255, 255),
        pixels
    );
}

/**
 * Draws the overlap error message (if there is overlap between domains).
 * 
 * @param pixels  A reference to the QImage to draw on
 */
void EventHandler::drawOverlap(QImage* pixels) {
    // Do nothing if there is no overlap
    if (!overlap) return;
    
    // Draw the overlap message
    int x = pixels->width() / 2;
    drawText(
        "Cannot add a domain here.",
        x - 25*10/2,
        7,
        12,
        qRgb(255, 0, 0),
        pixels
    );
    drawText(
        "There is overlap with another domain.",
        x - 37*10/2,
        21,
        12,
        qRgb(255, 0, 0),
        pixels
    );
}

void EventHandler::drawZerowall(QImage* pixels) {
    // Do nothing if there is no zero wall
    if (!zerowall) return;
    
    // Do nothing if the overlap message is already shown
    if (overlap) return;
    
    // Draw the zero wall message
    int x = pixels->width() / 2;
    drawText(
        "Cannot add a domain here.",
        x - 25*10/2,
        7,
        12,
        qRgb(255, 0, 0),
        pixels
    );
    drawText(
        "The width or height of the domain is zero.",
        x - 42*10/2,
        21,
        12,
        qRgb(255, 0, 0),
        pixels
    );
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
    
    // Update all domain's walls
    for (unsigned int i = 0; i < model->domains.size(); i++) {
        model->domains[i].resetWalls();
    }
    for (unsigned int i = 0; i < model->domains.size(); i++) {
        model->domains[i].mergeDomains(&model->domains, i);
    }
}

/**
 * Unselects all selected objects.
 */
void EventHandler::clearSelection() {
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
    // Clamp the coordinates
    QPoint p = Grid::clampFull(x, y, model, settings, true);
    
    // Create a new Domain instance
    Domain d(
        p.x() / model->zoom,
        p.y() / model->zoom,
        p.x() / model->zoom,
        p.y() / model->zoom,
        settings
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
    
    // Clamp the coordinates
    QPoint p = Grid::clampFull(x, y, model, settings, false);
    
    // Update the end coordinates of the newest model
    model->lastDomain()->setX1(p.x() / model->zoom);
    model->lastDomain()->setY1(p.y() / model->zoom);
    
    // Re-merge all intersecting walls
    for (unsigned int i = 0; i < model->domains.size(); i++) {
        model->domains[i].mergeDomains(&model->domains, i);
    }
    
    // Verify that there are no overlapping domains
    overlap = false;
    for (unsigned int i = 0; i < model->domains.size(); i++) {
        for (unsigned int j = i + 1; j < model->domains.size(); j++) {
            // Get the two domains to check
            Domain* one = &(model->domains[i]);
            Domain* two = &(model->domains[j]);
            
            // Check whether or not these two domains overlap
            if (one->overlaps(two)) overlap = true;
        }
    }
    
    // Verify that the domain width and height are not zero
    zerowall = false;
    for (unsigned int i = 0; i < model->domains.size(); i++) {
        Domain domain = model->domains[i];
        int width = domain.getX1() - domain.getX0();
        int height = domain.getY1() - domain.getY0();
        if (width * height == 0) zerowall = true;
    }
    
    // Update the wall text states
    updateWallTextState();
}

/**
 * Private event handling method: Adds a new source.
 * 
 * @param x  The x coordinate at which to add the new source
 * @param y  The y coordinate at which to add the new source
 */
void EventHandler::addSource(int x, int y) {
    // Clamp the coordinates to the grid
    bool clamped;
    QPoint gridPoint = Grid::clampGrid(x, y, model, settings, &clamped);
    
    // Add a new source
    model->sources.push_back(Source(
        gridPoint.x() / model->zoom,
        gridPoint.y() / model->zoom,
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
    bool clamped;
    QPoint gridPoint = Grid::clampGrid(x, y, model, settings, &clamped);
    
    // Add a new receiver
    model->receivers.push_back(Receiver(
        gridPoint.x() / model->zoom,
        gridPoint.y() / model->zoom,
        settings
    ));
}

/**
 * Selects all walls, sources, and receivers within the selection rectangle.
 * 
 * @param ctrl  Whether or not the ctrl key is pressed
 * @param deselect  Whether or not deselecting objects should be possible
 */
void EventHandler::select(bool ctrl, bool deselect) {
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
    if (!ctrl) {
        selectedWalls.clear();
        selectedSources.clear();
        selectedReceivers.clear();
    }
    
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
                    // Check if this wall needs to be deselected
                    bool deselected = false;
                    if (ctrl && deselect) {
                        for (unsigned int k = 0; k < selectedWalls.size(); k++) {
                            if (selectedWalls[k].first == i && selectedWalls[k].second == j) {
                                selectedWalls.erase(selectedWalls.begin() + k);
                                deselected = true;
                                break;
                            }
                        }
                    }
                    if (!deselected) {
                        selectedWalls.push_back(std::make_pair(i, j));
                    }
                }
            } else {
                if (y0 <= wy0 && wy0 <= y1 && x0 <= wx1 && wx0 <= x1) {
                    bool deselected = false;
                    if (ctrl && deselect) {
                        for (unsigned int k = 0; k < selectedWalls.size(); k++) {
                            if (selectedWalls[k].first == i && selectedWalls[k].second == j) {
                                selectedWalls.erase(selectedWalls.begin() + k);
                                deselected = true;
                                break;
                            }
                        }
                    }
                    if (!deselected) {
                        selectedWalls.push_back(std::make_pair(i, j));
                    }
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
            bool deselected = false;
            if (ctrl && deselect) {
                for (unsigned int j = 0; j < selectedSources.size(); j++) {
                    if (selectedSources[j] == i) {
                        selectedSources.erase(selectedSources.begin() + j);
                        deselected = true;
                        break;
                    }
                }
            }
            if (!deselected && std::find(selectedSources.begin(), selectedSources.end(), i) == selectedSources.end()) {
                selectedSources.push_back(i);
            }
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
            bool deselected = false;
            if (ctrl && deselect) {
                for (unsigned int j = 0; j < selectedReceivers.size(); j++) {
                    if (selectedReceivers[j] == i) {
                        selectedReceivers.erase(selectedReceivers.begin() + j);
                        deselected = true;
                        break;
                    }
                }
            }
            if (!deselected && std::find(selectedReceivers.begin(), selectedReceivers.end(), i) == selectedReceivers.end()) {
                selectedReceivers.push_back(i);
            }
        }
    }
}

/**
 * Selects all domains within the selection rectangle.
 * 
 * @param ctrl  Whether or not the ctrl key is pressed
 * @param deselect  Whether or not deselecting domains should be possible
 */
void EventHandler::selectDomains(bool ctrl, bool deselect) {
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
    if (!ctrl) {
        selectedWalls.clear();
        selectedSources.clear();
        selectedReceivers.clear();
    }
    
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
                    // Check if this wall needs to be deselected
                    /*bool deselected = false;
                    if (ctrl && deselect) {
                        for (unsigned int k = 0; k < selectedWalls.size(); k++) {
                            if (selectedWalls[k].first == i) {
                                selectedWalls.erase(selectedWalls.begin() + k);
                                deselected = true;
                                break;
                            }
                        }
                    }
                    if (!deselected) {*/
                        // Add all walls of this domain
                        for (unsigned int k = 0; k < walls->size(); k++) {
                            selectedWalls.push_back(std::make_pair(i, k));
                        }
                        
                        // Go to the next domain
                        break;
                    //}
                }
            } else {
                if (y0 <= wy0 && wy0 <= y1 && x0 <= wx1 && wx0 <= x1) {
                    /*bool deselected = false;
                    if (ctrl && deselect) {
                        for (unsigned int k = 0; k < selectedWalls.size(); k++) {
                            if (selectedWalls[k].first == i) {
                                selectedWalls.erase(selectedWalls.begin() + k);
                                deselected = true;
                                break;
                            }
                        }
                    }
                    if (!deselected) {*/
                        // Add all walls of this domain
                        for (unsigned int k = 0; k < walls->size(); j++) {
                            selectedWalls.push_back(std::make_pair(i, k));
                        }
                        
                        // Go to the next domain
                        break;
                    //}
                }
            }
        }
    }
}

/**
 * Draws a string to the given pixels array.
 * 
 * @param text  The text to draw
 * @param x  The x position to draw at
 * @param y  The y position to draw at
 * @param size  The font size to draw with
 * @param color  The color to draw in
 * @param pixels  The pixels array to draw to
 */
void EventHandler::drawText(std::string text, int x, int y, int size, QRgb color, QImage* pixels) {
    QPainter p;
    p.begin(pixels);
    p.setPen(QPen(color));
    p.setFont(QFont("Monospace", size));
    p.drawText(x, y + size, QString(text.c_str()));
    p.end();
}

/**
 * Updates the drawWallLength state of all walls, setting them to false if
 * and only if that wall length text would overlap with some other wall
 * length text.
 */
void EventHandler::updateWallTextState() {
    // Reset all walls to draw the wall length text
    std::vector<Domain> domains = model->domains;
    for (unsigned int i = 0; i < domains.size(); i++) {
        std::vector<Wall*>* w = domains[i].getWalls();
        for (unsigned int j = 0; j < w->size(); j++) {
            w->at(j)->setDrawWallLength(true);
        }
    }
    
    // Loop through all pairs of walls
    for (unsigned int i = 0; i < domains.size(); i++) {
        std::vector<Wall*>* ws1 = domains[i].getWalls();
        for (unsigned int j = 0; j < ws1->size(); j++) {
            Wall* w1 = ws1->at(j);
            for (unsigned int k = i + 1; k < domains.size(); k++) {
                std::vector<Wall*>* ws2 = domains[k].getWalls();
                for (unsigned int l = 0; l < ws2->size(); l++) {
                    Wall* w2 = ws2->at(l);
                    
                    // Compute the midpoints of the walls
                    int mx1 = (w1->getX0() + w1->getX1()) / 2;
                    int mx2 = (w2->getX0() + w2->getX1()) / 2;
                    int my1 = (w1->getY0() + w1->getY1()) / 2;
                    int my2 = (w2->getY0() + w2->getY1()) / 2;
                    
                    // Compute the distance between the midpoints
                    int dx = std::abs(mx1 - mx2);
                    int dy = std::abs(my1 - my2);
                    
                    // Check if the midpoints are too close to eachother
                    if (dx < 5*11/model->zoom && dy < 2*18/model->zoom) {
                        // Do not draw either of the walls' length texts
                        w1->setDrawWallLength(false);
                        w2->setDrawWallLength(false);
                    }
                }
            }
        }
    }
}

void EventHandler::changeabsorptiondialog() {
    AbsorptionDialog* ad = new AbsorptionDialog(
        parent,
        selectedWalls[0].first,
        model
    );
    ad->exec();
}