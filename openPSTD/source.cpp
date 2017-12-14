#include "source.h"
#include "model.h"

/**
 * Constructor.
 * 
 * @param pos  The position of the source (object coordinates)
 */
Source::Source(QPoint pos) {
    // Save the position of the source locally
    this->pos = pos;
}

/**
 * Draws the source to the given pixels array.
 * 
 * @param pixels  The pixels array to draw to
 * @throws runtime_error  If pixels is a nullptr
 */
void Source::draw(QImage* pixels) {
    // Do nothing if pixels is a nullptr
    if (pixels == nullptr) {
        throw new std::runtime_error("Given pixels array reference is a nullptr.");
    }
    
    // Get a reference to the Model instance
    Model* model = Model::getInstance();
    
    // Convert the position to screen coordinates
    QPoint p = Utility::obj2scr(pos);
    
    // Draw a square representing the source
    int d = 2;
    Utility::drawRect(
        QRect(p - QPoint(d, d), p + QPoint(d, d)),
        Settings::getInstance()->sourceColor,
        pixels
    );
}

/**
 * Get method for the position of the source.
 * 
 * @return  The position of the source (object coordinates)
 */
QPoint Source::getPos() {
    // Return the position of the source
    return pos;
}

/**
 * Set method for the position of the source.
 * 
 * @param pos  The position of the source (object coordinates)
 */
void Source::setPos(QPoint pos) {
    // Update the position of the source
    this->pos = pos;
}