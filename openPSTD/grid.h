#ifndef GRID_H
#define GRID_H

#include <math.h>
#include <QPoint>
#include "modelmanager.h"
#include "settings.h"
#include "utility.h"
#include "domain.h"

/**
 * Contains utility methods regarding the background grid.
 */
class Grid {
public:
    // Checks whether or not a given point is on the background grid
    static bool isOnGrid(QPoint pos);
    
    // Clamps a given point to any clamping element
    static bool clamp(QPoint* pos);
    
    // Clamps a given point to the background grid
    static bool clampGrid(QPoint* pos);
    
    // Clamps a given point to domain walls
    static bool clampWalls(QPoint* pos);
private:
    // Clamps a given point to a given wall
    static QPoint clampWall(Wall wall, QPoint pos);
};

#endif