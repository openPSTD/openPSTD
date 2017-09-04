#include "domain.h"

/**
 * Constructor.
 * 
 * @param x0  The x coordinate of the first corner of the domain
 * @param y0  The y coordinate of the first corner of the domain
 * @param x1  The x coordinate of the second corner of the domain
 * @param y1  The y coordinate of the second corner of the domain
 */
Domain::Domain(int x0, int y0, int x1, int y1) {
    // Save the domain's corner coordinates
    this->x0 = x0;
    this->y0 = y0;
    this->x1 = x1;
    this->y1 = y1;
}

/**
 * Drawing method.
 * Draws the domain to a given QImage
 * 
 * @param pixels  A reference te the QImage to draw to
 * @param zoom  The current zoom level (as in model)
 * @param offsetX  The current x offset of the scene
 * @param offsetY  The current y offset of the scene
 */
void Domain::draw(QImage* pixels, int zoom, int offsetX, int offsetY) {
    // Update the sides of all walls
    updateWallSides();
    
    // Draw all walls
    for (unsigned int i = 0; i < walls.size(); i++) {
        walls[i]->draw(pixels, zoom, offsetX, offsetY);
    }
}

/**
 * Get method for the x coordinate of the left wall.
 * 
 * @return The x coordinate of the left wall
 */
int Domain::getX0() {
    return std::min(x0, x1);
}

/**
 * Get method for the y coordinate of the top wall.
 * 
 * @return The y coordinate of the top wall
 */
int Domain::getY0() {
    return std::min(y0, y1);
}

/**
 * Get method for the x coordinate of the right wall.
 * 
 * @return The x coordinate of the right wall
 */
int Domain::getX1() {
    return std::max(x0, x1);
}

/**
 * Get method for the y coordinate of the bottom wall.
 * 
 * @return The y coordinate of the bottom wall
 */
int Domain::getY1() {
    return std::max(y0, y1);
}

/**
 * Set method for the x coordinate of the right wall.
 * 
 * @param x1  The x coordinate to give to the right wall
 */
void Domain::setX1(int x1) {
    this->x1 = x1;
}

/**
 * Set method for the y coordinate of the bottom wall.
 * 
 * @param y1  The y coordinate to give to the bottom wall
 */
void Domain::setY1(int y1) {
    this->y1 = y1;
}

/**
 * Checks if any of the walls in this domain need to be merged with any of
 * the walls in any other domain, and performs the merge if needed.
 * 
 * @param domains  A pointer to the domains vector
 * @param ownID  The index in the domains vector of this domain
 */
void Domain::mergeDomains(std::vector<Domain>* domains, unsigned int ownID) {
    // Loop through all walls in this domain
    for (unsigned int i = 0; i < walls.size(); i++) {
        // Loop through all domains
        for (unsigned int j = 0; j < domains->size(); j++) {
            // Do not merge with ourselves
            if (j == ownID) continue;
            
            // Loop through all walls in the domain
            for (unsigned int k = 0; k < domains->at(j).getWalls()->size(); k++) {
                Wall* wallk = domains->at(j).getWalls()->at(k);
                
                // Merge with our wall
                std::pair<int, int> toMerge;
                bool merge = Wall::mergeWalls(*(walls[i]), *wallk, &toMerge);
                if (merge) {
                    // The walls intersect, update both walls according to the intersection
                    handleIntersection(this, i, toMerge);
                    handleIntersection(&(domains->at(j)), k, toMerge);
                }
            }
        }
    }
}

/**
 * Resets the domain's walls to be the original four non-merged walls.
 */
void Domain::resetWalls() {
    // Remove all old walls from the walls vector
    for (unsigned int i = 0; i < walls.size(); i++) {
        delete walls[i];
    }
    walls.clear();
    
    // Recreate the original four non-merged walls
    walls.push_back(new Wall(x0, y0, x0, y1, LEFT));
    walls.push_back(new Wall(x1, y0, x1, y1, RIGHT));
    walls.push_back(new Wall(x0, y0, x1, y0, TOP));
    walls.push_back(new Wall(x0, y1, x1, y1, BOTTOM));
}

/**
 * Updates the side of all walls.
 */
void Domain::updateWallSides() {
    // Compute the minimum and maximum x and y coordinates among all walls
    int minx = -1;
    int maxx = -1;
    int miny = -1;
    int maxy = -1;
    for (unsigned int i = 0; i < walls.size(); i++) {
        int x0 = walls[i]->getX0();
        int x1 = walls[i]->getX1();
        int y0 = walls[i]->getY0();
        int y1 = walls[i]->getY1();
        
        if (minx == -1 || std::min(x0, x1) < minx) minx = std::min(x0, x1);
        if (maxx == -1 || std::max(x0, x1) > maxx) maxx = std::max(x0, x1);
        if (miny == -1 || std::min(y0, y1) < miny) miny = std::min(y0, y1);
        if (maxy == -1 || std::max(y0, y1) > maxy) maxy = std::max(y0, y1);
    }
    
    // Compute the midpoint of this domain
    double midx = (double) (minx + maxx) / 2;
    double midy = (double) (miny + maxy) / 2;
    
    // Loop through all walls
    for (unsigned int i = 0; i < walls.size(); i++) {
        // Get the corner coordinates of this wall
        int x0 = walls[i]->getX0();
        int x1 = walls[i]->getX1();
        int y0 = walls[i]->getY0();
        int y1 = walls[i]->getY1();
        
        // Check if this is a vertical wall
        if (x0 == x1 && y0 != y1) {
            // Set side according to position relative to the midpoint
            if (x0 < midx) walls[i]->setSide(LEFT);
            if (x0 > midx) walls[i]->setSide(RIGHT);
        }
        
        // Check if this is a horizontal wall
        if (x0 != x1 && y0 == y1) {
            // Set side according to position relative to the midpoint
            if (y0 < midy) walls[i]->setSide(TOP);
            if (y0 > midy) walls[i]->setSide(BOTTOM);
        }
    }
}

/**
 * Updates a given wall according to a given intersecting segment.
 * The algorithm implemented in this method is documented
 * in docs/Wall intersection.docx.
 * 
 * @param parent  The parent domain of the wall to update
 * @param wallID  The index of the wall in the walls vector of the parent
 * @param toMerge  The segment to merge
 */
void Domain::handleIntersection(Domain* parent, int wallID, std::pair<int, int> toMerge) {
    // Check if the wall is horizontal or vertical
    Wall* wall = parent->getWalls()->at(wallID);
    if (wall->getSide() == LEFT || wall->getSide() == RIGHT) {
        // Get the end point coordinates of the wall
        int y0 = wall->getY0();
        int y1 = wall->getY1();
        
        // Check intersection type 1a
        if (y0 == toMerge.first && y1 > toMerge.second) {
            // Set y0 to equal toMerge.second
            wall->setY0(toMerge.second);
        }
        
        // Check intersection type 1b
        if (y1 == toMerge.first && y0 > toMerge.second) {
            // Set y1 to equal toMerge.second
            wall->setY1(toMerge.second);
        }
        
        // Check intersection type 2a
        if (y0 < toMerge.first && y1 == toMerge.second) {
            // Set y1 to equal toMerge.first
            wall->setY1(toMerge.first);
        }
        
        // Check intersection type 2b
        if (y1 < toMerge.first && y0 == toMerge.second) {
            // Set y0 to equal toMerge.first
            wall->setY0(toMerge.first);
        }
        
        // Check intersection type 3a
        if (y0 == toMerge.first && y1 == toMerge.second) {
            // Remove wall
            std::vector<Wall*>* domwalls = parent->getWalls();
            domwalls->erase(domwalls->begin() + wallID);
        }
        
        // Check intersection type 3b
        if (y1 == toMerge.first && y0 == toMerge.second) {
            // Remove wall
            std::vector<Wall*>* domwalls = parent->getWalls();
            domwalls->erase(domwalls->begin() + wallID);
        }
        
        // Check intersection type 4a
        if (y0 < toMerge.first && y1 > toMerge.second) {
            // Set y1 to equal toMerge.first
            wall->setY1(toMerge.first);
            
            // Create a new wall from toMerge.second to y1
            Wall* newWall = new Wall(wall->getX0(), toMerge.second, wall->getX0(), y1, wall->getSide());
            parent->getWalls()->push_back(newWall);
        }
        
        // Check intersection type 4b
        if (y1 < toMerge.first && y0 > toMerge.second) {
            // Set y0 to equal toMerge.first
            wall->setY0(toMerge.first);
            
            // Create a new wall from toMerge.second to y0
            Wall* newWall = new Wall(wall->getX0(), toMerge.second, wall->getX0(), y0, wall->getSide());
            parent->getWalls()->push_back(newWall);
        }
    } else {
        // Get the end point coordinates of the wall
        int x0 = wall->getX0();
        int x1 = wall->getX1();
        
        // Check intersection type 1a
        if (x0 == toMerge.first && x1 > toMerge.second) {
            // Set x0 to equal toMerge.second
            wall->setX0(toMerge.second);
        }
        
        // Check intersection type 1b
        if (x1 == toMerge.first && x0 > toMerge.second) {
            // Set x1 to equal toMerge.second
            wall->setX1(toMerge.second);
        }
        
        // Check intersection type 2a
        if (x0 < toMerge.first && x1 == toMerge.second) {
            // Set x1 to equal toMerge.first
            wall->setX1(toMerge.first);
        }
        
        // Check intersection type 2b
        if (x1 < toMerge.first && x0 == toMerge.second) {
            // Set x0 to equal toMerge.first
            wall->setX0(toMerge.first);
        }
        
        // Check intersection type 3a
        if (x0 == toMerge.first && x1 == toMerge.second) {
            // Remove wall
            std::vector<Wall*>* domwalls = parent->getWalls();
            domwalls->erase(domwalls->begin() + wallID);
        }
        
        // Check intersection type 3b
        if (x1 == toMerge.first && x0 == toMerge.second) {
            // Remove wall
            std::vector<Wall*>* domwalls = parent->getWalls();
            domwalls->erase(domwalls->begin() + wallID);
        }
        
        // Check intersection type 4a
        if (x0 < toMerge.first && x1 > toMerge.second) {
            // Set x1 to equal toMerge.first
            wall->setX1(toMerge.first);
            
            // Create a new wall from toMerge.second to x1
            Wall* newWall = new Wall(wall->getY0(), toMerge.second, wall->getY0(), x1, wall->getSide());
            parent->getWalls()->push_back(newWall);
        }
        
        // Check intersection type 4b
        if (x1 < toMerge.first && x0 > toMerge.second) {
            // Set x0 to equal toMerge.first
            wall->setX0(toMerge.first);
            
            // Create a new wall from toMerge.second to x0
            Wall* newWall = new Wall(wall->getY0(), toMerge.second, wall->getY0(), x0, wall->getSide());
            parent->getWalls()->push_back(newWall);
        }
    }
}
