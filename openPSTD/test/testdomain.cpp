#include "testdomain.h"

/**
 * Constructor.
 * 
 * @param window  A reference to the main window
 */
TestDomain::TestDomain(Window* window) {
    // Save reference to the main window locally
    this->window = window;
}

/**
 * Verifies that Domain::draw() throws a std::runtime_error
 * when the given pixels array is a nullptr.
 */
void TestDomain::testDraw1() {
    // Create a new Domain instance
    QRect rect(0, 0, 10, 10);
    Domain domain(rect);
    
    // Draw to a nullptr
    QVERIFY_EXCEPTION_THROWN(
        domain.draw(nullptr),
        std::runtime_error*
    );
}

/**
 * Verifies that Domain::draw() works correctly.
 */
void TestDomain::testDraw2() {
    // Set the zoom level and scene offset
    Model::getInstance()->zoomlevel = 10;
    Model::getInstance()->offset = QPoint(0, 0);
    
    // Create a new Wall instance
    QRect rect(0, 0, 10, 10);
    Domain domain(rect);
    
    // Draw to a pixel array
    QImage pixels(800, 600, QImage::Format_RGB32);
    domain.draw(&pixels);
    
    // Verify that the domain was drawn
    QVERIFY(pixels.pixel(10, 5) == Settings::getInstance()->wallColor0);
}

/**
 * Verifies that Domain::getRect() works correctly.
 */
void TestDomain::testGetRect() {
    // Create a new Domain instance
    QRect rect(0, 0, 10, 10);
    Domain domain(rect);
    
    // Get the QRect of the domain
    QRect r = domain.getRect();
    
    // Verify that the output is correct
    QVERIFY(r.top() == rect.top());
    QVERIFY(r.bottom() == rect.bottom());
    QVERIFY(r.left() == rect.left());
    QVERIFY(r.right() == rect.right());
}

/**
 * Verifies that Domain::getWalls() works correctly.
 */
void TestDomain::testGetWalls() {
    // Remove all old domains
    Model::getInstance()->domains.clear();
    
    // Create a new Domain instance
    QRect rect(0, 0, 10, 10);
    Domain domain(rect);
    
    // Get the walls vector of the domain
    std::vector<Wall>* walls = domain.getWalls();
    
    // Verify that walls is not a nullptr
    QVERIFY(walls != nullptr);
    
    // Verify that the domain has the correct number of walls
    QVERIFY(walls->size() == 4);
}