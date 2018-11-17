#include "receiverhandler.h"

/**
 * Constructor.
 * Initializes the private reference variables.
 * 
 * @param window  A reference to the main window
 */
ReceiverHandler::ReceiverHandler(Point* mouse, QWidget* window) {
	// Initialize the class instance variables
	this->mouse = mouse;
	this->window = window;
}

// TODO
bool ReceiverHandler::isSelected(unsigned int id) {
	// Do nothing if this receiver does not exist
	Model* model = ModelManager::getInstance()->getCurrent();
	if (id >= model->receivers.size()) return false;
	
	// Return whether or not the receiver is selected
	return model->receivers[id]->getSelected();
}

/**
 * Selects a receiver, given its ID.
 * Does nothing if an invalid ID is given.
 * 
 * @param id  The ID of the receiver to select
 */
void ReceiverHandler::selectReceiver(bool add) {
	// Deselect all receivers if not adding to the selection
	if (!add) deselectAll();
	
	// Get the mouse coordinates
	int x = mouse->getObject().x();
	int y = mouse->getObject().y();
	
	// Loop through all receivers
	Model* model = ModelManager::getInstance()->getCurrent();
	for (unsigned int i = 0; i < model->receivers.size(); i++) {
		// Get the coordinates of this receiver
		int x1 = model->receivers[i]->getX();
		int y1 = model->receivers[i]->getY();
		
		// Check if this receiver was clicked
		bool xmatch = std::abs(x1 - x) < 5;
		bool ymatch = std::abs(y1 - y) < 5;
		if (xmatch && ymatch) {
			// Select this receiver
			model->receivers[i]->toggleSelected();
		}
	}
}

/**
 * Deselects all receivers.
 */
void ReceiverHandler::deselectAll() {
	// Loop through all receivers
	Model* model = ModelManager::getInstance()->getCurrent();
	for (unsigned int i = 0; i < model->receivers.size(); i++) {
		// Deselect this receiver
		model->receivers[i]->setSelected(false);
	}
}

/**
 * Deletes all selected receivers.
 */
bool ReceiverHandler::deleteSelected() {
	// Loop through all receivers
	bool deleted = false;
	Model* model = ModelManager::getInstance()->getCurrent();
	for (unsigned int i = 0; i < model->receivers.size(); i++) {
		// Do nothing if this receiver is not selected
		if (!model->receivers[i]->getSelected()) continue;
		
		// Delete this receiver
		delete model->receivers[i];
		model->receivers.erase(model->receivers.begin() + i);
		i--;
		deleted = true;
	}
	
	// Return whether or not a receiver has been deleted
	return deleted;
}

/**
 * Adds a new receiver.
 * Presents a popup for entering the coordinates of a new receiver,
 * and then adds a new receiver at those coordinates.
 */
void ReceiverHandler::addReceiver() {
	// Do nothing if the Simulator is running
	Model* model = ModelManager::getInstance()->getCurrent();
	if (model->simulating) return;
	
	// Open a CoordinateDialog to request the coordinates for the new receiver
	CoordinateDialog* cd = new CoordinateDialog(window, false);
	cd->exec();
	
	// Do nothing if the CoordinateDialog was cancelled
	if (!cd->getSaved()) return;
	
	// Save the current ModelManager state
	ModelManager::getInstance()->saveState();
	
	// Add a new receiver at the entered coordinates
	QPoint coords = cd->getCoords();
	Point* pos = new Point(coords.x(), coords.y(), OBJECT);
	model->receivers.push_back(new Receiver(pos));
	
	// Delete the CoordinateDialog
	delete cd;
}

void ReceiverHandler::moveReceivers(QPoint deltaObject) {
	// Loop through all receivers
	Model* model = ModelManager::getInstance()->getCurrent();
	for (unsigned int i = 0; i < model->receivers.size(); i++) {
		// Do nothing if this receiver is not selected
		if (!model->receivers[i]->getSelected()) continue;
		
		// Update the position of this receiver
		model->receivers[i]->moveReceiver(deltaObject);
	}
}