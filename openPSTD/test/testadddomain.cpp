#include "testadddomain.h"

/**
 * Constructor.
 * 
 * @param window  A reference to the main window
 */
TestAddDomain::TestAddDomain(Window* window) {
    // Save reference variables locally
    this->window = window;
}

/**
 * Verifies that the model is updated correctly
 * when adding a new domain from the bottom-left
 * corner to the top-right corner.
 */
void TestAddDomain::addQ1() {
    // Click the add domain button
    window->ui->actionAddDomain->activate(QAction::Trigger);
    
    // Draw a new domain by clicking and dragging the mouse
    unsigned int numdomains = window->view->model->domains.size();
    QTest::mousePress(window->view->viewport(), Qt::LeftButton, Qt::KeyboardModifiers(), QPoint(200, 200));
    QTest::mouseRelease(window->view->viewport(), Qt::LeftButton, Qt::KeyboardModifiers(), QPoint(300, 100));
    
    // Verify that a new domain has been added at the correct position
    QVERIFY(window->view->model->domains.size() == numdomains + 1);
    Domain domain = window->view->model->domains[numdomains];
    checkDomain(20, 20, 30, 10, domain);
    
    // Draw a new domain by clicking twice
    numdomains = window->view->model->domains.size();
    QTest::mouseClick(window->view->viewport(), Qt::LeftButton, Qt::KeyboardModifiers(), QPoint(500, 200));
    QTest::mouseClick(window->view->viewport(), Qt::LeftButton, Qt::KeyboardModifiers(), QPoint(600, 100));
    
    // Verify that a new domain has been added
    QVERIFY(window->view->model->domains.size() == numdomains + 1);
    domain = window->view->model->domains[numdomains];
    checkDomain(50, 20, 60, 10, domain);
}

/**
 * Verifies that the model is updated correctly
 * when accding a new domain from the bottom-right
 * corner to the top-left corner.
 */
void TestAddDomain::addQ2() {
    // Click the add domain button
    window->ui->actionAddDomain->activate(QAction::Trigger);
    
    // Draw a new domain by clicking and dragging the mouse
    unsigned int numdomains = window->view->model->domains.size();
    QTest::mousePress(window->view->viewport(), Qt::LeftButton, Qt::KeyboardModifiers(), QPoint(200, 200));
    QTest::mouseRelease(window->view->viewport(), Qt::LeftButton, Qt::KeyboardModifiers(), QPoint(100, 100));
    
    // Verify that a new domain has been added at the correct position
    QVERIFY(window->view->model->domains.size() == numdomains + 1);
    Domain domain = window->view->model->domains[numdomains];
    checkDomain(20, 20, 10, 10, domain);
    
    // Draw a new domain by clicking twice
    numdomains = window->view->model->domains.size();
    QTest::mouseClick(window->view->viewport(), Qt::LeftButton, Qt::KeyboardModifiers(), QPoint(500, 200));
    QTest::mouseClick(window->view->viewport(), Qt::LeftButton, Qt::KeyboardModifiers(), QPoint(400, 100));
    
    // Verify that a new domain has been added
    QVERIFY(window->view->model->domains.size() == numdomains + 1);
    domain = window->view->model->domains[numdomains];
    checkDomain(50, 20, 40, 10, domain);
}

/**
 * Verifies that the model is updated correctly
 * when accding a new domain from the top-right
 * corner to the bottom-left corner.
 */
void TestAddDomain::addQ3() {
    // Click the add domain button
    window->ui->actionAddDomain->activate(QAction::Trigger);
    
    // Draw a new domain by clicking and dragging the mouse
    unsigned int numdomains = window->view->model->domains.size();
    QTest::mousePress(window->view->viewport(), Qt::LeftButton, Qt::KeyboardModifiers(), QPoint(200, 200));
    QTest::mouseRelease(window->view->viewport(), Qt::LeftButton, Qt::KeyboardModifiers(), QPoint(100, 300));
    
    // Verify that a new domain has been added at the correct position
    QVERIFY(window->view->model->domains.size() == numdomains + 1);
    Domain domain = window->view->model->domains[numdomains];
    checkDomain(20, 20, 10, 30, domain);
    
    // Draw a new domain by clicking twice
    numdomains = window->view->model->domains.size();
    QTest::mouseClick(window->view->viewport(), Qt::LeftButton, Qt::KeyboardModifiers(), QPoint(500, 200));
    QTest::mouseClick(window->view->viewport(), Qt::LeftButton, Qt::KeyboardModifiers(), QPoint(400, 300));
    
    // Verify that a new domain has been added
    QVERIFY(window->view->model->domains.size() == numdomains + 1);
    domain = window->view->model->domains[numdomains];
    checkDomain(50, 20, 40, 30, domain);
}

/**
 * Verifies that the model is updated correctly
 * when accding a new domain from the top-left
 * corner to the bottom-right corner.
 */
void TestAddDomain::addQ4() {
    // Click the add domain button
    window->ui->actionAddDomain->activate(QAction::Trigger);
    
    // Draw a new domain by clicking and dragging the mouse
    unsigned int numdomains = window->view->model->domains.size();
    QTest::mousePress(window->view->viewport(), Qt::LeftButton, Qt::KeyboardModifiers(), QPoint(200, 200));
    QTest::mouseRelease(window->view->viewport(), Qt::LeftButton, Qt::KeyboardModifiers(), QPoint(300, 300));
    
    // Verify that a new domain has been added at the correct position
    QVERIFY(window->view->model->domains.size() == numdomains + 1);
    Domain domain = window->view->model->domains[numdomains];
    checkDomain(20, 20, 30, 30, domain);
    
    // Draw a new domain by clicking twice
    numdomains = window->view->model->domains.size();
    QTest::mouseClick(window->view->viewport(), Qt::LeftButton, Qt::KeyboardModifiers(), QPoint(500, 200));
    QTest::mouseClick(window->view->viewport(), Qt::LeftButton, Qt::KeyboardModifiers(), QPoint(600, 300));
    
    // Verify that a new domain has been added
    QVERIFY(window->view->model->domains.size() == numdomains + 1);
    domain = window->view->model->domains[numdomains];
    checkDomain(50, 20, 60, 30, domain);
}

/**
 * Verifies that a single domain is correct according to given corner
 * coordinates.
 * 
 * @param x0  The x coordinate of the first corner of the domain
 * @param y0  The y coordinate of the first corner of the domain
 * @param x1  The x coordinate of the second corner of the domain
 * @param y1  The y coordinate of the second corner of the domain
 * @param domain  The domain to test
 */
void TestAddDomain::checkDomain(int x0, int y0, int x1, int y1, Domain domain) {
    // Verify that the corner coordinates of the domain are correct
    QVERIFY(domain.getX0() == std::min(x0, x1));
    QVERIFY(domain.getY0() == std::min(y0, y1));
    QVERIFY(domain.getX1() == std::max(x0, x1));
    QVERIFY(domain.getY1() == std::max(y0, y1));
    
    // Loop through all walls in the domain
    std::vector<Wall> walls = domain.getWalls();
    QVERIFY(walls.size() == 4);
    int n = 0;
    for (unsigned int i = 0; i < walls.size(); i++) {
        // Get the end point coordinates of this wall
        int xx0 = walls[i].getX0();
        int xx1 = walls[i].getX1();
        int yy0 = walls[i].getY0();
        int yy1 = walls[i].getY1();
        Side s = walls[i].getSide();
        
        // Verify that the type of the wall is correct according to its coordinates
        if (xx0 == x0 && yy0 == y0 && xx1 == x0 && yy1 == y1) { n++; QVERIFY(s == LEFT); }
        if (xx0 == x1 && yy0 == y0 && xx1 == x1 && yy1 == y1) { n++; QVERIFY(s == RIGHT); }
        if (xx0 == x0 && yy0 == y0 && xx1 == x1 && yy1 == y0) { n++; QVERIFY(s == TOP); }
        if (xx0 == x0 && yy0 == y1 && xx1 == x1 && yy1 == y1) { n++; QVERIFY(s == BOTTOM); }
    }
    
    // Verify that the domain contains four walls with correct sides
    QVERIFY(n == 4);
}