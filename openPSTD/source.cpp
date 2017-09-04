#include "source.h"

/**
 * Constructor.
 * 
 * @param x  The x coordinate of the source
 * @param y  The y coordinate of the source
 * @param settings  A reference to the Settings instance
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
 * @param offsetX  The current x offset of the scene (as in model)
 * @param offsetY  The current y offset of the scene (as in model)
 * @param selected  Whether or not the source is currently selected
 */
void Source::draw(QImage* pixels, int zoom, int offsetX, int offsetY, bool selected) {
    // Draw a square representing the source
    int d = 2;
    for (int i = -d; i <= d; i++) {
        for (int j = -d; j <= d; j++) {
            pixels->setPixel(
                zoom * x + i + offsetX,
                zoom * y + j + offsetY,
                settings->sourceColor
            );
        }
    }
    
    // Check if this source is selected
    if (selected) {
        // Draw a square around the source
        for (int i = -5; i <= 5; i++) {
            pixels->setPixel(
                zoom * x + i + offsetX,
                zoom * y + 5 + offsetY,
                qRgb(0, 255, 255)
            );
            pixels->setPixel(
                zoom * x + i + offsetX,
                zoom * y - 5 + offsetY,
                qRgb(0, 255, 255)
            );
            pixels->setPixel(
                zoom * x + 5 + offsetX,
                zoom * y + i + offsetY,
                qRgb(0, 255, 255)
            );
            pixels->setPixel(
                zoom * x - 5 + offsetX,
                zoom * y + i + offsetY,
                qRgb(0, 255, 255)
            );
        }
    }
}