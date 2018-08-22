#include "testaddsource.h"

/**
 * Constructor.
 * 
 * @param window  A reference to the main window
 */
TestAddSource::TestAddSource(Window* window) {
	// Save reference variables locally
	this->window = window;
}

/**
 * Verifies that the model is updated correctly
 * when adding a new source.
 */
void TestAddSource::addSource() {
	// Click the add source button
	window->ui->actionAddSource->activate(QAction::Trigger);
	
	// Add a new source
	unsigned int numsources = window->view->model->sources.size();
	QTest::mouseClick(window->view->viewport(), Qt::LeftButton, Qt::KeyboardModifiers(), QPoint(150, 200));
	
	// Verify that a new source is added
	QVERIFY(window->view->model->sources.size() == numsources + 1);
	
	// Verify that the new source has the correct position
	std::vector<Source> sources = window->view->model->sources;
	Source source = sources[sources.size() - 1];
	QVERIFY(source.getX() == 15);
	QVERIFY(source.getY() == 20);
}