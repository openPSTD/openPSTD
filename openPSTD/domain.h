#ifndef DOMAIN_H
#define DOMAIN_H

#include <vector>
#include <QImage>
#include <QRect>
#include "wall.h"

/**
 * Represents a single domain.
 */
class Domain {
public:
    // Constructor
    Domain(QRect rect);
    
    // Draws the domain to the given pixels array
    void draw(QImage* pixels);
    
    // Get methods for representation variables
    QRect getRect();
    std::vector<Wall>* getWalls();
    
    // Set methods for representation variables
    void setP2(QPoint pos);
    
    // Recalculates all walls
    void recalcWalls();
private:
    // Representation variables
    QPoint corner1;
    QPoint corner2;
    std::vector<Wall> walls;
    
    // Creates a fully merged wall
    void createWall(Side side);
};

#endif