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
        // Compute the coordinates of the nearest grid corner
        int gridx = (model->gridsize * model->zoom) * round((double) x / (model->gridsize * model->zoom));
        int gridy = (model->gridsize * model->zoom) * round((double) y / (model->gridsize * model->zoom));
        
        // Compute the distance to this point from the given point
        int dx = std::abs(gridx - x);
        int dy = std::abs(gridy - y);
        
        // Clamp if the point is within clampDist, otherwise take itself
        int px = (dx < settings->clampDist ? gridx : x);
        int py = (dy < settings->clampDist ? gridy : y);
        
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
        return (x % (model->zoom * model->gridsize) == 0 || y % (model->zoom * model->gridsize) == 0);
    }
};

#endif