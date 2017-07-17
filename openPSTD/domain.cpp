#include "domain.h"

/**
 * Constructor.
 * 
 * @param x0  The x coordinate of the first corner of the domain
 * @param y0  The y coordinate of the first corner of the domain
 * @param x1  The x coordinate of the second corner of the domain
 * @param y1  The y coordinate of the second corner of the domain
 */
Domain::Domain(int x0, int y0, int x1, int y1) {
    // Save the corner coordinates locally
    this->x0 = x0;
    this->y0 = y0;
    this->x1 = x1;
    this->y1 = y1;
}

/**
 * Drawing method.
 * Draws the domain to a given QImage
 * 
 * @param pixels  A reference te the QImage to draw to
 */
void Domain::draw(QImage* pixels) {
    // Get coordinates of the corners
    int minx = std::min(x0, x1);
    int maxx = std::max(x0, x1);
    int miny = std::min(y0, y1);
    int maxy = std::max(y0, y1);
    
    // Draw left wall
    QRgb cleft = qRgb(0, 0, 255);
    for (int y = miny; y < maxy; y++) {
        pixels->setPixel(minx, y, cleft);
    }
    
    // Draw right wall
    QRgb cright = qRgb(0, 0, 255);
    for (int y = miny; y < maxy; y++) {
        pixels->setPixel(maxx, y, cright);
    }
    
    // Draw top wall
    QRgb ctop = qRgb(0, 0, 255);
    for (int x = minx; x < maxx; x++) {
        pixels->setPixel(x, miny, ctop);
    }
    
    // Draw bottom wall
    QRgb cbottom = qRgb(0, 0, 255);
    for (int x = minx; x < maxx; x++) {
        pixels->setPixel(x, maxy, cbottom);
    }
}