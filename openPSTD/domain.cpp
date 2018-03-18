#include "domain.h"

/**
 * Constructor.
 * 
 * @param x0  The x coordinate of the first corner of the domain
 * @param y0  The y coordinate of the first corner of the domain
 * @param x1  The x coordinate of the second corner of the domain
 * @param y1  The y coordinate of the second corner of the domain
 * @param settings  A reference to a Settings instance
 */
Domain::Domain(int x0, int y0, int x1, int y1) {
    // Save the domain's corner coordinates
    this->x0 = x0;
    this->y0 = y0;
    this->x1 = x1;
    this->y1 = y1;
    
    // Initialize the domain's absorption coefficients
    absorptionTop = 0;
    absorptionBottom = 0;
    absorptionLeft = 0;
    absorptionRight = 0;
    
    // Initialize state variables
    selected_domain = false;
    selected_top = false;
    selected_bottom = false;
    selected_left = false;
    selected_right = false;
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
        if (walls[i]->getSide() == TOP) {
            walls[i]->draw(pixels, zoom, offsetX, offsetY, selected_domain || selected_top);
        }
        if (walls[i]->getSide() == BOTTOM) {
            walls[i]->draw(pixels, zoom, offsetX, offsetY, selected_domain || selected_bottom);
        }
        if (walls[i]->getSide() == LEFT) {
            walls[i]->draw(pixels, zoom, offsetX, offsetY, selected_domain || selected_left);
        }
        if (walls[i]->getSide() == RIGHT) {
            walls[i]->draw(pixels, zoom, offsetX, offsetY, selected_domain || selected_right);
        }
    }
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
    
    // Compute min and max
    int minx = std::min(x0, x1);
    int maxx = std::max(x0, x1);
    int miny = std::min(y0, y1);
    int maxy = std::max(y0, y1);
    
    // Recreate the original four non-merged walls
    walls.push_back(new Wall(minx, miny, minx, maxy, LEFT, absorptionLeft));
    walls.push_back(new Wall(maxx, miny, maxx, maxy, RIGHT, absorptionRight));
    walls.push_back(new Wall(minx, maxy, maxx, maxy, TOP, absorptionTop));
    walls.push_back(new Wall(minx, miny, maxx, miny, BOTTOM, absorptionBottom));
}

/**
 * Checks whether or not this domain overlaps with a given other domain.
 * 
 * @param other  The domain to check overlap against
 * @return  Whether or not there is overlap between the two domains
 */
bool Domain::overlaps(Domain* other) {
    // Get corner coordinates of this domain
    int x0a = getX0();
    int y0a = getY0();
    int x1a = getX1();
    int y1a = getY1();
    
    // Get corner coordinates of other domain
    int x0b = other->getX0();
    int y0b = other->getY0();
    int x1b = other->getX1();
    int y1b = other->getY1();
    
    // Check if the domains overlap in the x direction
    bool overlapxa = ((x0a < x0b && x0b < x1a) || (x0a < x1b && x1b < x1a));
    bool overlapxb = ((x0b < x0a && x0a < x1b) || (x0b < x1a && x1a < x1b));
    bool overlapx = overlapxa || overlapxb;
    
    // Check if the domains overlap in the y direction
    bool overlapya = ((y0a < y0b && y0b < y1a) || (y0a < y1b && y1b < y1a));
    bool overlapyb = ((y0b < y0a && y0a < y1b) || (y0b < y1a && y1a < y1b));
    bool overlapy = overlapya || overlapyb;
    
    // Return whether or not the two domains overlap
    return (overlapx && overlapy);
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
            if (y0 < midy) walls[i]->setSide(BOTTOM);
            if (y0 > midy) walls[i]->setSide(TOP);
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
            Wall* newWall = new Wall(
                wall->getX0(),
                toMerge.second,
                wall->getX0(),
                y1,
                wall->getSide(),
                getAbsorption(wall->getSide())
            );
            parent->getWalls()->push_back(newWall);
        }
        
        // Check intersection type 4b
        if (y1 < toMerge.first && y0 > toMerge.second) {
            // Set y0 to equal toMerge.first
            wall->setY0(toMerge.first);
            
            // Create a new wall from toMerge.second to y0
            Wall* newWall = new Wall(
                wall->getX0(),
                toMerge.second,
                wall->getX0(),
                y0,
                wall->getSide(),
                getAbsorption(wall->getSide())
            );
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
            Wall* newWall = new Wall(
                wall->getY0(),
                toMerge.second,
                wall->getY0(),
                x1,
                wall->getSide(),
                getAbsorption(wall->getSide())
            );
            parent->getWalls()->push_back(newWall);
        }
        
        // Check intersection type 4b
        if (x1 < toMerge.first && x0 > toMerge.second) {
            // Set x0 to equal toMerge.first
            wall->setX0(toMerge.first);
            
            // Create a new wall from toMerge.second to x0
            Wall* newWall = new Wall(
                wall->getY0(),
                toMerge.second,
                wall->getY0(),
                x0,
                wall->getSide(),
                getAbsorption(wall->getSide())
            );
            parent->getWalls()->push_back(newWall);
        }
    }
}

/**
 * Returns the absorption of a wall given its side.
 * 
 * @param side  The side of the wall
 * @return  The absorption coefficient of this wall
 */
double Domain::getAbsorption(Side side) {
    if (side == TOP) return absorptionTop;
    if (side == BOTTOM) return absorptionBottom;
    if (side == LEFT) return absorptionLeft;
    if (side == RIGHT) return absorptionRight;
}

/**
 * Updates each wall's absorption coefficient according to
 * the domain's absorption coefficient.
 */
void Domain::updateAbsorption() {
    for (unsigned int i = 0; i < walls.size(); i++) {
        if (walls[i]->getSide() == TOP) walls[i]->setAbsorption(absorptionTop);
        if (walls[i]->getSide() == BOTTOM) walls[i]->setAbsorption(absorptionBottom);
        if (walls[i]->getSide() == LEFT) walls[i]->setAbsorption(absorptionLeft);
        if (walls[i]->getSide() == RIGHT) walls[i]->setAbsorption(absorptionRight);
    }
}

/**
 * Updates the domain's absorption coefficient given a side to update.
 * 
 * @param side  The side of which to update the absorption coefficient
 * @param value  The new value of the absorption coefficient
 */
void Domain::setAbsorption(Side side, double value) {
    if (side == TOP) absorptionTop = value;
    if (side == BOTTOM) absorptionBottom = value;
    if (side == LEFT) absorptionLeft = value;
    if (side == RIGHT) absorptionRight = value;
}