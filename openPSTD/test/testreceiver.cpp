#include "testreceiver.h"

/**
 * Constructor.
 * 
 * @param window  A reference to the main window
 */
TestReceiver::TestReceiver(Window* window) {
    // Save reference to the main window locally
    this->window = window;
}

/**
 * Verifies that Receiver::draw() throws a std::runtime_error
 * when the given pixels array is a nullptr.
 */
void TestReceiver::testDraw1() {
    // Create a new Receiver instance
    QPoint pos(0, 0);
    Receiver receiver(pos);
    
    // Draw to a nullptr
    QVERIFY_EXCEPTION_THROWN(
        receiver.draw(nullptr),
        std::runtime_error*
    );
}

/**
 * Verifies that Receiver::draw() works correctly.
 */
void TestReceiver::testDraw2() {
    // Set the zoom level and scene offset
    Model::getInstance()->zoomlevel = 10;
    Model::getInstance()->offset = QPoint(0, 0);
    
    // Create a new Receiver instance
    QPoint pos(0, 0);
    Receiver receiver(pos);
    
    // Draw to a pixel array
    QImage pixels(800, 600, QImage::Format_RGB32);
    receiver.draw(&pixels);
    
    // Verify that the receiver was drawn
    QVERIFY(pixels.pixel(0, 0) == Settings::getInstance()->receiverColor);
}

/**
 * Verifies that Receiver::getPos() works correctly.
 */
void TestReceiver::testGetPos() {
    // Create a new Receiver instance
    QPoint pos(0, 0);
    Receiver receiver(pos);
    
    // Get the position of the receiver
    QPoint p = receiver.getPos();
    
    // Verify that the output is correct
    QVERIFY(p.x() == pos.x());
    QVERIFY(p.y() == pos.y());
}

/**
 * Verifies that Receiver::setPos() works correctly.
 */
void TestReceiver::testSetPos() {
    // Create a new Receiver instance
    QPoint pos(0, 0);
    Receiver receiver(pos);
    
    // Give the receiver a new position
    QPoint pos2(1, 2);
    receiver.setPos(pos2);
    
    // Verify that the receiver's position was updated correctly
    QPoint p = receiver.getPos();
    QVERIFY(p.x() == pos2.x());
    QVERIFY(p.y() == pos2.y());
}