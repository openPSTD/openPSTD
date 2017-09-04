#ifndef WALL_H
#define WALL_H

#include <QImage>
#include <QPainter>
#include <QPen>
#include <QFont>
#include <iostream>
#include <algorithm>

enum Side {
    LEFT,
    RIGHT,
    TOP,
    BOTTOM
};

/**
 * Representation of a single wall in a domain.
 */
class Wall {
public:
    // Constructor
    inline Wall() {}
    Wall(int x0, int y0, int x1, int y1, Side side);
    
    // Drawing method
    void draw(QImage* pixels, int zoom);
    
    // Get methods
    inline int getX0() { return x0; }
    inline int getY0() { return y0; }
    inline int getX1() { return x1; }
    inline int getY1() { return y1; }
    inline Side getSide() { return side; }
    
    // Set methods
    inline void setX0(int x0) { this->x0 = x0; }
    inline void setY0(int y0) { this->y0 = y0; }
    inline void setX1(int x1) { this->x1 = x1; }
    inline void setY1(int y1) { this->y1 = y1; }
    inline void setSide(Side side) { this->side = side; }
    
    // Checks if two given walls need to be merged
    static bool mergeWalls(Wall one, Wall two, std::pair<int, int>* toMerge);
private:
    // Corner coordinates
    int x0;
    int y0;
    int x1;
    int y1;
    
    // Side of the wall
    Side side;
    
    // Private drawing methods
    void drawText(std::string text, int x, int y, int size, QRgb color, QImage* pixels);
    
    // Sorts a vector of integers
    static std::vector<int> sort(std::vector<int> original);
    
    // Checks if a given point is on a wall, given its end point coordinates
    inline static bool pointOnWall(int point, int wallstart, int wallend) {
        return (std::min(wallstart, wallend) <= point && point <= std::max(wallstart, wallend));
    }
};

#endif