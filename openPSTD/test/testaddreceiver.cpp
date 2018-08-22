#include "testaddreceiver.h"

/**
 * Constructor.
 * 
 * @param window  A reference to the main window
 */
TestAddReceiver::TestAddReceiver(Window* window) {
	// Save reference variables locally
	this->window = window;
}

/**
 * Verifies that the model is updated correctly
 * when adding a new receiver.
 */
void TestAddReceiver::addReceiver() {
	// Click the add receiver button
	window->ui->actionAddReceiver->activate(QAction::Trigger);
	
	// Add a new receiver
	unsigned int numreceivers = window->view->model->receivers.size();
	QTest::mouseClick(window->view->viewport(), Qt::LeftButton, Qt::KeyboardModifiers(), QPoint(150, 200));
	
	// Verify that a new receiver is added
	QVERIFY(window->view->model->receivers.size() == numreceivers + 1);
	
	// Verify that the new receiver has the correct position
	std::vector<Receiver> receivers = window->view->model->receivers;
	Receiver receiver = receivers[receivers.size() - 1];
	QVERIFY(receiver.getX() == 15);
	QVERIFY(receiver.getY() == 20);
}