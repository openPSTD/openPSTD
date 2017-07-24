#include "model.h"

/**
 * Clamps a point to the grid, given its x and y
 * coordinates. Returns the input point if there
 * is no grid edge within clampDist.
 * 
 * @param x  The x coordinate of the point to clamp
 * @param y  The y coordinate of the point to clamp
 * @return  The clamped point
 */
QPoint Model::clampGrid(int x, int y) {
    // Compute the coordinates of the nearest grid corner
    int gridx = (gridsize * zoom) * round((double) x / (gridsize * zoom));
    int gridy = (gridsize * zoom) * round((double) y / (gridsize * zoom));
    
    // Compute the distance to this point from the given point
    int dx = std::abs(gridx - x);
    int dy = std::abs(gridy - y);
    
    // Clamp if the point is within clampDist, otherwise take itself
    int px = (dx < clampDist ? gridx : x);
    int py = (dy < clampDist ? gridy : y);
    
    // Return the clamped point
    return QPoint(px, py);
}

/**
 * Checks whether or not a given point is on a grid edge.
 * 
 * @param x  The x coordinate of the point to test
 * @param y  The y coordinate of the point to test
 * @return  Whether or not the given point is on a grid edge
 */
bool Model::isOnGrid(int x, int y) {
    // Return whether or not the given point is on a grid edge
    return (x % (zoom * gridsize) == 0 || y % (zoom * gridsize) == 0);
}