#ifndef GRID_H
#define GRID_H

#include <QPoint>
#include "settings.h"
#include "model.h"

/**
 * Class providing methods for comparing a point against the grid.
 */
class Grid {
public:
    /**
     * Clamps a point to the grid, given its x and y
     * coordinates. Returns the input point if there
     * is no grid edge within clampDist.
     */
    static QPoint clampGrid(int x, int y, Model* model, bool* clamped);
    
    /**
     * Clamps a point to a domain wall, given its x and y
     * coordinates. Returns the input point if there is
     * no wall within clampDist.
     */
    static QPoint clampDomains(int x, int y, std::vector<Domain> domains, Model* model, bool* clamped, bool clampLastDomain);
    
    /**
     * Clamps a point to the grid and to domain walls. Returns the input point
     * if there is no wall or grid within clampDist.
     */
    static QPoint clampFull(int x, int y, Model* model, bool clampLastDomain);
    
    /**
     * Checks whether or not a given point is on a grid edge.
     */
    static bool isOnGrid(int x, int y, Model* model);
private:
    /**
     * Clamps a point to a given wall. Always returns a point on the wall.
     */
    static QPoint clampWall(int x, int y, Wall* wall);
};

#endif