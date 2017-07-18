#include "testmaintoolbar.h"

/**
 * Constructor.
 * 
 * @param window  The main window
 */
TestMainToolbar::TestMainToolbar(Window* window) {
    // Save reference variables locally
    this->window = window;
}

/**
 * Verifies that the model's state is correctly updated
 * once the select button is clicked.
 */
void TestMainToolbar::select() {
    // Click the select button
    window->ui->actionSelect->activate(QAction::Trigger);
    
    // Verify that the model's state was correctly updated
    QVERIFY(window->view->model->state == SELECT);
}

/**
 * Verifies that the model's state is correctly updated
 * once the move button is clicked.
 */
void TestMainToolbar::move() {
    // Click the move button
    window->ui->actionMoveScene->activate(QAction::Trigger);
    
    // Verify that the model's state was correctly updated
    QVERIFY(window->view->model->state == MOVE);
}

/**
 * Verifies that the model's state is correctly updated
 * once the add domain button is clicked.
 */
void TestMainToolbar::addDomain() {
    // Click the add domain button
    window->ui->actionAddDomain->activate(QAction::Trigger);
    
    // Verify that the model's state was correctly updated
    QVERIFY(window->view->model->state == ADDDOMAIN);
    
    // Draw a new domain by clicking and dragging the mouse
    unsigned int numdomains = window->view->model->domains.size();
    QTest::mousePress(window->view->viewport(), Qt::LeftButton, Qt::KeyboardModifiers(), QPoint(100, 100));
    QTest::mouseRelease(window->view->viewport(), Qt::LeftButton, Qt::KeyboardModifiers(), QPoint(200, 300));
    
    // Verify that a new domain has been added
    QVERIFY(window->view->model->domains.size() == numdomains + 1);
    
    // Draw a new domain by clicking twice
    numdomains = window->view->model->domains.size();
    QTest::mouseClick(window->view->viewport(), Qt::LeftButton, Qt::KeyboardModifiers(), QPoint(150, 150));
    QTest::mouseClick(window->view->viewport(), Qt::LeftButton, Qt::KeyboardModifiers(), QPoint(250, 200));
    
    // Verify that a new domain has been added
    QVERIFY(window->view->model->domains.size() == numdomains + 1);
}

/**
 * Verifies that the model's state is correctly updated
 * once the add source button is clicked.
 */
void TestMainToolbar::addSource() {
    // Click the add source button
    window->ui->actionAddSource->activate(QAction::Trigger);
    
    // Verify that the model's state was correctly updated
    QVERIFY(window->view->model->state == ADDSOURCE);
}

/**
 * Verifies that the model's state is correctly updated
 * once the add receiver button is clicked.
 */
void TestMainToolbar::addReceiver() {
    // Click the add receiver button
    window->ui->actionAddReceiver->activate(QAction::Trigger);
    
    // Verify that the model's state was correctly updated
    QVERIFY(window->view->model->state == ADDRECEIVER);
}