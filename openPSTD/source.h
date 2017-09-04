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
    Source(int x, int y, Settings* settings);
    
    // Drawing method
    void draw(QImage* pixels, int zoom, int offsetX, int offsetY);
    
    // Get methods for the position of the source
    int getX() { return x; }
    int getY() { return y; }
private:
    // Class instance variables
    Settings* settings;
    
    // Position of the source
    int x;
    int y;
};

#endif