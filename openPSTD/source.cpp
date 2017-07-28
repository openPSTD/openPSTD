#include "source.h"

/**
 * Constructor.
 * 
 * @param x  The x coordinate of the source
 * @param y  The y coordinate of the source
 * @param settings  A reference to a Settings instance
 */
Source::Source(int x, int y, Settings* settings) {
    // Save position locally
    this->x = x;
    this->y = y;
    
    // Save reference variables locally
    this->settings = settings;
}

/**
 * Draws the source.
 * 
 * @param pixels  The pixels array to draw to
 * @param zoom  The current zoom level (as in model)
 */
void Source::draw(QImage* pixels, int zoom) {
    int d = 2;
    for (int i = -d; i < d; i++) {
        for (int j = -d; j < d; j++) {
            pixels->setPixel(
                zoom * x + i,
                zoom * y + j,
                settings->sourceColor
            );
        }
    }
}