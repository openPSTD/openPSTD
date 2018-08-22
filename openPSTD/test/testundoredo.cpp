#include "testundoredo.h"

/**
 * Constructor.
 * 
 * @param window  A reference to the main window
 */
TestUndoRedo::TestUndoRedo(Window* window) {
	// Save reference variables locally
	this->window = window;
}

/**
 * Verifies that the undo and redo operations work correctly
 * on the add domain action.
 */
void TestUndoRedo::addDomain() {
	// Click the add domain button
	window->ui->actionAddDomain->activate(QAction::Trigger);
	
	// Draw a new domain by clicking and dragging the mouse
	unsigned int numdomains = window->view->model->domains.size();
	QTest::mousePress(window->view->viewport(), Qt::LeftButton, Qt::KeyboardModifiers(), QPoint(200, 200));
	QTest::mouseRelease(window->view->viewport(), Qt::LeftButton, Qt::KeyboardModifiers(), QPoint(300, 100));
	
	// Verify that a new domain has been added
	QVERIFY(window->view->model->domains.size() == numdomains + 1);
	
	// Undo the last operation
	QTest::keyClick(window->view, Qt::Key_Z, Qt::ControlModifier);
	
	// Verify that the add domain operation has been undone
	QVERIFY(window->view->model->domains.size() == numdomains);
	
	// Redo the undone operation
	QTest::keyClick(window->view, Qt::Key_Z, Qt::ControlModifier | Qt::ShiftModifier);
	
	// Verify that the add domain operation has been redone
	QVERIFY(window->view->model->domains.size() == numdomains + 1);
}

/**
 * Verifies that the undo and redo operations work correctly
 * on the add source action.
 */
void TestUndoRedo::addSource() {
	// Click the add source button
	window->ui->actionAddSource->activate(QAction::Trigger);
	
	// Add a new source
	unsigned int numsources = window->view->model->sources.size();
	QTest::mouseClick(window->view->viewport(), Qt::LeftButton, Qt::KeyboardModifiers(), QPoint(150, 200));
	
	// Verify that a new source is added
	QVERIFY(window->view->model->sources.size() == numsources + 1);
	
	// Undo the last operation
	QTest::keyClick(window->view, Qt::Key_Z, Qt::ControlModifier);
	
	// Verify that the add source operation has been undone
	QVERIFY(window->view->model->sources.size() == numsources);
	
	// Redo the undone operation
	QTest::keyClick(window->view, Qt::Key_Z, Qt::ControlModifier | Qt::ShiftModifier);
	
	// Verify that the add source operation has been redone
	QVERIFY(window->view->model->sources.size() == numsources + 1);
}

/**
 * Verifies that the undo and redo operations work correctly
 * on the add receiver action.
 */
void TestUndoRedo::addReceiver() {
	// Click the add receiver button
	window->ui->actionAddReceiver->activate(QAction::Trigger);
	
	// Add a new receiver
	unsigned int numreceivers = window->view->model->receivers.size();
	QTest::mouseClick(window->view->viewport(), Qt::LeftButton, Qt::KeyboardModifiers(), QPoint(150, 200));
	
	// Verify that a new receiver is added
	QVERIFY(window->view->model->receivers.size() == numreceivers + 1);
	
	// Undo the last operation
	QTest::keyClick(window->view, Qt::Key_Z, Qt::ControlModifier);
	
	// Verify that the add receiver operation has been undone
	QVERIFY(window->view->model->receivers.size() == numreceivers);
	
	// Redo the undone operation
	QTest::keyClick(window->view, Qt::Key_Z, Qt::ControlModifier | Qt::ShiftModifier);
	
	// Verify that the add receiver operation has been redone
	QVERIFY(window->view->model->receivers.size() == numreceivers + 1);
}