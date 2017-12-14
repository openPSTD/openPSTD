#include "utility.h"

/**
 * Draws a single pixel to a given pixels array.
 * 
 * @param pos  The pixel to draw (screen coordinates)
 * @param color  The color to draw the pixel
 * @param pixels  A pixels array to draw to
 * @throws runtime_error  If pixels is a nullptr
 */
void Utility::drawPixel(QPoint pos, QRgb color, QImage* pixels) {
    // Verify that pixels is not a nullptr
    if (pixels == nullptr) {
        throw new std::runtime_error("Given pixels array reference is a nullptr.");
    }
    
    // Verify that the pixel is visible
    if (pos.x() < 0 || pos.y() < 0) return;
    if (pos.x() >= pixels->width() || pos.y() >= pixels->height()) return;
    
    // Draw the pixel
    pixels->setPixel(pos, color);
}

/**
 * Draws a line to a given pixels array.
 * Note: The line has to be horizontal or vertical
 * 
 * @param p1  The first point of the line
 * @param p2  The second point of the line
 * @param color  The color to draw the line
 * @param pixels  A pixels array to draw to
 * @throws runtime_error  If pixels is a nullptr
 * @throws runtime_error  If the line is not horizontal or vertical
 */
void Utility::drawLine(QPoint p1, QPoint p2, QRgb color, QImage* pixels) {
    // Verify that pixels is not a nullptr
    if (pixels == nullptr) {
        throw new std::runtime_error("Given pixels array reference is a nullptr.");
    }
    
    // Check if the line is horizontal
    if (p1.y() == p2.y()) {
        // Loop through all x-coordinates
        int minx = std::min(p1.x(), p2.x());
        int maxx = std::max(p1.x(), p2.x());
        for (int i = minx; i <= maxx; i++) {
            // Draw this pixel
            drawPixel(QPoint(i, p1.y()), color, pixels);
        }
        return;
    }
    
    // Check if the line is vertical
    if (p1.x() == p2.x()) {
        // Loop through all y-coordinates
        int miny = std::min(p1.y(), p2.y());
        int maxy = std::max(p1.y(), p2.y());
        for (int i = miny; i <= maxy; i++) {
            // Draw this pixel
            drawPixel(QPoint(p1.x(), i), color, pixels);
        }
        return;
    }
    
    // Line was not horizontal or vertical
    throw new std::runtime_error("Cannot draw a non-orthogonal line");
}

/**
 * Draws a rectangle to a given pixels array.
 * 
 * @param rect  The rectangle to draw (screen coordinates)
 * @param color  The color to draw the rectangle
 * @param pixels  A pixels array to draw to
 * @throws runtime_error  If pixels is a nullptr
 */
void Utility::drawRect(QRect rect, QRgb color, QImage* pixels) {
    // Verify that pixels is not a nullptr
    if (pixels == nullptr) {
        throw new std::runtime_error("Given pixels array reference is a nullptr.");
    }
    
    // Loop through all pixels to draw
    for (int x = rect.left(); x < rect.right(); x++) {
        for (int y = rect.top(); y < rect.bottom(); y++) {
            // Draw this pixel
            drawPixel(QPoint(x, y), color, pixels);
        }
    }
}

/**
 * Converts a point in screen coordinates to object coordinates.
 * 
 * @param pos  A point in screen coordinates
 * @return  The corresponding point in object coordinates
 */
QPoint Utility::scr2obj(QPoint pos) {
    // Create a result QPoint
    QPoint result = pos;
    
    // Undo the scene offset
    result -= Model::getInstance()->offset;
    
    // Undo the zoom level
    result /= Model::getInstance()->zoomlevel;
    
    // Return the result
    return result;
}

/**
 * Converts a point in object coordinates to screen coordinates.
 * 
 * @param pos  A point in object coordinates
 * @return  The corresponding point in screen coordinates
 */
QPoint Utility::obj2scr(QPoint pos) {
    // Create a result QPoint
    QPoint result = pos;
    
    // Apply the zoom level
    result *= (int) Model::getInstance()->zoomlevel;
    
    // Apply the scene offset
    result += Model::getInstance()->offset;
    
    // Return the result
    return result;
}