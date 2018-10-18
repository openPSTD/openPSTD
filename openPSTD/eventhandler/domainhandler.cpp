#include "domainhandler.h"

/**
 * Constructor.
 * Initializes the private reference variables.
 * 
 * @param mouse  A reference to the mouse position
 */
DomainHandler::DomainHandler(Point* mouse) {
	// Save reference variables locally
	this->mouse = mouse;
	
	// Initialize the state variables
	this->hasP1 = false;
	this->hasP2 = false;
}

/**
 * Returns a vector of all IDs of the walls of a domain that are selected,
 * given the domain's ID.
 * Returns an empty vector if the given ID does not exist.
 * 
 * @param domainID  The ID of the domain
 * @return  A vector of all IDs of the selected walls of the domain
 */
std::vector<unsigned int> DomainHandler::getSelectedWalls(unsigned int domainID) {
	// Initialize the result vector
	std::vector<unsigned int> result;
	
	// Verify that the given domainID exists
	Model* model = ModelManager::getInstance()->getCurrent();
	if (domainID >= model->domains.size()) return result;
	
	// Get a vector of all walls of this domain
	std::vector<Wall*>* walls = model->domains[domainID]->getWalls();
	
	// Loop through all walls
	for (unsigned int i = 0; i < walls->size(); i++) {
		// Check if this wall is selected
		if (walls->at(i)->getSelected()) {
			// Add this wall's ID to the result vector
			result.push_back(i);
		}
	}
	
	// Return the result vector
	return result;
}

/**
 * Registers the first corner of a new domain.
 */
void DomainHandler::addP1() {
	// Save the current ModelManager state
	ModelManager::getInstance()->saveState();
	
	// Register the first corner of a new domain
	this->p1 = mouse->clampToGrid().getObject();
	this->hasP1 = true;
	this->hasP2 = false;
}

/**
 * Registers the second corner of a new domain.
 */
void DomainHandler::addP2(bool final) {
	// Do nothing if P1 has not been set
	if (!hasP1) return;
	
	// Check if P2 has been set
	QPoint m = mouse->clampToGrid().getObject();
	Model* model = ModelManager::getInstance()->getCurrent();
	if (hasP2) {
		// Update the end coordinate of the last domain
		//model->domains.back()->setX1(m.x());
		//model->domains.back()->setY1(m.y());
		model->domains.back()->setP1(mouse->clampToGrid());
		model->domains.back()->resetWalls();
		
		if (final) {
			this->hasP1 = false;
			this->hasP2 = false;
		}
	} else {
		// Update state variables
		hasP2 = true;
		
		// Create a new domain
		model->domains.push_back(new Domain(
			new Point(p1, OBJECT),
			new Point(mouse->clampToGrid().getObject(), OBJECT)
		));
	}
	
	for (unsigned int i = 0; i < model->domains.size(); i++) {
		model->domains[i]->resetWalls();
	}
	for (unsigned int i = 0; i < model->domains.size(); i++) {
		model->domains[i]->mergeDomains(&(model->domains), i);
	}
}

/**
 * Cancels adding a new domain.
 */
void DomainHandler::cancelNewDomain() {
	// Delete the new domain if it was added
	/*Model* model = ModelManager::getInstance()->getCurrent();
	if (hasP2) {
		model->domains.pop_back();
	}*/
	if (!hasP1) return;
	ModelManager::getInstance()->undo(false);
	
	// Update the state variables
	this->hasP1 = false;
	this->hasP2 = false;
}

/**
 * Deletes all selected domains and walls.
 */
bool DomainHandler::deleteSelected() {
	// Loop through all domains
	bool deleted = false;
	Model* model = ModelManager::getInstance()->getCurrent();
	for (unsigned int i = 0; i < model->domains.size(); i++) {
		// Do nothing if this domain is not selected
		if (!model->domains[i]->getSelected()) continue;
		
		// Delete this domain
		delete model->domains[i];
		model->domains.erase(model->domains.begin() + i);
		i--;
		deleted = true;
	}
	
	// Re-merge all domains
	for (unsigned int i = 0; i < model->domains.size(); i++) {
		model->domains[i]->resetWalls();
	}
	for (unsigned int i = 0; i < model->domains.size(); i++) {
		model->domains[i]->mergeDomains(&(model->domains), i);
	}
	
	// Return whether or not a domain has been deleted
	return deleted;
}

/**
 * Deselects all selected domains and walls.
 */
void DomainHandler::deselectAll() {
	// Loop through all domains
	Model* model = ModelManager::getInstance()->getCurrent();
	for (unsigned int i = 0; i < model->domains.size(); i++) {
		// Deselect this domain, and its walls
		model->domains[i]->deselectAll();
	}
	
	// Disable the set absorption action
	model->actionChangeAbsorption->setEnabled(false);
}

void DomainHandler::selectDomain(bool add) {
	// Deselect all domains if not adding to the selection
	if (!add) deselectAll();
	
	// Get the mouse coordinates
	int x = mouse->getObject().x();
	int y = mouse->getObject().y();
	
	// Loop through all domains
	Model* model = ModelManager::getInstance()->getCurrent();
	for (unsigned int i = 0; i < model->domains.size(); i++) {
		// Get the coordinates of this domain
		int x1 = model->domains[i]->getMinX();
		int x2 = model->domains[i]->getMaxX();
		int y1 = model->domains[i]->getMinY();
		int y2 = model->domains[i]->getMaxY();
		
		// Check if this domain was clicked
		bool xmatch = (x1 <= x && x <= x2);
		bool ymatch = (y1 <= y && y <= y2);
		if (xmatch && ymatch) {
			// Select this domain
			model->domains[i]->toggleSelected();
		}
	}
	
	// Update the set absorption action
	updateWindowAction();
}

void DomainHandler::selectWall(bool add) {
	// Deselect all walls if not adding to the selection
	if (!add) deselectAll();
	
	// Get the mouse coordinates
	int x = mouse->getObject().x();
	int y = mouse->getObject().y();
	
	// Loop through all domains
	Model* model = ModelManager::getInstance()->getCurrent();
	for (unsigned int i = 0; i < model->domains.size(); i++) {
		// Loop through all walls of this domain
		std::vector<Wall*>* walls = model->domains[i]->getWalls();
		for (unsigned int j = 0; j < walls->size(); j++) {
			// Get the coordinates of this wall
			int x1 = walls->at(j)->getMinX();
			int x2 = walls->at(j)->getMaxX();
			int y1 = walls->at(j)->getMinY();
			int y2 = walls->at(j)->getMaxY();
			
			// Check if this wall was clicked
			bool xmatch = (x1 <= x && x <= x2);
			bool ymatch = (y1 <= y && y <= y2);
			if (xmatch && ymatch) {
				// Select this wall
				walls->at(j)->toggleSelected();
			}
		}
	}
}

void DomainHandler::moveDomains(QPoint deltaObject) {
	// Loop through all domains
	Model* model = ModelManager::getInstance()->getCurrent();
	for (unsigned int i = 0; i < model->domains.size(); i++) {
		// Do nothing if this domain is not selected
		if (!model->domains[i]->getSelected()) continue;
		
		// Update the position of this domain
		model->domains[i]->moveDomain(deltaObject);
	}
	
	// Re-merge all domains
	for (unsigned int i = 0; i < model->domains.size(); i++) {
		model->domains[i]->resetWalls();
	}
	for (unsigned int i = 0; i < model->domains.size(); i++) {
		model->domains[i]->mergeDomains(&(model->domains), i);
	}
}

void DomainHandler::moveWalls(QPoint deltaObject) {
	// Loop through all domains
	Model* model = ModelManager::getInstance()->getCurrent();
	for (unsigned int i = 0; i < model->domains.size(); i++) {
		// Loop through all walls of this domain
		std::vector<Wall*>* walls = model->domains[i]->getWalls();
		std::set<Side> toMove;
		for (unsigned int j = 0; j < walls->size(); j++) {
			// Do nothing if this wall is not selected
			if (!walls->at(j)->getSelected()) continue;
			
			// Move this wall
			// A set is used to prevent double-moving a wall
			// if two walls of the same side are both selected.
			toMove.insert(walls->at(j)->getSide());
		}
		
		// Move the selected walls of this domain
		model->domains[i]->moveWalls(toMove, deltaObject);
	}
	
	// Re-merge all domains
	for (unsigned int i = 0; i < model->domains.size(); i++) {
		model->domains[i]->resetWalls();
	}
	for (unsigned int i = 0; i < model->domains.size(); i++) {
		model->domains[i]->mergeDomains(&(model->domains), i);
	}
}

void DomainHandler::updateWindowAction() {
	// Count the number of selected domains
	unsigned int numSelected = 0;
	Model* model = ModelManager::getInstance()->getCurrent();
	for (unsigned int i = 0; i < model->domains.size(); i++) {
		// Check if this domain is selected
		bool selected = model->domains[i]->getSelected();
		if (selected) numSelected++;
	}
	
	// Update the set absorption action
	model->actionChangeAbsorption->setEnabled(numSelected == 1);
}