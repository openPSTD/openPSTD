#ifndef WALL_H
#define WALL_H

#include <QLine>
#include <QImage>
#include "modelmanager.h"
#include "utility.h"
#include "settings.h"

// Side enum
enum Side {
    TOP,
    BOTTOM,
    LEFT,
    RIGHT
};

/**
 * Represents a single wall.
 */
class Wall {
public:
    // Constructor
    Wall(QLine line, Side side);
    
    // Draws the wall to the given pixels array
    void draw(QImage* pixels);
    
    // Get methods for representation variables
    QLine* getLine();
    Side getSide();
    bool isHorizontal();
    double getAbsorption();
    
    // Set methods for representation variables
    void setAbsorption(double absorption);
private:
    // Representation variables
    QLine line;
    Side side;
    double absorption;
};

#endif