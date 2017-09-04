#ifndef DOMAIN_H
#define DOMAIN_H

#include <QImage>
#include <QPen>
#include <QPainter>
#include <vector>
#include "wall.h"

/**
 * Representation of a single domain.
 */
class Domain {
public:
    // Constructor
    Domain(int x0, int y0, int x1, int y1);
    
    // Drawing method
    void draw(QImage* pixels, int zoom, int offsetX, int offsetY);
    
    // Get methods for the domain's corner coordinates
    int getX0();
    int getY0();
    int getX1();
    int getY1();
    
    // Set methods for the domain's corner coordinates
    void setX1(int x1);
    void setY1(int y1);
    
    // Get method for the walls vector
    std::vector<Wall*>* getWalls() { return &walls; }
    
    // Merges this domain with all other domains
    void mergeDomains(std::vector<Domain>* domains, unsigned int ownID);
    
    // Resets the domain's walls to be the original four non-merged walls
    void resetWalls();
private:
    // Domain walls
    int x0;
    int y0;
    int x1;
    int y1;
    std::vector<Wall*> walls;
    
    // Method for updating the side of all walls
    void updateWallSides();
    
    // Updates a given wall according to a given intersecting segment
    void handleIntersection(Domain* parent, int wallID, std::pair<int, int> toMerge);
};

#endif