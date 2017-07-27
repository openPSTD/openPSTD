#include "wall.h"

/**
 * Constructor.
 * 
 * @param x0  The x coordinate of the first end of the wall
 * @param y0  The y coordinate of the first end of the wall
 * @param x1  The x coordinate of the second end of the wall
 * @param y1  The y coordinate of the second end of the wall
 * @param side  The side of the domain that this wall is on
 */
Wall::Wall(int x0, int y0, int x1, int y1, Side side) {
    // Save end point coordinates locally
    this->x0 = x0;
    this->y0 = y0;
    this->x1 = x1;
    this->y1 = y1;
    this->side = side;
}

/**
 * Draws the length of a wall given its end points.
 * 
 * @param pixels  The pixels array to draw to
 * @param zoom  The current zoom level (as in model)
 */
void Wall::draw(QImage* pixels, int zoom) {
    // Verify that the wall is orthogonal
    if (x0 != x1 && y0 != y1) {
        std::cerr << "Cannot draw non-orthogonal wall length text" << std::endl;
        return;
    }
    
    // Get the minimum and maximum x and y coordinates of the corners
    int minx = std::min(x0, x1) * zoom;
    int maxx = std::max(x0, x1) * zoom;
    int miny = std::min(y0, y1) * zoom;
    int maxy = std::max(y0, y1) * zoom;
    
    // Draw all points on the wall
    for (int i = minx; i <= maxx; i++) {
        for (int j = miny; j <= maxy; j++) {
            pixels->setPixel(i, j, qRgb(0, 0, 255));
        }
    }
    
    // Compute the length of the wall
    int length = std::abs((x1 - x0) + (y1 - y0));
    std::string lengthtext = std::to_string(length);
    
    // Compute the midpoint of the wall
    int midx = (minx + maxx) / 2;
    int midy = (miny + maxy) / 2;
    
    // Update midpoint based on the wall's side
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
void Wall::drawText(std::string text, int x, int y, int size, QRgb color, QImage* pixels) {
    QPainter p;
    p.begin(pixels);
    p.setPen(QPen(color));
    p.setFont(QFont("Monospace", size));
    p.drawText(x, y + size, QString(text.c_str()));
    p.end();
}