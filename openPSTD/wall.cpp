#include "wall.h"

/**
 * Constructor.
 * 
 * @param line  A QLine representing the location of the wall
 * @param side  The side of the domain this wall is on
 */
Wall::Wall(QLine line, Side side) {
    // Initialize representation variables
    this->line = line;
    this->side = side;
    this->absorption = 0;
}

/**
 * Draws the wall to the given pixels array.
 * 
 * @param pixels  The pixels array to draw to
 * @throws runtime_error  If pixels is a nullptr
 */
void Wall::draw(QImage* pixels) {
    // Do nothing if pixels is a nullptr
    if (pixels == nullptr) {
        throw new std::runtime_error("Given pixels array reference is a nullptr.");
    }
    
    // Convert the position to screen coordinates
    QPoint p1 = Utility::obj2scr(line.p1());
    QPoint p2 = Utility::obj2scr(line.p2());
    
    // Calculate the color of the wall
    QRgb w0 = Settings::getInstance()->wallColor0;
    QRgb w1 = Settings::getInstance()->wallColor1;
    QRgb color = absorption * w0 + (1-absorption) * w1;
    
    // Draw a line representing the wall
    Utility::drawLine(
        p1,
        p2,
        color,
        pixels
    );
}

/**
 * Get method for the QLine of the wall.
 * 
 * @return  A pointer to the QLine of the wall
 */
QLine* Wall::getLine() {
    // Return a pointer to the QLine
    return &line;
}

/**
 * Get method for the side of the wall.
 * 
 * @return  The side of the wall
 */
Side Wall::getSide() {
    // Return the side of the wall
    return side;
}

/**
 * Returns whether or not the wall is horizontal.
 * 
 * @return  Whether or not the wall is horizontal
 */
bool Wall::isHorizontal() {
    // Return whether or not the wall is horizontal
    return (side == TOP || side == BOTTOM);
}

/**
 * Get method for the absorption of the wall.
 * 
 * @return  The absorption of the wall
 */
double Wall::getAbsorption() {
    // Return the absorption of the wall
    return absorption;
}

/**
 * Set method for the absorption of the wall.
 * 
 * @param absorption  The new absorption of the wall
 */
void Wall::setAbsorption(double absorption) {
    // Save the new absorption of the wall
    this->absorption = absorption;
}