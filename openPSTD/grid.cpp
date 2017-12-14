#include "grid.h"

/**
 * Checks whether or not a given point is on the background grid.
 * 
 * @param pos  The point to check (screen coordinates)
 * @return  Whether or not the given point is on the backgroind grid
 */
bool Grid::isOnGrid(QPoint pos) {
    // Get a reference to the Model instance
    Model* model = Model::getInstance();
    
    // Normalize the given point
    QPoint normalized = Utility::scr2obj(pos);
    
    // Check if the normalized point is on the grid
    bool xongrid = normalized.x() % model->gridsize == 0;
    bool yongrid = normalized.y() % model->gridsize == 0;
    
    // Return whether or not the given point is on the grid
    return xongrid || yongrid;
}

/**
 * Clamps a given point to any clamping element.
 * This includes the background grid, and domain walls.
 * Returns whether or not the point was clamped (i.e. pos is on
 * the grid after this method has finished executing).
 * 
 * @param pos  The point to clamp (screen coordinates)
 * @modifies pos
 * @return  Whether or not the point was clamped
 * @throws runtime_error  When pos is a nullptr
 */
bool Grid::clamp(QPoint* pos) {
    // Verify that pos is a QPoint
    if (pos == nullptr) {
        throw std::runtime_error("[Grid] clamp called with nullptr argument");
    }
    
    // Try to clamp the given point to the background grid
    QPoint p1 = *pos;
    bool ongrid = clampGrid(&p1);
    
    // Try to clamp the given point to a domain wall
    QPoint p2 = *pos;
    bool onwall = clampWalls(&p2);
    
    // Return the original point if neither element clamped the point
    if (!ongrid && !onwall) return false;
    
    // Clamp to the grid if this was the only clamping element
    if (ongrid && !onwall) {
        *pos = p1;
        return true;
    }
    
    // Clamp to the wall if this was the only clamping element
    if (!ongrid && onwall) {
        *pos = p2;
        return true;
    }
    
    // Compute the closest element if both elements clamped
    if (ongrid && onwall) {
        // Compute the distance of the original point to the grid clamp
        int d1x = std::abs(pos->x() - p1.x());
        int d1y = std::abs(pos->y() - p1.y());
        int d1 = d1x + d1y;
        
        // Compute the distance of the original point to the wall clamp
        int d2x = std::abs(pos->x() - p2.x());
        int d2y = std::abs(pos->y() - p2.y());
        int d2 = d2x + d2y;
        
        // Clamp to the closest of the two points
        if (d1 < d2) {
            *pos = p1;
            return true;
        } else {
            *pos = p2;
            return true;
        }
    }
}

/**
 * Clamps a given point to the background grid.
 * Returns whether or not the point was clamped (i.e. pos is on
 * the grid after this method has finished executing).
 * 
 * @param pos  The point to clamp (screen coordinates)
 * @modifies pos
 * @return  Whether or not the point was clamped
 * @throws runtime_error  When pos is a nullptr
 */
bool Grid::clampGrid(QPoint* pos) {
    // Verify that pos is a QPoint
    if (pos == nullptr) {
        throw std::runtime_error("[Grid] clampGrid called with nullptr argument");
    }
    
    // Do not clamp if the grid is hidden
    Model* model = Model::getInstance();
    if (!model->showGrid) return false;
    
    // Normalize the given point
    QPoint normalized = Utility::scr2obj(*pos);
    
    // Compute the coordinates of the nearest grid edges
    unsigned int g = model->gridsize;
    int gridx = g * round((double) normalized.x() / g);
    int gridy = g * round((double) normalized.y() / g);
    
    // Compute the distance from the given point to the grid edges
    unsigned int dx = std::abs(gridx - normalized.x());
    unsigned int dy = std::abs(gridy - normalized.y());
    
    // Re-apply offset and zoom level to the grid edges
    QPoint grido = Utility::obj2scr(QPoint(gridx, gridy));
    
    // Clamp if the grid edges are within the maximum clamp distance
    bool clamped = false;
    if (dx <= Settings::getInstance()->clampDist) {
        pos->setX(grido.x());
        clamped = true;
    }
    if (dy <= Settings::getInstance()->clampDist) {
        pos->setY(grido.y());
        clamped = true;
    }
    
    // Return whether or not the given point was clamped
    return clamped;
}

/**
 * Clamps a given point to domain walls.
 * Returns whether or not the point was clamped (i.e. pos is on
 * a domain wall after this method has finished executing).
 * 
 * @param pos  The point to clamp (screen coordinates)
 * @modifies pos
 * @return  Whether or not the point was clamped
 * @throws runtime_error  When pos is a nullptr
 */
bool Grid::clampWalls(QPoint* pos) {
    // Verify that pos is a QPoint
    if (pos == nullptr) {
        throw std::runtime_error("[Grid] clampWalls called with nullptr argument");
    }
    
    // Normalize the given point
    Model* model = Model::getInstance();
    QPoint normalized = Utility::scr2obj(*pos);
    
    // Loop through all domains
    bool isClamped = false;
    unsigned int mind;
    QPoint result;
    for (unsigned int i = 0; i < model->domains.size(); i++) {
        // Get the walls in this domain
        std::vector<Wall>* walls = model->domains[i]->getWalls();
        
        // Loop through all walls in this domain
        for (unsigned int j = 0; j < walls->size(); j++) {
            // Clamp the input point to this wall
            QPoint clamped = clampWall(walls->at(j), normalized);
            
            // Compute the distance from the given point to the clamped point
            unsigned int dx = std::abs(clamped.x() - normalized.x());
            unsigned int dy = std::abs(clamped.y() - normalized.y());
            unsigned int d = std::sqrt(dx*dx + dy*dy);
            
            // Update the closest wall
            if (i + j == 0 || d < mind) {
                mind = d;
                result = clamped;
                isClamped = true;
            }
        }
    }
    
    // Check if the closest point is within the maximum clamp distance
    if (isClamped && mind < Settings::getInstance()->clampDist) {
        // Clamp to this point
        *pos = result;
        return true;
    } else {
        // Do not clamp
        return false;
    }
}

/**
 * Clamps a given point to a given wall.
 * Will always clamp to the closest point on the wall, regardless
 * of Settings::clampdist.
 * 
 * @param wall  The wall to clamp to
 * @param pos  The original point to clamp (screen coordinates)
 * @return  The clamped point
 */
QPoint Grid::clampWall(Wall wall, QPoint pos) {
    // Check if the wall is horizontal or vertical
    if (wall.isHorizontal()) {
        // Get the leftmost and rightmost points of the wall
        int minx = std::min(wall.getLine()->x1(), wall.getLine()->x2());
        int maxx = std::max(wall.getLine()->x1(), wall.getLine()->x2());
        
        // Return the leftmost point if x < minx
        if (pos.x() < minx) return QPoint(minx, wall.getLine()->y1());
        
        // Return the rightmost point if x > maxx
        if (pos.x() > maxx) return QPoint(maxx, wall.getLine()->y1());
        
        // Return the same x coordinate otherwise
        return QPoint(pos.x(), wall.getLine()->y1());
    } else {
        // Get the lowest and highest points of the wall
        int miny = std::min(wall.getLine()->y1(), wall.getLine()->y2());
        int maxy = std::max(wall.getLine()->y1(), wall.getLine()->y2());
        
        // Return the lowest point if y < miny
        if (pos.y() < miny) return QPoint(wall.getLine()->x1(), miny);
        
        // Return the highest point if y > maxy
        if (pos.y() > maxy) return QPoint(wall.getLine()->x1(), maxy);
        
        // Return the same y coordinate otherwise
        return QPoint(wall.getLine()->x1(), pos.y());
    }
}