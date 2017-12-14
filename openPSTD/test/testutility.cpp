#include "testutility.h"

/**
 * Constructor.
 * 
 * @param window  A reference to the main window
 */
TestUtility::TestUtility(Window* window) {
    // Save reference to the main window locally
    this->window = window;
}

/**
 * Verifies that Utility::drawPixel() throws a runtime_error if
 * the given pixels array is a nullptr.
 */
void TestUtility::testDrawPixel1() {
    // Try to draw to a nullptr pixels array
    QVERIFY_EXCEPTION_THROWN(
        Utility::drawPixel(
            QPoint(0, 0),
            qRgb(255, 0, 0),
            nullptr
        ),
        std::runtime_error*
    );
}

/**
 * Verifies that Utility::drawPixel() works correctly.
 */
void TestUtility::testDrawPixel2() {
    // Create a pixels array to draw to
    QImage pixels(800, 600, QImage::Format_RGB32);
    
    // Draw a pixel
    Utility::drawPixel(
        QPoint(0, 0),
        qRgb(255, 0, 0),
        &pixels
    );
    
    // Verify that the pixel's color is updated correctly
    QVERIFY(pixels.pixel(0, 0) == qRgb(255, 0, 0));
}

/**
 * Verifies that Utility::drawLine() throws a runtime_error if
 * the given pixels array is a nullptr.
 */
void TestUtility::testDrawLine1() {
    // Try to draw to a nullptr pixels array
    QVERIFY_EXCEPTION_THROWN(
        Utility::drawLine(
            QPoint(0, 0),
            QPoint(10, 0),
            qRgb(255, 0, 0),
            nullptr
        ),
        std::runtime_error*
    );
}

/**
 * Verifies that Utility::drawLine() works correctly.
 */
void TestUtility::testDrawLine2() {
    // Create a pixels array to draw to
    QImage pixels(800, 600, QImage::Format_RGB32);
    
    // Draw a line
    Utility::drawLine(
        QPoint(0, 0),
        QPoint(10, 0),
        qRgb(255, 0, 0),
        &pixels
    );
    
    // Verify that the pixels' colors are updated correctly
    QVERIFY(pixels.pixel(5, 0) == qRgb(255, 0, 0));
}

/**
 * Verifies that Utility::drawRect() throws a runtime_error if
 * the given pixels array is a nullptr.
 */
void TestUtility::testDrawRect1() {
    // Try to draw to a nullptr pixels array
    QVERIFY_EXCEPTION_THROWN(
        Utility::drawRect(
            QRect(QPoint(0, 0), QPoint(10, 10)),
            qRgb(255, 0, 0),
            nullptr
        ),
        std::runtime_error*
    );
}

/**
 * Verifies that Utility::drawRect() works correctly.
 */
void TestUtility::testDrawRect2() {
    // Create a pixels array to draw to
    QImage pixels(800, 600, QImage::Format_RGB32);
    
    // Draw a rectangle
    Utility::drawRect(
        QRect(QPoint(0, 0), QPoint(10, 10)),
        qRgb(255, 0, 0),
        &pixels
    );
    
    // Verify that the pixels' colors are updated correctly
    QVERIFY(pixels.pixel(3, 3) == qRgb(255, 0, 0));
}

/**
 * Verifies that Utility::scr2obj() works correctly.
 */
void TestUtility::testScr2obj() {
    // Get a reference to the Model instance
    Model* model = Model::getInstance();
    
    // Set the zoom level and scene offset
    model->zoomlevel = 10;
    model->offset = QPoint(10, 20);
    
    // Test a point
    QPoint pos(20, 50);
    QPoint result = Utility::scr2obj(pos);
    
    // Verify that the output is correct
    QVERIFY(result.x() == 1);
    QVERIFY(result.y() == 3);
}

/**
 * Verifies that Utility::obj2scr() works correctly.
 */
void TestUtility::testObj2scr() {
    // Get a reference to the Model instance
    Model* model = Model::getInstance();
    
    // Set the zoom level and scene offset
    model->zoomlevel = 10;
    model->offset = QPoint(10, 20);
    
    // Test a point
    QPoint pos(1, 3);
    QPoint result = Utility::obj2scr(pos);
    
    // Verify that the output is correct
    QVERIFY(result.x() == 20);
    QVERIFY(result.y() == 50);
}