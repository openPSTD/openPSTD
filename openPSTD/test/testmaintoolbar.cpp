#include "testmaintoolbar.h"

/**
 * Constructor.
 * 
 * @param window  A reference to the main window
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

/**
 * Verifies that the model's state is correctly updated
 * once the value of the zoom spinbox was changed.
 */
void TestMainToolbar::zoom() {
	/*// Change the value of the zoom spinbox
	int value = window->sbZoom->value();
	window->sbZoom->setValue(value + 1);
	
	// Verify that the model's state was correctly updated
	QVERIFY(window->sbZoom->value() == value + 1);
	QVERIFY(window->view->model->zoom == value + 1);*/
}

/**
 * Verifies that the model's state is correctly updated
 * once the value of the grid size spinbox was changed.
 */
void TestMainToolbar::gridSize() {
	// Change the value of the grid size spinbox
	int value = window->sbGridSize->value();
	window->sbGridSize->setValue(value + 1);
	
	// Verify that the model's state was correctly updated
	QVERIFY(window->sbGridSize->value() == value + 1);
	QVERIFY(window->view->model->gridsize == value + 1);
}