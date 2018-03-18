#include "receiver.h"

/**
 * Constructor.
 * 
 * @param x  The x coordinate of the receiver
 * @param y  The y coordinate of the receiver
 * @param settings  A reference to the Settings instance
 */
Receiver::Receiver(int x, int y) {
    // Save position locally
    this->x = x;
    this->y = y;
    
    // Initialize state variables
    selected = false;
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
    // Draw a square representing the receiver
    int d = 2;
    for (int i = -d; i <= d; i++) {
        for (int j = -d; j <= d; j++) {
            setPixel(
                x * zoom + i + offsetX,
                -y * zoom + j + offsetY,
                Settings::getInstance()->receiverColor,
                pixels
            );
        }
    }
    
    // Check if this receiver is selected
    if (selected) {
        // Draw a square around the receiver
        for (int i = -5; i <= 5; i++) {
            setPixel(
                x * zoom + i + offsetX,
                -y * zoom + 5 + offsetY,
                qRgb(0, 255, 255),
                pixels
            );
            setPixel(
                x * zoom + i + offsetX,
                -y * zoom - 5 + offsetY,
                qRgb(0, 255, 255),
                pixels
            );
            setPixel(
                x * zoom + 5 + offsetX,
                -y * zoom + i + offsetY,
                qRgb(0, 255, 255),
                pixels
            );
            setPixel(
                x * zoom - 5 + offsetX,
                -y * zoom + i + offsetY,
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