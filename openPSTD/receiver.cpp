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
 * @param selected  Whether or not the receiver is currently selected
 */
void Receiver::draw(QImage* pixels, int zoom, int offsetX, int offsetY, bool selected) {
    // Draw a square representing the receiver
    int d = 2;
    for (int i = -d; i <= d; i++) {
        for (int j = -d; j <= d; j++) {
            setPixel(
                zoom * x + i + offsetX,
                zoom * y + j + offsetY,
                settings->receiverColor,
                pixels
            );
        }
    }
    
    // Check if this receiver is selected
    if (selected) {
        // Draw a square around the receiver
        for (int i = -5; i <= 5; i++) {
            setPixel(
                zoom * x + i + offsetX,
                zoom * y + 5 + offsetY,
                qRgb(0, 255, 255),
                pixels
            );
            setPixel(
                zoom * x + i + offsetX,
                zoom * y - 5 + offsetY,
                qRgb(0, 255, 255),
                pixels
            );
            setPixel(
                zoom * x + 5 + offsetX,
                zoom * y + i + offsetY,
                qRgb(0, 255, 255),
                pixels
            );
            setPixel(
                zoom * x - 5 + offsetX,
                zoom * y + i + offsetY,
                qRgb(0, 255, 255),
                pixels
            );
        }
    }
}

/**
 * Sets a single pixel's color.
 * 
 * @param x  The x coordinate of the pixel
 * @param y  The y coordinate of the pixel
 * @param color  The color to give the pixel
 * @param pixels  A pixels array to draw to
 */
void Receiver::setPixel(int x, int y, QRgb color, QImage* pixels) {
    if (x < 0 || y < 0) return;
    if (x >= pixels->width() || y >= pixels->height()) return;
    pixels->setPixel(x, y, color);
}