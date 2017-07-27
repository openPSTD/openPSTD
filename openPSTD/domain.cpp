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
    // Create four walls for this domain
    walls.push_back(Wall(x0, y0, x0, y1, LEFT));
    walls.push_back(Wall(x1, y0, x1, y1, RIGHT));
    walls.push_back(Wall(x0, y0, x1, y0, TOP));
    walls.push_back(Wall(x0, y1, x1, y1, BOTTOM));
}

/**
 * Drawing method.
 * Draws the domain to a given QImage
 * 
 * @param pixels  A reference te the QImage to draw to
 * @param zoom  The current zoom level (as in model)
 */
void Domain::draw(QImage* pixels, int zoom) {
    // Update the sides of all walls
    updateWallSides();
    
    // Draw all walls
    for (unsigned int i = 0; i < walls.size(); i++) {
        walls[i].draw(pixels, zoom);
    }
}

/**
 * Get method for the x coordinate of the left wall.
 * @return The x coordinate of the left wall
 */
int Domain::getX0() {
    int minx = -1;
    for (unsigned int i = 0; i < walls.size(); i++) {
        int x = walls[i].getX0();
        if (minx == -1 || x < minx) minx = x;
    }
    return minx;
}

/**
 * Get method for the y coordinate of the top wall.
 * @return The y coordinate of the top wall
 */
int Domain::getY0() {
    int miny = -1;
    for (unsigned int i = 0; i < walls.size(); i++) {
        int y = walls[i].getY0();
        if (miny == -1 || y < miny) miny = y;
    }
    return miny;
}

/**
 * Get method for the x coordinate of the right wall.
 * @return The x coordinate of the right wall
 */
int Domain::getX1() {
    int maxx = -1;
    for (unsigned int i = 0; i < walls.size(); i++) {
        int x = walls[i].getX1();
        if (maxx == -1 || x > maxx) maxx = x;
    }
    return maxx;
}

/**
 * Get method for the y coordinate of the bottom wall.
 * @return The y coordinate of the bottom wall
 */
int Domain::getY1() {
    int maxy = -1;
    for (unsigned int i = 0; i < walls.size(); i++) {
        int y = walls[i].getY1();
        if (maxy == -1 || y > maxy) maxy = y;
    }
    return maxy;
}

/**
 * Set method for the x coordinate of the right wall
 * @param x1  The x coordinate to give to the right wall
 */
void Domain::setX1(int x1) {
    // Loop through all walls
    for (unsigned int i = 0; i < walls.size(); i++) {
        // If this is a right wall, update its x coordinates
        if (walls[i].getSide() == RIGHT) {
            walls[i].setX0(x1);
            walls[i].setX1(x1);
        }
        
        // If this is a top wall, update its end coordinate
        if (walls[i].getSide() == TOP) {
            walls[i].setX1(x1);
        }
        
        // If this is a bottom wall, update its end coordinate
        if (walls[i].getSide() == BOTTOM) {
            walls[i].setX1(x1);
        }
    }
}

/**
 * Set method for the y coordinate of the bottom wall
 * @param y1  The y coordinate to give to the bottom wall
 */
void Domain::setY1(int y1) {
    // Loop through all walls
    for (unsigned int i = 0; i < walls.size(); i++) {
        // If this is a left wall, update its end coordinate
        if (walls[i].getSide() == LEFT) {
            walls[i].setY1(y1);
        }
        
        // If this is a right wall, update its end coordinate
        if (walls[i].getSide() == RIGHT) {
            walls[i].setY1(y1);
        }
        
        // If this is a bottom wall, update its y coordinates
        if (walls[i].getSide() == BOTTOM) {
            walls[i].setY0(y1);
            walls[i].setY1(y1);
        }
    }
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
        int x0 = walls[i].getX0();
        int x1 = walls[i].getX1();
        int y0 = walls[i].getY0();
        int y1 = walls[i].getY1();
        
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
        int x0 = walls[i].getX0();
        int x1 = walls[i].getX1();
        int y0 = walls[i].getY0();
        int y1 = walls[i].getY1();
        
        // Check if this is a vertical wall
        if (x0 == x1 && y0 != y1) {
            // Set side according to position relative to the midpoint
            if (x0 < midx) walls[i].setSide(LEFT);
            if (x0 > midx) walls[i].setSide(RIGHT);
        }
        
        // Check if this is a horizontal wall
        if (x0 != x1 && y0 == y1) {
            // Set side according to position relative to the midpoint
            if (y0 < midy) walls[i].setSide(TOP);
            if (y0 > midy) walls[i].setSide(BOTTOM);
        }
    }
}