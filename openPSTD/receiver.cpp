#include "receiver.h"

/**
 * Constructor.
 * 
 * @param x  The x coordinate of the receiver
 * @param y  The y coordinate of the receiver
 * @param settings  A reference to the Settings instance
 */
Receiver::Receiver(int x, int y, Settings* settings) {
    // Save position locally
    this->x = x;
    this->y = y;
    
    // Save reference variables locally
    this->settings = settings;
}

/**
 * Draws the receiver.
 * 
 * @param pixels  The pixels array to draw to
 * @param zoom  The current zoom level (as in model)
 * @param offsetX  The current x offset of the scene (as in model)
 * @param offsetY  The current y offset of the scene (as in model)
 */
void Receiver::draw(QImage* pixels, int zoom, int offsetX, int offsetY) {
    int d = 2;
    for (int i = -d; i < d; i++) {
        for (int j = -d; j < d; j++) {
            pixels->setPixel(
                zoom * x + i + offsetX,
                zoom * y + j + offsetY,
                settings->receiverColor
            );
        }
    }
}