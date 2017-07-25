#ifndef DOMAIN_H
#define DOMAIN_H

#include <QImage>
#include <QPen>
#include <QPainter>
#include <iostream>

/**
 * Representation of a single domain.
 */
class Domain {
public:
    // Constructor
    Domain(int x0, int y0, int x1, int y1);
    
    // Drawing method
    void draw(QImage* pixels, int zoom);
    
    // Get methods for the domain's corner coordinates
    int getX0() { return x0; }
    int getY0() { return y0; }
    
    // Set methods for the domain's corner coordinates
    void setX1(int x1) { this->x1 = x1; }
    void setY1(int y1) { this->y1 = y1; }
private:
    // Corner coordinates
    int x0;
    int x1;
    int y0;
    int y1;
    
    // Private drawing methods
    enum Side {
        LEFT,
        RIGHT,
        TOP,
        BOTTOM
    };
    void drawWallLength(int x0, int y0, int x1, int y1, QImage* pixels, int zoom, Side side);
    void drawText(std::string text, int x, int y, int size, QRgb color, QImage* pixels);
};

#endif