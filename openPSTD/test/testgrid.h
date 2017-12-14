#ifndef TESTGRID_H
#define TESTGRID_H

#include <QtTest/QtTest>
#include "window.h"
#include "grid.h"
#include "modelmanager.h"

/**
 * Tests the functionality of the methods in Grid.
 */
class TestGrid : public QObject {
    Q_OBJECT
public:
    TestGrid(Window* window);
private:
    Window* window;
private slots:
    // Tests for Grid::isOnGrid()
    void testIsOnGrid1();
    void testIsOnGrid2();
    
    // Tests for Grid::clamp()
    void testClamp1();
    void testClamp2();
    void testClamp3();
    void testClamp4();
    
    // Tests for Grid::clampGrid()
    void testClampGrid1();
    void testClampGrid2();
    void testClampGrid3();
    void testClampGrid4();
    
    // Tests for Grid::clampWalls()
    void testClampWalls1();
    void testClampWalls2();
    void testClampWalls3();
    void testClampWalls4();
};

#endif