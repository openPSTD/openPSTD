#include "sourcehandler.h"

/**
 * Constructor.
 * Intitializes the private reference variables.
 * 
 * @param window  A reference to the main window
 */
SourceHandler::SourceHandler(Point* mouse, QWidget* window) {
	// Initialize the class instance variables
	this->mouse = mouse;
	this->window = window;
}

// TODO
bool SourceHandler::isSelected(unsigned int id) {
	// TODO
	return false;
}

/**
 * Selects a source, given its ID.
 * Does nothing if an invalid ID is given.
 * 
 * @param id  The ID of the source to select
 */
void SourceHandler::selectSource(bool add) {
	// Deselect all sources if not adding to the selection
	if (!add) deselectAll();
	
	// Get the mouse coordinates
	int x = mouse->getObject().x();
	int y = mouse->getObject().y();
	
	// Loop through all sources
	Model* model = ModelManager::getInstance()->getCurrent();
	for (unsigned int i = 0; i < model->sources.size(); i++) {
		// Get the coordinates of this source
		int x1 = model->sources[i]->getX();
		int y1 = model->sources[i]->getY();
		
		// Check if this source was clicked
		bool xmatch = std::abs(x1 - x) < 5;
		bool ymatch = std::abs(y1 - y) < 5;
		if (xmatch && ymatch) {
			// Select this source
			model->sources[i]->toggleSelected();
		}
	}
}

/**
 * Deselects all sources.
 */
void SourceHandler::deselectAll() {
	// Loop through all sources
	Model* model = ModelManager::getInstance()->getCurrent();
	for (unsigned int i = 0; i < model->sources.size(); i++) {
		// Deselect this source
		model->sources[i]->setSelected(false);
	}
}

/**
 * Deletes all selected sources.
 */
bool SourceHandler::deleteSelected() {
	// Loop through all sources
	bool deleted = false;
	Model* model = ModelManager::getInstance()->getCurrent();
	for (unsigned int i = 0; i < model->sources.size(); i++) {
		// Do nothing if this source is not selected
		if (!model->sources[i]->getSelected()) continue;
		
		// Delete this source
		delete model->sources[i];
		model->sources.erase(model->sources.begin() + i);
		i--;
		deleted = true;
	}
	
	// Return whether or not a source has been deleted
	return deleted;
}

/**
 * Adds a new source.
 * Presents a popup for entering the coordinates of the new source,
 * and then adds a new source at those coordinates.
 */
void SourceHandler::addSource() {
	// Open a CoordinateDialog to request the coordinates for the new source
	CoordinateDialog* cd = new CoordinateDialog(window, true);
	cd->exec();
	
	// Do nothing if the CoordinateDialog was cancelled
	if (!cd->getSaved()) return;
	
	// Save the current ModelManager state
	ModelManager::getInstance()->saveState();
	
	// Add a new source at the entered coordinates
	QPoint coords = cd->getCoords();
	Model* model = ModelManager::getInstance()->getCurrent();
	Point* pos = new Point(coords.x(), coords.y(), OBJECT);
	model->sources.push_back(new Source(pos));
	
	// Delete the CoordinateDialog
	delete cd;
}

void SourceHandler::moveSources(QPoint deltaObject) {
	// Loop through all sources
	Model* model = ModelManager::getInstance()->getCurrent();
	for (unsigned int i = 0; i < model->sources.size(); i++) {
		// Do nothing if this source is not selected
		if (!model->sources[i]->getSelected()) continue;
		
		// Update the position of this source
		model->sources[i]->moveSource(deltaObject);
	}
}