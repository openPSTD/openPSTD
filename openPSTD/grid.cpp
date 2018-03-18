#include "grid.h"

/**
 * Clamps a point to the grid, given its x and y
 * coordinates. Returns the input point if there
 * is no grid edge within clampDist.
 * 
 * @param x  The x coordinate of the point to clamp
 * @param y  The y coordinate of the point to clamp
 * @param model  A reference to the Model
 * @param settings  A reference to the Settings instance
 * @param clamped  Whether or not the given (x, y) position could be clamped
 * @return  The clamped point
 */
QPoint Grid::clampGrid(int x, int y, Model* model, bool* clamped) {
    // Update x according to the scene offset
    int xx = x - model->offsetX;
    int yy = -y + model->offsetY;
    
    // Do not clamp if there is no grid
    if (!model->showGrid) {
        return QPoint(xx, yy);
    }
    
    // Compute the coordinates of the nearest grid corner
    int gridx = (model->gridsize * model->zoom) * round((double) xx / (model->gridsize * model->zoom));
    int gridy = (model->gridsize * model->zoom) * round((double) yy / (model->gridsize * model->zoom));
    
    // Compute the distance to this point from the given point
    int dx = std::abs(gridx - xx);
    int dy = std::abs(gridy - yy);
    
    // Clamp if the point is within clampDist, otherwise take itself
    Settings* settings = Settings::getInstance();
    int px = (dx < settings->clampDist ? gridx : xx);
    int py = (dy < settings->clampDist ? gridy : yy);
    
    // Return the clamped point
    *clamped = dx < settings->clampDist || dy < settings->clampDist;
    return QPoint(px, -py);
}

/**
 * Clamps a point to a domain wall, given its x and y
 * coordinates. Returns the input point if there is
 * no wall within clampDist.
 * 
 * @param x  The x coordinate of the point to clamp
 * @param y  The y coordinate of the point to clamp
 * @param domains  A vector of domains to clamp to
 * @param model  A reference to the Model
 * @param settings  A reference to the Settings instance
 * @param clamped  Whether or not the given (x, y) position could be clamped
 * @param clampLastDomain  Whether or not to clamp to the last domain as well
 * @return  The clamped point
 */
QPoint Grid::clampDomains(int x, int y, std::vector<Domain> domains, Model* model, bool* clamped, bool clampLastDomain) {
    // Update x according to the scene offset and zoom level
    int xx = (x - model->offsetX) / model->zoom;
    int yy = -(y - model->offsetY) / model->zoom;
    
    // Return the input point if there are no domains to clamp to
    if (domains.size() == 0) {
        *clamped = false;
        return QPoint(x, y);
    }
    
    // Return the input point if the only domain is not allowed to be clamped to
    if (domains.size() == 1 && !clampLastDomain) {
        *clamped = false;
        return QPoint(x, y);
    }
    
    // Loop through all domains
    QPoint domainClamp(-1, -1);
    int mindx = -1;
    int mindy = -1;
    int mind2 = -1;
    for (unsigned int i = 0; i < domains.size(); i++) {
        // Do not clamp to the last domain if requested so
        if (!clampLastDomain && i == domains.size() - 1) continue;
        
        // Loop through all walls in the domain
        std::vector<Wall*>* walls = domains[i].getWalls();
        for (unsigned int j = 0; j < walls->size(); j++) {
            // Get a pointer to this wall
            Wall* wall = walls->at(j);
            
            // Clamp the input point to this wall
            QPoint clamped = clampWall(xx, yy, wall);
            
            // Compute the distance of the input point to the clamped point
            int dx = xx - clamped.x();
            int dy = yy - clamped.y();
            int d2 = dx*dx + dy*dy;
            
            // Check if this distance is less than the current smallest distance
            if (mind2 == -1 || d2 < mind2) {
                // Save the new closest point
                domainClamp = QPoint(
                    clamped.x() * model->zoom,
                    -clamped.y() * model->zoom
                );
                mindx = std::abs(dx) * model->zoom;
                mindy = std::abs(dy) * model->zoom;
                mind2 = d2;
            }
        }
    }
    
    // Return the input point if the closest clamped point is not within clampDist
    Settings* settings = Settings::getInstance();
    if (mindx > settings->clampDist || mindy > settings->clampDist) {
        *clamped = false;
        return QPoint(x, y);
    }
    
    // Return the clamped point otherwise
    *clamped = true;
    return domainClamp;
}

/**
 * Clamps a point to the grid and to domain walls. Returns the input point
 * if there is no wall or grid within clampDist.
 * 
 * @param x  The x coordinate of the point to clamp
 * @param y  The y coordinate of the point to clamp
 * @param model  A reference to the Model
 * @param settings  A reference to the Settings instance
 * @param clampLastDomain  Whether or not to clamp to the last domain as well
 * @return  The clamped point
 */
QPoint Grid::clampFull(int x, int y, Model* model, bool clampLastDomain) {
    // Clamp the input point to the grid
    bool clampedGrid;
    QPoint clampgrid = Grid::clampGrid(x, y, model, &clampedGrid);
    
    // Clamp the input point to domain walls
    bool clampedWall;
    QPoint clampwall = Grid::clampDomains(x, y, model->domains, model, &clampedWall, clampLastDomain);
    
    // If only a wall is within clampDist, clamp to that wall
    if (!clampedGrid && clampedWall) return clampwall;
    
    // If only a grid is within clampDist, clamp to the grid
    if (clampedGrid && !clampedWall) return clampgrid;
    
    // If both a grid and wall are within clampDist, compute the closest
    if (clampedGrid && clampedWall) {
        // Compute the dx and dy to the grid and wall
        int dgx = std::abs(clampgrid.x() - x);
        int dgy = std::abs(clampgrid.y() - y);
        int dwx = std::abs(clampwall.x() - x);
        int dwy = std::abs(clampwall.y() - y);
        
        // Compute the total distance to the grid and wall
        int dg = dgx*dgx + dgy*dgy;
        int dw = dwx*dwx + dwy*dwy;
        
        // Return the closest
        if (dg < dw) return clampgrid;
        return clampwall;
    }
    
    // If neither is within clampDist, return the input point
    return QPoint(x - model->offsetX, y - model->offsetY);
}

/**
 * Checks whether or not a given point is on a grid edge.
 * 
 * @param x  The x coordinate of the point to test
 * @param y  The y coordinate of the point to test
 * @param model  A reference to the Model
 * @return  Whether or not the given point is on a grid edge
 */
bool Grid::isOnGrid(int x, int y, Model* model) {
    // Pixel is not on grid if there is no grid
    if (!model->showGrid) return false;
    
    // Return whether or not the given point is on a grid edge
    return ((x - model->offsetX) % (model->zoom * model->gridsize) == 0 || (y - model->offsetY) % (model->zoom * model->gridsize) == 0);
}

/**
 * Clamps a point to a given wall. Always returns a point on the wall.
 * 
 * @param x  The x coordinate of the point to clamp
 * @param y  The y coordinate of the point to clamp
 * @param wall  A reference to the wall to clamp to
 * @return  The clamped point
 */
QPoint Grid::clampWall(int x, int y, Wall* wall) {
    // Check if the wall is vertical or horizontal
    if (wall->getSide() == LEFT || wall->getSide() == RIGHT) {
        // Get the lowest and highest points of the wall
        int miny = std::min(wall->getY0(), wall->getY1());
        int maxy = std::max(wall->getY0(), wall->getY1());
        
        // Return the lowest point if y < miny
        if (y < miny) return QPoint(wall->getX0(), miny);
        
        // Return the highest point if y > maxy
        if (y > maxy) return QPoint(wall->getX0(), maxy);
        
        // Return the same y coordinate otherwise
        return QPoint(wall->getX0(), y);
    } else {
        // Get the leftmost and rightmost points of the wall
        int minx = std::min(wall->getX0(), wall->getX1());
        int maxx = std::max(wall->getX0(), wall->getX1());
        
        // Return the leftmost point if x < minx
        if (x < minx) return QPoint(minx, wall->getY0());
        
        // Return the rightmost point if x > maxx
        if (x > maxx) return QPoint(maxx, wall->getY0());
        
        // Return the same x coordinate otherwise
        return QPoint(x, wall->getY0());
    }
}