#include "testsource.h"

/**
 * Constructor.
 * 
 * @param window  A reference to the main window
 */
TestSource::TestSource(Window* window) {
    // Save reference to the main window locally
    this->window = window;
}

/**
 * Verifies that Source::draw() throws a std::runtime_error
 * when the given pixels array is a nullptr.
 */
void TestSource::testDraw1() {
    // Create a new Source instance
    QPoint pos(0, 0);
    Source source(pos);
    
    // Draw to a nullptr
    QVERIFY_EXCEPTION_THROWN(
        source.draw(nullptr),
        std::runtime_error*
    );
}

/**
 * Verifies that Source::draw() works correctly.
 */
void TestSource::testDraw2() {
    // Set the zoom level and scene offset
    Model::getInstance()->zoomlevel = 10;
    Model::getInstance()->offset = QPoint(0, 0);
    
    // Create a new Source instance
    QPoint pos(0, 0);
    Source source(pos);
    
    // Draw to a pixel array
    QImage pixels(800, 600, QImage::Format_RGB32);
    source.draw(&pixels);
    
    // Verify that the source was drawn
    QVERIFY(pixels.pixel(0, 0) == Settings::getInstance()->sourceColor);
}

/**
 * Verifies that Source::getPos() works correctly.
 */
void TestSource::testGetPos() {
    // Create a new Source instance
    QPoint pos(0, 0);
    Source source(pos);
    
    // Get the position of the source
    QPoint p = source.getPos();
    
    // Verify that the output is correct
    QVERIFY(p.x() == pos.x());
    QVERIFY(p.y() == pos.y());
}

/**
 * Verifies that Source::setPos() works correctly.
 */
void TestSource::testSetPos() {
    // Create a new Source instance
    QPoint pos(0, 0);
    Source source(pos);
    
    // Give the source a new position
    QPoint pos2(1, 2);
    source.setPos(pos2);
    
    // Verify that the source's position was updated correctly
    QPoint p = source.getPos();
    QVERIFY(p.x() == pos2.x());
    QVERIFY(p.y() == pos2.y());
}