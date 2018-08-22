#include "testmenuview.h"

/**
 * Constructor.
 * 
 * @param window  A reference to the main window
 */
TestMenuView::TestMenuView(Window* window) {
	// Save reference variables locally
	this->window = window;
}

/**
 * Verifies that the model's showFPS state is correctly
 * updated once the option is toggled.
 */
void TestMenuView::toggleFPS() {
	// Get the current state of the model's showFPS
	bool fpsShown = window->view->model->showFPS;
	
	// Toggle the fps action
	window->ui->actionFPS_counter->activate(QAction::Trigger);
	
	// Verify that the model's showFPS value has been updated
	QVERIFY(window->view->model->showFPS != fpsShown);
	
	// Toggle back
	window->ui->actionFPS_counter->activate(QAction::Trigger);
	
	// Verify that the model's showFPS value has been updated
	QVERIFY(window->view->model->showFPS == fpsShown);
}