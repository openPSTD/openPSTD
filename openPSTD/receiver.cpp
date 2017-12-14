#include "receiver.h"
#include "model.h"

/**
 * Constructor.
 * 
 * @param pos  The position of the receiver (object coordinates)
 */
Receiver::Receiver(QPoint pos) {
    // Save the position of the receiver locally
    this->pos = pos;
}

/**
 * Draws the receiver to the given pixels array.
 * 
 * @param pixels  The pixels array to draw to
 * @throws runtime_error  If pixels is a nullptr
 */
void Receiver::draw(QImage* pixels) {
    // Do nothing if pixels is a nullptr
    if (pixels == nullptr) {
        throw new std::runtime_error("Given pixels array reference is a nullptr.");
    }
    
    // Get a reference to the Model instance
    Model* model = Model::getInstance();
    
    // Convert the position to screen coordinates
    QPoint p = Utility::obj2scr(pos);
    
    // Draw a square representing the receiver
    int d = 2;
    Utility::drawRect(
        QRect(p - QPoint(d, d), p + QPoint(d, d)),
        Settings::getInstance()->receiverColor,
        pixels
    );
}

/**
 * Get method for the position of the receiver.
 * 
 * @return The position of the receiver (object coordinates)
 */
QPoint Receiver::getPos() {
    // Return the position of the receiver
    return pos;
}

/**
 * Set method for the position of the receiver.
 * 
 * @param pos  The position of the receiver (object coordinates)
 */
void Receiver::setPos(QPoint pos) {
    // Update the position of the receiver
    this->pos = pos;
}