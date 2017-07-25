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
 * @param zoom  The current zoom level (as in model)
 */
void Domain::draw(QImage* pixels, int zoom) {
    // Get coordinates of the corners
    int minx = std::min(x0, x1) * zoom;
    int maxx = std::max(x0, x1) * zoom;
    int miny = std::min(y0, y1) * zoom;
    int maxy = std::max(y0, y1) * zoom;
    
    // Draw left wall line
    QRgb cleft = qRgb(0, 0, 255);
    for (int y = miny; y < maxy; y++) {
        pixels->setPixel(minx, y, cleft);
    }
    
    // Draw left wall text
    drawWallLength(minx, miny, minx, maxy, pixels, zoom, LEFT);
    
    // Draw right wall line
    QRgb cright = qRgb(0, 0, 255);
    for (int y = miny; y < maxy; y++) {
        pixels->setPixel(maxx, y, cright);
    }
    
    // Draw right wall text
    drawWallLength(maxx, miny, maxx, maxy, pixels, zoom, RIGHT);
    
    // Draw top wall line
    QRgb ctop = qRgb(0, 0, 255);
    for (int x = minx; x < maxx; x++) {
        pixels->setPixel(x, miny, ctop);
    }
    
    // Draw top wall text
    drawWallLength(minx, miny, maxx, miny, pixels, zoom, TOP);
    
    // Draw bottom wall line
    QRgb cbottom = qRgb(0, 0, 255);
    for (int x = minx; x < maxx; x++) {
        pixels->setPixel(x, maxy, cbottom);
    }
    
    // Draw bottom wall text
    drawWallLength(minx, maxy, maxx, maxy, pixels, zoom, BOTTOM);
}

/**
 * Draws the length of a wall given its end points.
 * 
 * @param x0  The x coordinate of the first end of the wall
 * @param y0  The y coordinate of the first end of the wall
 * @param x1  The x coordinate of the second end of the wall
 * @param y1  The y coordinate of the second end of the wall
 * @param pixels  The pixels array to draw to
 * @param zoom  The current zoom level (as in model)
 * @param side  The side of the wall to draw the text on
 */
void Domain::drawWallLength(int x0, int y0, int x1, int y1, QImage* pixels, int zoom, Side side) {
    // Ensure that the wall is orthogonal
    if (x0 != x1 && y0 != y1) {
        std::cerr << "Cannot draw non-orthogonal wall length text" << std::endl;
        return;
    }
    
    // Compute the length of the wall
    int length = std::abs((x1 - x0) + (y1 - y0)) / zoom;
    std::string lengthtext = std::to_string(length);
    
    // Compute the midpoint of the wall
    int midx = (x0 + x1) / 2;
    int midy = (y0 + y1) / 2;
    
    // Update the midpoint based on the given side
    if (side == LEFT) {
        midx -= 12 * lengthtext.size();
    }
    if (side == TOP) {
        midx -= 5 * lengthtext.size();
        midy -= 9;
    }
    if (side == BOTTOM) {
        midx -= 5 * lengthtext.size();
        midy += 9;
    }
    
    // Draw the wall length text
    drawText(
        lengthtext,
        midx,
        midy - 7,
        14,
        qRgb(0, 0, 0),
        pixels
    );
}

/**
 * Draws a string to the given pixels array.
 * 
 * @param text  The text to draw
 * @param x  The x position to draw at
 * @param y  The y position to draw at
 * @param size  The font size to draw with
 * @param color  The color to draw in
 * @param pixels  The pixels array to draw to
 */
void Domain::drawText(std::string text, int x, int y, int size, QRgb color, QImage* pixels) {
    QPainter p;
    p.begin(pixels);
    p.setPen(QPen(color));
    p.setFont(QFont("Monospace", size));
    p.drawText(x, y + size, QString(text.c_str()));
    p.end();
}