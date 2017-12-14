#include "domain.h"

/**
 * Constructor.
 * 
 * @param rect  A QRect representing the location of the domain
 */
Domain::Domain(QRect rect) {
    // Initialize representation variables
    this->corner1 = rect.topLeft();
    this->corner2 = rect.bottomRight();
    
    // Calculate the initial walls
    recalcWalls();
}

/**
 * Draws the domain to the given pixels array.
 * 
 * @param pixels  The pixels array to draw to
 * @throws runtime_error  If pixels is a nullptr
 */
void Domain::draw(QImage* pixels) {
    // Do nothing if pixels is a nullptr
    if (pixels == nullptr) {
        throw new std::runtime_error("Given pixels array reference is a nullptr.");
    }
    
    // Draw all walls
    for (unsigned int i = 0; i < walls.size(); i++) {
        walls[i].draw(pixels);
    }
}

/**
 * Get method for the QRect of the domain.
 * 
 * @return  The QRect of the domain
 */
QRect Domain::getRect() {
    // Return a QRect representing the domain
    return QRect(corner1, corner2);
}

/**
 * Get method for the walls vector of the domain.
 * 
 * @return  A pointer to the walls vector of the domain
 */
std::vector<Wall>* Domain::getWalls() {
    // Return a pointer to the walls vector
    return &walls;
}

/**
 * Set method for the second corner of the domain.
 * 
 * @param pos  The new second corner for the domain
 */
void Domain::setP2(QPoint pos) {
    // Save the new position for the second corner
    this->corner2 = pos;
}

/**
 * Recalculates all walls in the domain.
 */
void Domain::recalcWalls() {
    // Clear the walls vector
    walls.clear();
    
    // Create new walls for the four sides
    createWall(TOP);
    createWall(BOTTOM);
    createWall(LEFT);
    createWall(RIGHT);
}

/**
 * Creates a fully merged wall.
 * 
 * @param side  The side of the wall to create
 */
void Domain::createWall(Side side) {
    // Get a reference to the current Model instance
    Model* model = Model::getInstance();
    
    // Loop through all domains in Model
    for (unsigned int i = 0; i < model->domains.size(); i++) {
        // Get the QRect of this wall
        QRect rect = model->domains[i]->getRect();
        
        // Do nothing if domains[i] is this domain
        if (rect == getRect()) continue;
        
        // TODO: Intersect this->rect with rect
    }
}