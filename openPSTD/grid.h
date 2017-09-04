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
     * 
     * @param x  The x coordinate of the point to clamp
     * @param y  The y coordinate of the point to clamp
     * @param model  A reference to the Model
     * @param settings  A reference to the Settings instance
     * @return  The clamped point
     */
    inline static QPoint clampGrid(int x, int y, Model* model, Settings* settings) {
        // Update x according to the scene offset
        int xx = x - model->offsetX;
        int yy = y - model->offsetY;
        
        // Compute the coordinates of the nearest grid corner
        int gridx = (model->gridsize * model->zoom) * round((double) xx / (model->gridsize * model->zoom));
        int gridy = (model->gridsize * model->zoom) * round((double) yy / (model->gridsize * model->zoom));
        
        // Compute the distance to this point from the given point
        int dx = std::abs(gridx - xx);
        int dy = std::abs(gridy - yy);
        
        // Clamp if the point is within clampDist, otherwise take itself
        int px = (dx < settings->clampDist ? gridx : xx);
        int py = (dy < settings->clampDist ? gridy : yy);
        
        // Return the clamped point
        return QPoint(px, py);
    }
    
    /**
     * Checks whether or not a given point is on a grid edge.
     * 
     * @param x  The x coordinate of the point to test
     * @param y  The y coordinate of the point to test
     * @param model  A reference to the Model
     * @return  Whether or not the given point is on a grid edge
     */
    inline static bool isOnGrid(int x, int y, Model* model) {
        // Return whether or not the given point is on a grid edge
        return ((x - model->offsetX) % (model->zoom * model->gridsize) == 0 || (y - model->offsetY) % (model->zoom * model->gridsize) == 0);
    }
};

#endif