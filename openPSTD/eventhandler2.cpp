#include "eventhandler2.h"

/**
 * Constructor.
 * 
 * @param model  A reference to the model
 * @param renderer  A reference to the renderer
 * @param pixels  A reference to the QImage from Renderer
 * @param parent  A reference to the main window
 */
EventHandler2::EventHandler2(Model* model, Renderer* renderer, QImage* pixels, QWidget* parent) {
    // Save references locally
    this->model = model;
    this->renderer = renderer;
    this->pixels = pixels;
    this->parent = parent;
    
    // Initialize state variables
    addingDomain = false;
    selecting = false;
    measuring = false;
    moving = false;
}

/**
 * Deletes all selected domains, sources, and receivers.
 */
void EventHandler2::EventHandler2::deleteSelected() {
    // Loop through all domains
    for (unsigned int i = 0; i < model->domains.size(); i++) {
        // Check if the entire domain is selected
        if (model->domains[i].getSelectedDomain()) {
            // Remove this domain
            model->domains.erase(model->domains.begin() + i);
            i--;
        }
    }
    
    // Loop through all sources
    for (unsigned int i = 0; i < model->sources.size(); i++) {
        // Check if this source is selected
        if (model->sources[i].getSelected()) {
            // Remove this source
            model->sources.erase(model->sources.begin() + i);
            i--;
        }
    }
    
    // Loop through all receivers
    for (unsigned int i = 0; i < model->receivers.size(); i++) {
        // Check if this receiver is selected
        if (model->receivers[i].getSelected()) {
            // Remove this receiver
            model->receivers.erase(model->receivers.begin() + i);
            i--;
        }
    }
}

/**
 * Deselects all domains, sources, and receivers.
 */
void EventHandler2::clearSelection() {
    // Loop through all domains
    for (unsigned int i = 0; i < model->domains.size(); i++) {
        // Deselect this domain
        model->domains[i].setSelectedDomain(false);
        
        // Deselect all walls in this domain
        model->domains[i].setSelectedTop(false);
        model->domains[i].setSelectedBottom(false);
        model->domains[i].setSelectedLeft(false);
        model->domains[i].setSelectedRight(false);
    }
    
    // Loop through all sources
    for (unsigned int i = 0; i < model->sources.size(); i++) {
        // Deselect this source
        model->sources[i].setSelected(false);
    }
    
    // Loop through all receivers
    for (unsigned int i = 0; i < model->receivers.size(); i++) {
        // Deselect this receiver
        model->receivers[i].setSelected(false);
    }
}

/**
 * Cancels adding a new domain.
 */
void EventHandler2::cancelNewDomain() {
    // Do nothing if not adding a domain
    if (!addingDomain) return;
    
    // Remove the last domain from model
    model->domains.pop_back();
    
    // Update the state variables
    addingDomain = false;
}

void EventHandler2::changeAbsorptionDialog() {
    // Verify that exactly one domain is selected
    int domainID = -1;
    for (unsigned int i = 0; i < model->domains.size(); i++) {
        if (model->domains[i].getSelectedDomain()) {
            if (domainID != -1) return;
            domainID = i;
        }
    }
    if (domainID == -1) return;
    
    // Verify that no walls are selected
    for (unsigned int i = 0; i < model->domains.size(); i++) {
        if (model->domains[i].getSelectedTop()) return;
        if (model->domains[i].getSelectedBottom()) return;
        if (model->domains[i].getSelectedLeft()) return;
        if (model->domains[i].getSelectedRight()) return;
    }
    
    // Verify that no sources are selected
    for (unsigned int i = 0; i < model->sources.size(); i++) {
        if (model->sources[i].getSelected()) return;
    }
    
    // Verify that no receivers are selected
    for (unsigned int i = 0; i < model->receivers.size(); i++) {
        if (model->receivers[i].getSelected()) return;
    }
    
    // Open an AbsorptionDialog for the selected domain
    AbsorptionDialog ad(
        parent,
        model->domains[domainID].getAbsorption(TOP),
        model->domains[domainID].getAbsorption(BOTTOM),
        model->domains[domainID].getAbsorption(LEFT),
        model->domains[domainID].getAbsorption(RIGHT)
    );
    ad.exec();
    model->domains[domainID].setAbsorption(TOP, ad.top);
    model->domains[domainID].setAbsorption(BOTTOM, ad.bottom);
    model->domains[domainID].setAbsorption(LEFT, ad.left);
    model->domains[domainID].setAbsorption(RIGHT, ad.right);
    model->domains[domainID].updateAbsorption();
}

/**
 * Centers the scene on all objects such that the zoom level
 * is maximized while keeping all objects visible in the scene.
 */
void EventHandler2::moveToCenter() {
    // Go to (0, 0) if there are no domains
    if (model->domains.size() == 0) {
        model->offsetX = pixels->width() / 2;
        model->offsetY = pixels->height() / 2;
        return;
    }
    
    // Keep track of the minimum and maximum x and y coordinates
    int minx = model->domains[0].getX0();
    int maxx = model->domains[0].getX1();
    int miny = model->domains[0].getY0();
    int maxy = model->domains[0].getY1();
    
    // Loop through all domains
    for (unsigned int i = 1; i < model->domains.size(); i++) {
        // Update min and max according to this domain
        Domain d = model->domains[i];
        minx = std::min(minx, d.getX0());
        maxx = std::max(maxx, d.getX1());
        miny = std::min(miny, d.getY0());
        maxy = std::max(maxy, d.getY1());
    }
    
    // Loop through all sources
    for (unsigned int i = 0; i < model->sources.size(); i++) {
        // Update min and max according to this source
        Source s = model->sources[i];
        minx = std::min(minx, s.getX());
        maxx = std::max(maxx, s.getX());
        miny = std::min(miny, s.getY());
        maxy = std::max(maxy, s.getY());
    }
    
    // Loop through all receivers
    for (unsigned int i = 0; i < model->receivers.size(); i++) {
        // Update min and max according to this receiver
        Receiver s = model->receivers[i];
        minx = std::min(minx, s.getX());
        maxx = std::max(maxx, s.getX());
        miny = std::min(miny, s.getY());
        maxy = std::max(miny, s.getY());
    }
    
    // Compute the maximum zoom level
    int maxzoomx = pixels->width() / (1.1 * (maxx-minx));
    int maxzoomy = pixels->height() / (1.1 * (maxy-miny));
    int maxzoom = (maxzoomx < maxzoomy ? maxzoomx : maxzoomy);
    
    // Compute the new offset
    int offsetx = -maxzoom * minx;
    int offsety = -maxzoom * miny;
    int doffsetx = (pixels->width() - (maxx-minx) * maxzoom) / 2;
    int doffsety = (pixels->height() - (maxy-miny) * maxzoom) / 2;
    
    // Save the new zoom level and offset
    model->zoom = maxzoom;
    model->offsetX = offsetx + doffsetx;
    model->offsetY = offsety + doffsety;
}

/**
 * Adds a new domain at a given position.
 * 
 * @param pos  The screen coordinates of the mouse
 * @param stop  True iff the domain needs to be finalized here
 */
void EventHandler2::addDomain(QPoint pos, bool stop) {
    // Check if starting a new domain, or updating
    if (!addingDomain && stop) {
        // Convert the screen coordinates to object coordinates
        // and clamp to the grid and other domains
        QPoint p = Grid::clampFull(pos.x(), pos.y(), model, true) / model->zoom;
        
        // Add a new domain with these coordinates
        model->domains.push_back(Domain(
            p.x(),
            -p.y(),
            p.x(),
            -p.y()
        ));
        
        // Update the state variables
        addingDomain = true;
    } else if (addingDomain) {
        // Convert the screen coordinates to object coordinates
        // and clamp to the grid and other domains
        QPoint p = Grid::clampFull(pos.x(), pos.y(), model, false) / model->zoom;
        
        // Update the last domain in model
        model->domains.back().setX1(p.x());
        model->domains.back().setY1(-p.y());
        
        // Merge all domains' walls
        for (unsigned int i = 0; i < model->domains.size(); i++) {
            model->domains[i].resetWalls();
        }
        for (unsigned int i = 0; i < model->domains.size(); i++) {
            model->domains[i].mergeDomains(&model->domains, i);
        }
        
        // Update the state variables
        if (stop) addingDomain = false;
    }
}

/**
 * Adds a new source to the scene by prompting
 * for its coordinates.
 */
void EventHandler2::addSource() {
    // Open a CoordinateDialog to prompt the coordinates
    CoordinateDialog cd(parent);
    cd.exec();
    
    // Add a new source with the entered coordinates
    model->sources.push_back(Source(cd.x, cd.y));
}

/**
 * Adds a new receiver to the scene by prompting
 * for its coordinates.
 */
void EventHandler2::addReceiver() {
    // Open a CoordinateDialog to prompt the coordinates
    CoordinateDialog cd(parent);
    cd.exec();
    
    // Add a new receiver with the entered coordinates
    model->receivers.push_back(Receiver(cd.x, cd.y));
}

/**
 * Starts a selection rectangle at the mouse position.
 * 
 * @param pos  The position of the mouse
 */
void EventHandler2::selectStart(QPoint pos) {
    // Convert the screen coordinates to object coordinates
    QPoint p = QPoint(
        pos.x() - model->offsetX,
        -pos.y() + model->offsetY
    ) / model->zoom;
    
    // Update the state variables
    selecting = true;
    selectstart = p;
    
    // Update Renderer
    renderer->setSelectionRectangle(QRect(p, p));
    renderer->setSelectionEnabled(true);
}

/**
 * Updates the selection rectangle at the mouse position.
 * 
 * @param pos  The position of the mouse
 * @param stop  Whether or not to stop the selection here
 */
void EventHandler2::selectStop(QPoint pos, bool stop) {
    // Do nothing if not selecting
    if (!selecting) return;
    
    // Convert the screen coordinates to object coordinates
    QPoint p = QPoint(
        pos.x() - model->offsetX,
        -pos.y() + model->offsetY
    ) / model->zoom;
    
    // Compute min and max
    int minx = std::min(selectstart.x(), p.x());
    int maxx = std::max(selectstart.x(), p.x());
    int miny = std::min(selectstart.y(), p.y());
    int maxy = std::max(selectstart.y(), p.y());
    
    // Update min and max if the selection is a single point
    if (selectstart.x() == p.x() && selectstart.y() == p.y()) {
        minx--;
        maxx++;
        miny--;
        maxy++;
    }
    
    // Loop through all sources
    for (unsigned int i = 0; i < model->sources.size(); i++) {
        // Check if this source was clicked
        bool x = minx <= model->sources[i].getX() && model->sources[i].getX() <= maxx;
        bool y = miny <= model->sources[i].getY() && model->sources[i].getY() <= maxy;
        model->sources[i].setSelected(x && y);
    }
    
    // Loop through all receivers
    for (unsigned int i = 0; i < model->receivers.size(); i++) {
        // Check if this receiver was clicked
        bool x = minx <= model->receivers[i].getX() && model->receivers[i].getX() <= maxx;
        bool y = miny <= model->receivers[i].getY() && model->receivers[i].getY() <= maxy;
        model->receivers[i].setSelected(x && y);
    }
    
    // Update the state variables
    if (stop) selecting = false;
    
    // Update Renderer
    renderer->setSelectionRectangle(QRect(selectstart, p));
    renderer->setSelectionEnabled(!stop);
}

/**
 * Updates the selection rectangle at the mouse position.
 * 
 * @param pos  The position of the mouse
 * @param stop  Whether or not to stop the selection here
 */
void EventHandler2::selectDomainStop(QPoint pos, bool stop) {
    // Do nothing if not selecting
    if (!selecting) return;
    
    // Convert the screen coordinates to object coordinates
    QPoint p = QPoint(
        pos.x() - model->offsetX,
        -pos.y() + model->offsetY
    ) / model->zoom;
    
    // Compute min and max
    int minx = std::min(selectstart.x(), p.x());
    int maxx = std::max(selectstart.x(), p.x());
    int miny = std::min(selectstart.y(), p.y());
    int maxy = std::max(selectstart.y(), p.y());
    
    // Update min and max if the selection is a single point
    if (selectstart.x() == p.x() && selectstart.y() == p.y()) {
        minx--;
        maxx++;
        miny--;
        maxy++;
    }
    
    // Loop through all domains
    for (unsigned int i = 0; i < model->domains.size(); i++) {
        // Check if the mouse is inside this domain
        int x0 = model->domains[i].getX0();
        int x1 = model->domains[i].getX1();
        int y0 = model->domains[i].getY0();
        int y1 = model->domains[i].getY1();
        bool x = x0 <= maxx && minx <= x1;
        bool y = y0 <= maxy && miny <= y1;
        model->domains[i].setSelectedDomain(x && y);
    }
    
    // Update the state variables
    if (stop) selecting = false;
    
    // Update Renderer
    renderer->setSelectionRectangle(QRect(selectstart, p));
    renderer->setSelectionEnabled(!stop);
}

/**
 * Updates the selection rectangle at the mouse position.
 * 
 * @param pos  The position of the mouse
 * @param stop  Whether or not to stop the selection here
 */
void EventHandler2::selectWallStop(QPoint pos, bool stop) {
    // Do nothing if not selecting
    if (!selecting) return;
    
    // Convert the screen coordinates to object coordinates
    QPoint p = QPoint(
        pos.x() - model->offsetX,
        -pos.y() + model->offsetY
    ) / model->zoom;
    
    // Compute min and max
    int minx = std::min(selectstart.x(), p.x());
    int maxx = std::max(selectstart.x(), p.x());
    int miny = std::min(selectstart.y(), p.y());
    int maxy = std::max(selectstart.y(), p.y());
    
    // Update min and max if the selection is a single point
    if (selectstart.x() == p.x() && selectstart.y() == p.y()) {
        minx--;
        maxx++;
        miny--;
        maxy++;
    }
    
    // Loop through all domains
    for (unsigned int i = 0; i < model->domains.size(); i++) {
        // Get the coordinates of this domain
        int x0 = model->domains[i].getX0();
        int x1 = model->domains[i].getX1();
        int y0 = model->domains[i].getY0();
        int y1 = model->domains[i].getY1();
        
        // Check if the top wall is selected
        bool top = x0 <= maxx && minx <= x1 && miny <= y0 && y0 <= maxy;
        model->domains[i].setSelectedTop(top);
        
        // Check if the bottom wall is selected
        bool bottom = x0 <= maxx && minx <= x1 && miny <= y1 && y1 <= maxy;
        model->domains[i].setSelectedBottom(bottom);
        
        // Check if the left wall is selected
        bool left = minx <= x0 && x0 <= maxx && y0 <= maxy && miny <= y1;
        model->domains[i].setSelectedLeft(left);
        
        // Check if the right wall is selected
        bool right = minx <= x1 && x1 <= maxx && y0 <= maxy && miny <= y1;
        model->domains[i].setSelectedRight(right);
    }
    
    // Update the state variables
    if (stop) selecting = false;
    
    // Update Renderer
    renderer->setSelectionRectangle(QRect(selectstart, p));
    renderer->setSelectionEnabled(!stop);
}

/**
 * Measures the distance between two points (call this function
 * once with the start position, and once or more with the
 * updated end position).
 * 
 * @param pos  The position of the mouse
 * @param stop  Whether or not to stop measuring
 */
void EventHandler2::measure(QPoint pos, bool stop) {
    // Check if starting measuring, or updating
    if (!measuring && stop) {
        // Convert the screen coordinates to object coordinates
        // and clamp to the grid and other domains
        QPoint p = Grid::clampFull(pos.x(), pos.y(), model, true) / model->zoom;
        
        // Update the state variables
        measureLine = QLine(p, p);
        measuring = true;
    } else if (measuring) {
        // Convert the screen coordinates to object coordinates
        // and clamp to the grid and other domains
        QPoint p = Grid::clampFull(pos.x(), pos.y(), model, true) / model->zoom;
        
        // Update the state variables
        measureLine.setP2(p);
        if (stop) measuring = false;
        
        // Update renderer
        renderer->setMeasure(measureLine);
        renderer->setMeasureShown(true);
    }
}

/**
 * Removes the measuring tool.
 */
void EventHandler2::removeMeasure() {
    // Update the state variables
    measuring = false;
    
    // Update renderer
    renderer->setMeasureShown(false);
}

/**
 * Starts moving selected sources and receivers
 */
void EventHandler2::moveStart() {
    // Update the state variables
    moving = true;
    moveDelta = QPoint(0, 0);
}

/**
 * Moves the selected sources and receivers.
 * 
 * @param delta  The delta distance to move (in screen coordinates)
 * @param stop  Whether or not to stop moving
 */
void EventHandler2::moveStop(QPoint delta, bool stop) {
    // Do nothing if not moving
    if (!moving) return;
    
    // Update the state variables
    moving = !stop;
    
    // Add delta to moveDelta
    moveDelta += delta;
    
    // Compute the object coordinates unit value
    int unit = model->zoom;
    
    // Compute how many units the objects can be moved
    int dx = moveDelta.x() / unit;
    int dy = moveDelta.y() / unit;
    
    // Loop through all sources
    for (unsigned int i = 0; i < model->sources.size(); i++) {
        // Do nothing if this source is not selected
        if (!model->sources[i].getSelected()) continue;
        
        // Update the position of this source
        model->sources[i].setX(model->sources[i].getX() + dx);
        model->sources[i].setY(model->sources[i].getY() - dy);
    }
    
    // Loop through all receivers
    for (unsigned int i = 0; i < model->receivers.size(); i++) {
        // Do nothing if this receiver is not selected
        if (!model->receivers[i].getSelected()) continue;
        
        // Update the position of this receiver
        model->receivers[i].setX(model->receivers[i].getX() + dx);
        model->receivers[i].setY(model->receivers[i].getY() - dy);
    }
    
    // Update moveDelta
    moveDelta.setX(moveDelta.x() - unit * dx);
    moveDelta.setY(moveDelta.y() - unit * dy);
}

/**
 * Prompts for new coordinates for source or receiver under the mouse.
 * 
 * @param pos  The position of the mouse
 */
void EventHandler2::moveCoordinates(QPoint pos) {
    // Convert pos to object coordinates
    QPoint p(
        (pos.x() - model->offsetX) / model->zoom,
        -(pos.y() - model->offsetY) / model->zoom
    );
    
    // Loop through all sources
    int id = -1;
    bool isSource;
    QPoint orig;
    for (unsigned int i = 0; i < model->sources.size(); i++) {
        // Get the coordinates of this source
        int x = model->sources[i].getX();
        int y = model->sources[i].getY();
        
        // Do nothing if this source was not clicked
        int dx = std::abs(x - p.x());
        int dy = std::abs(y - p.y());
        if (dx < 2 && dy < 2) {
            if (id != -1) return;
            id = i;
            isSource = true;
            orig = QPoint(x, y);
        }
    }
    
    // Loop through all receivers
    for (unsigned int i = 0; i < model->receivers.size(); i++) {
        // Get the coordinates of this receiver
        int x = model->receivers[i].getX();
        int y = model->receivers[i].getY();
        
        // Do nothing if this receiver was not clicked
        int dx = std::abs(x - p.x());
        int dy = std::abs(y - p.y());
        if (dx < 2 && dy < 2) {
            if (id != -1) return;
            id = i;
            isSource = false;
            orig = QPoint(x, y);
        }
    }
    if (id == -1) return;
    
    // Open a CoordinateDialog to prompt the coordinates
    CoordinateDialog cd(parent, orig);
    cd.exec();
    
    // Save the new coordinates
    if (isSource) {
        model->sources[id].setX(cd.x);
        model->sources[id].setY(cd.y);
    } else {
        model->receivers[id].setX(cd.x);
        model->receivers[id].setY(cd.y);
    }
}