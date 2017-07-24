#include "testmodel.h"

/**
 * Constructor.
 * 
 * @param window  A reference to the main window
 */
TestModel::TestModel(Window* window) {
    // Save reference variables locally
    this->window = window;
    this->model = window->view->model;
}

/**
 * Verifies that the clampGrid method is working
 * correctly.
 */
void TestModel::clampGrid() {
    // Set the grid size and zoom level
    model->gridsize = 5;
    model->zoom = 10;
    
    // Test a point that should be clamped
    int a = model->gridsize * model->zoom;
    int b = a - window->settings->clampDist / 2;
    QPoint clamped1 = Grid::clampGrid(b, b, model, window->settings);
    QVERIFY(clamped1.x() == a && clamped1.y() == a);
    
    // Test a point that should not be clamped
    b = a - window->settings->clampDist - 1;
    QPoint clamped2 = Grid::clampGrid(b, b, model, window->settings);
    QVERIFY(clamped2.x() == b && clamped2.y() == b);
}

/**
 * Verifies that the isOnGrid method is working
 * correctly.
 */
void TestModel::isOnGrid() {
    // Set the grid size and zoom level
    model->gridsize = 5;
    model->zoom = 10;
    
    // Test a point that is on the grid
    int a = model->gridsize * model->zoom;
    bool onGrid = Grid::isOnGrid(a, a, model);
    QVERIFY(onGrid == true);
    
    // Test a point that is not on the grid
    int b = a - 1;
    onGrid = Grid::isOnGrid(b, b, model);
    QVERIFY(onGrid == false);
}