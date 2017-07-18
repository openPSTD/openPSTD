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