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
 * once the add domain button is clicked.
 */
void TestMainToolbar::addDomain() {
    // Click the add domain button
    window->ui->actionAddDomain->activate(QAction::Trigger);
    
    // Verify that the model's state was correctly updated
    QVERIFY(window->view->model->state == ADDDOMAIN);
}