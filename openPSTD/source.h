#ifndef SOURCE_H
#define SOURCE_H

#include <QImage>
#include "settings.h"

/**
 * Representation of a single source.
 */
class Source {
public:
    // Constructor
    Source(int x, int y);
    
    // Drawing method
    void draw(QImage* pixels, int zoom, int offsetX, int offsetY);
    
    // Get methods for the position of the source
    int getX() { return x; }
    int getY() { return y; }
    bool getSelected() { return selected; }
    
    // Set methods for the position of the source
    void setX(int x) { this->x = x; }
    void setY(int y) { this->y = y; }
    void setSelected(bool selected) { this->selected = selected; }
private:
    // Position of the source
    int x;
    int y;
    
    // State variables
    bool selected;
    
    // Sets a single pixel's color
    void setPixel(int x, int y, QRgb color, QImage* pixels);
};

#endif