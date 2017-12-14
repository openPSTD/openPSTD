#include "testwall.h"

/**
 * Constructor.
 * 
 * @param window  A reference to the main window
 */
TestWall::TestWall(Window* window) {
    // Save reference to the main window locally
    this->window = window;
}

/**
 * Verifies that Wall::draw() throws a std::runtime_error
 * when the given pixels array is a nullptr.
 */
void TestWall::testDraw1() {
    // Create a new Wall instance
    QLine line(QPoint(0, 0), QPoint(0, 20));
    Wall wall(line, LEFT);
    
    // Draw to a nullptr
    QVERIFY_EXCEPTION_THROWN(
        wall.draw(nullptr),
        std::runtime_error*
    );
}

/**
 * Verifies that Wall::draw() works correctly.
 */
void TestWall::testDraw2() {
    // Set the zoom level and scene offset
    Model::getInstance()->zoomlevel = 10;
    Model::getInstance()->offset = QPoint(0, 0);
    
    // Create a new Wall instance
    QLine line(QPoint(0, 0), QPoint(0, 20));
    Wall wall(line, LEFT);
    wall.setAbsorption(0);
    
    // Draw to a pixel array
    QImage pixels(800, 600, QImage::Format_RGB32);
    wall.draw(&pixels);
    
    // Verify that the wall was drawn
    QVERIFY(pixels.pixel(0, 0) == Settings::getInstance()->wallColor0);
}

/**
 * Verifies that Wall::getLine() works correctly.
 */
void TestWall::testGetLine() {
    // Create a new Wall instance
    QLine line(QPoint(0, 0), QPoint(0, 20));
    Wall wall(line, LEFT);
    
    // Get the QLine of the wall
    QLine* l = wall.getLine();
    
    // Verify that the output is correct
    QVERIFY(l->x1() == line.x1());
    QVERIFY(l->x2() == line.x2());
    QVERIFY(l->y1() == line.y1());
    QVERIFY(l->y2() == line.y2());
}

/**
 * Verifies that Wall::getSide() works correctly.
 */
void TestWall::testGetSide() {
    // Create a new Wall instance
    QLine line(QPoint(0, 0), QPoint(0, 20));
    Wall wall(line, LEFT);
    
    // Get the side of the wall
    Side side = wall.getSide();
    
    // Verify that the output is correct
    QVERIFY(side == LEFT);
}

/**
 * Verifies that Wall::isHorizontal() works correctly.
 */
void TestWall::testIsHorizontal() {
    // Create a new Wall instance
    QLine line(QPoint(0, 0), QPoint(0, 20));
    Wall wall(line, LEFT);
    
    // Check if the wall is horizontal
    bool h = wall.isHorizontal();
    
    // Verify that the output is correct
    QVERIFY(!h);
}

/**
 * Verifies that Wall::getAbsorption() works correctly.
 */
void TestWall::testGetAbsorption() {
    // Create a new Wall instance
    QLine line(QPoint(0, 0), QPoint(0, 20));
    Wall wall(line, LEFT);
    
    // Get the absorption of the wall
    double a = wall.getAbsorption();
    
    // Verify that the output is correct
    QVERIFY(a == 0);
}

/**
 * Verifies that Wall::setAbsorption() works correctly.
 */
void TestWall::testSetAbsorption() {
    // Create a new Wall instance
    QLine line(QPoint(0, 0), QPoint(0, 20));
    Wall wall(line, LEFT);
    
    // Set the absorption of the wall
    wall.setAbsorption(0.5);
    
    // Verify that the absorption was updated correctly
    double a = wall.getAbsorption();
    QVERIFY(a == 0.5);
}