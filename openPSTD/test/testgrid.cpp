#include "test/testgrid.h"

/**
 * Constructor.
 * 
 * @param window  A reference to the main window
 */
TestGrid::TestGrid(Window* window) {
    // Save reference to the main window locally
    this->window = window;
}

/**
 * Verifies that Grid::isOnGrid() returns false when the given point
 * is not on the grid.
 */
void TestGrid::testIsOnGrid1() {
    // Set the grid size and zoom level
    Model::getInstance()->gridsize = 10;
    Model::getInstance()->zoomlevel = 10;
    
    // Test a point that shouldn't be on the grid
    QPoint pos(10, 10);
    bool ongrid = Grid::isOnGrid(pos);
    
    // Verify that the output is correct
    QVERIFY(!ongrid);
}

/**
 * Verifies that Grid::isOnGrid() returns true when the given point
 * is on the grid.
 */
void TestGrid::testIsOnGrid2() {
    // Set the grid size and zoom level
    Model::getInstance()->gridsize = 10;
    Model::getInstance()->zoomlevel = 10;
    
    // Test a point that should be on the grid
    QPoint pos(0, 0);
    bool ongrid = Grid::isOnGrid(pos);
    
    // Verify that the output is correct
    QVERIFY(ongrid);
}

/**
 * Verifies that Grid::clamp() throws a std::runtime_error when
 * the given point is a nullptr.
 */
void TestGrid::testClamp1() {
    // Set the grid size and zoom level
    Model::getInstance()->gridsize = 10;
    Model::getInstance()->zoomlevel = 10;
    
    // Test a nullptr
    QVERIFY_EXCEPTION_THROWN(
        Grid::clamp(nullptr),
        std::runtime_error
    );
}

/**
 * Verifies that Grid::clamp() returns false when the given point
 * is too far away from any clamping element.
 * Verifies that the given point is not modified.
 */
void TestGrid::testClamp2() {
    // Set the grid size and zoom level
    Model::getInstance()->gridsize = 100;
    Model::getInstance()->zoomlevel = 10;
    
    // Test a point that shouldn't be clamped
    QPoint pos(200, 200);
    bool clamped = Grid::clamp(&pos);
    
    // Verify that the output is correct
    QVERIFY(!clamped);
    
    // Verify that the point was not modified
    QVERIFY(pos.x() == 200);
    QVERIFY(pos.y() == 200);
}

/**
 * Verifies that Grid::clamp() returns true when the given point
 * is close enough to (but not on) any clamping element.
 * Verifies that the given point is modified correctly.
 */
void TestGrid::testClamp3() {
    // Set the grid size and zoom level
    Model::getInstance()->gridsize = 10;
    Model::getInstance()->zoomlevel = 10;
    
    // Test a point that should be clamped
    QPoint pos(10, 10);
    bool clamped = Grid::clamp(&pos);
    
    // Verify that the output is correct
    QVERIFY(clamped);
    
    // Verify that the point was modified correctly
    QVERIFY(pos.x() == 0);
    QVERIFY(pos.y() == 0);
}

/**
 * Verifies that Grid::clamp() returns true when the given point
 * is on a clamping element.
 * Verifies that the given point is not modified.
 */
void TestGrid::testClamp4() {
    // Set the grid size and zoom level
    Model::getInstance()->gridsize = 10;
    Model::getInstance()->zoomlevel = 10;
    
    // Test a point that is already on the grid
    QPoint pos(0, 0);
    bool clamped = Grid::clamp(&pos);
    
    // Verify that the output is correct
    QVERIFY(clamped);
    
    // Verify that the point was not modified
    QVERIFY(pos.x() == 0);
    QVERIFY(pos.y() == 0);
}

/**
 * Verifies that Grid::clampGrid() throws a std::runtime_error
 * when the given point is a nullptr.
 */
void TestGrid::testClampGrid1() {
    // Set the grid size and zoom level
    Model::getInstance()->gridsize = 10;
    Model::getInstance()->zoomlevel = 10;
    
    // Test a nullptr
    QVERIFY_EXCEPTION_THROWN(
        Grid::clampGrid(nullptr),
        std::runtime_error
    );
}

/**
 * Verifies that Grid::clampGrid() returns false when the given
 * point is too far away from the grid.
 * Verifies that the given point is not modified.
 */
void TestGrid::testClampGrid2() {
    // Set the grid size and zoom level
    Model::getInstance()->gridsize = 100;
    Model::getInstance()->zoomlevel = 10;
    
    // Test a point that shouldn't be clamped
    QPoint pos(200, 200);
    bool clamped = Grid::clampGrid(&pos);
    
    // Verify that the output is correct
    QVERIFY(!clamped);
    
    // Verify that the point was not modified
    QVERIFY(pos.x() == 200);
    QVERIFY(pos.y() == 200);
}

/**
 * Verifies that Grid::clampGrid() returns true when the given
 * point is close enough to (but not on) the grid.
 * Verifies that the given point is modified correctly.
 */
void TestGrid::testClampGrid3() {
    // Set the grid size and zoom level
    Model::getInstance()->gridsize = 10;
    Model::getInstance()->zoomlevel = 10;
    
    // Test a point that should be clamped
    QPoint pos(10, 10);
    bool clamped = Grid::clampGrid(&pos);
    
    // Verify that the output is correct
    QVERIFY(clamped);
    
    // Verify that the point was modified correctly
    QVERIFY(pos.x() == 0);
    QVERIFY(pos.y() == 0);
}

/**
 * Verifies that Grid::clampGrid() returns true when the given
 * point is on the grid.
 * Verifies that the given point is modified correctly.
 */
void TestGrid::testClampGrid4() {
    // Set the grid size and zoom level
    Model::getInstance()->gridsize = 10;
    Model::getInstance()->zoomlevel = 10;
    
    // Test a point that is already on the grid
    QPoint pos(0, 0);
    bool clamped = Grid::clampGrid(&pos);
    
    // Verify that the output is correct
    QVERIFY(clamped);
    
    // Verify that the point was not modified
    QVERIFY(pos.x() == 0);
    QVERIFY(pos.y() == 0);
}

/**
 * Verifies that Grid::clampWalls() throws a std::runtime_error
 * when the given point is a nullptr.
 */
void TestGrid::testClampWalls1() {
    // Set the zoom level
    Model::getInstance()->zoomlevel = 10;
    
    // Remove all domains
    // TODO
    
    // Add a new domain
    // TODO
    
    // Test a nullptr
    QVERIFY_EXCEPTION_THROWN(
        Grid::clampWalls(nullptr),
        std::runtime_error
    );
}

/**
 * Verifies that Grid::clampWalls() returns false when the given
 * point is too far away from any domain wall.
 * Verifies that the given point is not modified.
 */
void TestGrid::testClampWalls2() {
    // Set the zoom level
    Model::getInstance()->zoomlevel = 10;
    
    // Remove all domains
    // TODO
    
    // Add a new domain
    // TODO
    
    // Test a point that shouldn't be clamped
    QPoint pos(200, 200);
    bool clamped = Grid::clampWalls(&pos);
    
    // Verify that the output is correct
    QVERIFY(!clamped);
    
    // Verify that the point was not modified
    QVERIFY(pos.x() == 200);
    QVERIFY(pos.y() == 200);
}

/**
 * Verifies that Grid::clampWalls() returns true when the given
 * point is close enough to (but not on) a domain wall.
 * Verifies that the given point is modified correctly.
 */
void TestGrid::testClampWalls3() {
    // Set the zoom level
    Model::getInstance()->zoomlevel = 10;
    
    // Remove all domains
    // TODO
    
    // Add a new domain
    // TODO
    
    // Test a point that should be clamped
    QPoint pos(1050, 1050);
    bool clamped = Grid::clampWalls(&pos);
    
    // Verify that the output is correct
    QVERIFY(clamped);
    
    // Verify that the point was modified correctly
    QVERIFY(pos.x() == 1000);
    QVERIFY(pos.y() == 1000);
}

/**
 * Verifies that Grid::clampWalls() returns true when the given
 * point is on a domain wall.
 * Verifies that the given point is modified correctly.
 */
void TestGrid::testClampWalls4() {
    // Set the zoom level
    Model::getInstance()->zoomlevel = 10;
    
    // Remove all domains
    // TODO
    
    // Add a new domain
    // TODO
    
    // Test a point that is already on a wall
    QPoint pos(1000, 1000);
    bool clamped = Grid::clampWalls(&pos);
    
    // Verify that the output is correct
    QVERIFY(clamped);
    
    // Verify that the point was not modified
    QVERIFY(pos.x() == 1000);
    QVERIFY(pos.y() == 1000);
}