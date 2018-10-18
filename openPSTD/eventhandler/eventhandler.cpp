#include "eventhandler.h"

/**
 * Constructor.
 * Initializes the reference variables.
 * 
 * @param window  A reference to the main window
 */
EventHandler::EventHandler(QWidget* window) {
	// Initialize reference variables
	this->window = window;
	
	// Initialize class instance variables
	mouse = new Point(0, 0, OBJECT);
	this->dh = new DomainHandler(mouse);
	this->sh = new SourceHandler(mouse, window);
	this->rh = new ReceiverHandler(mouse, window);
	this->mh = new MeasureHandler(mouse);
	
	// Set initial scene offset
	ModelManager::getInstance()->getCurrent()->offsetX = 0;
	ModelManager::getInstance()->getCurrent()->offsetY = 0;
	
	// Initialize state variables
	this->doubleclick = false;
	this->moved = false;
	p0 = new Point(0, 0, OBJECT);
	q0 = new Point(0, 0, OBJECT);
}

EventHandler::~EventHandler() {
	delete p0;
	delete q0;
	delete mouse;
}

/**
 * Mouse press event handler.
 * 
 * TODO.
 * @param x  The x-coordinate of the mouse
 * @param y  The y-coordinate of the mouse
 * @param button  The mouse button that was pressed
 * @param modifiers  The keyboard modifiers of the event
 */
/**
 * TODO:
 * Simulator
 * Select domains
 * Select objects
 * Add domain
 * Add source
 * Add receiver
 * Update change absorption button
 * Move objects
 * Move domains
 */
void EventHandler::mousePress(
	QPoint screen,
	Qt::MouseButton button,
	Qt::KeyboardModifiers modifiers
) {
	// Update the mouse position
	*mouse = Point(screen, SCREEN);
	
	// LMB + sMove = Move scene
	Model* model = ModelManager::getInstance()->getCurrent();
	if (button == Qt::LeftButton && model->state == MOVE && !model->simulating) {
		// Save the start coordinates
		*q0 = Point(model->offsetX, model->offsetY, OBJECT);
		s0 = screen;
	}
	
	// LMB + sMeasure = Measure
	if (button == Qt::LeftButton && model->state == MEASURE && !model->simulating) {
		// Delegate to MeasureHandler
		if (doubleclick) {
			mh->setP2(true);
			doubleclick = false;
		} else {
			mh->setP1();
		}
	}
	
	// LMB + sAddDomain = Add domain
	if (button == Qt::LeftButton && model->state == ADDDOMAIN && !model->simulating) {
		// Delegate to DomainHandler
		if (doubleclick) {
			dh->addP2(true);
			doubleclick = false;
		} else {
			dh->addP1();
		}
	}
	
	// LMB + sSelectDomain = Select domain
	if (button == Qt::LeftButton && model->state == SELECTDOMAIN && !model->simulating) {
		// Delegate to DomainHandler
		dh->selectDomain(modifiers == Qt::Modifier::CTRL);
		s0 = screen;
	}
	
	// LMB + sSelect = Select sources / receivers / walls
	if (button == Qt::LeftButton && model->state == SELECT) {
		// Delegate to SourceHandler
		sh->selectSource(modifiers == Qt::Modifier::CTRL);
		
		// Delegate to ReceiverHandler
		rh->selectReceiver(modifiers == Qt::Modifier::CTRL);
		
		// Delegate to DomainHandler
		dh->selectWall(modifiers == Qt::Modifier::CTRL);
		s0 = screen;
	}
}

/**
 * Mouse release event handler.
 * 
 * TODO.
 * @param x  The x-coordinate of the mouse
 * @param y  The y-coordinate of the mouse
 * @param button  The mouse button that was released
 */
void EventHandler::mouseRelease(QPoint screen, Qt::MouseButton button) {
	// Update the mouse position
	*mouse = Point(screen, SCREEN);
	
	// LMB + sMeasure = Measure
	Model* model = ModelManager::getInstance()->getCurrent();
	if (button == Qt::LeftButton && model->state == MEASURE && !model->simulating) {
		if (mouse->clampToGrid().getObject() == mh->getP1()) {
			doubleclick = true;
		} else {
			mh->setP2(true);
		}
	}
	
	// LMB + sAddDomain = Add domain
	if (button == Qt::LeftButton && model->state == ADDDOMAIN && !model->simulating) {
		if (mouse->clampToGrid().getObject() == dh->getP1()) {
			doubleclick = true;
		} else {
			dh->addP2(true);
		}
	}
	
	// LMB + sSelectDomain = Move domain
	if (button == Qt::LeftButton && model->state == SELECTDOMAIN && !model->simulating) {
		moved = false;
	}
	
	// LMB + sSelect = Move sources / receivers / walls
	if (button == Qt::LeftButton && model->state == SELECT && !model->simulating) {
		moved = false;
	}
}

/**
 * Mouse move event handler.
 * 
 * TODO
 * @param x  The x-coordinate of the mouse
 * @param y  The y-coordinate of the mouse
 * @param modifiers  The modifiers of the event
 */
void EventHandler::mouseMove(QPoint screen, Qt::KeyboardModifiers modifiers) {
	// Update the mouse position
	*mouse = Point(screen, SCREEN);
	
	// sMeasure = Measure
	Model* model = ModelManager::getInstance()->getCurrent();
	if (model->state == MEASURE && !model->simulating) {
		if (doubleclick) mh->setP2(false);
	}
	
	// sAddDomain = Add domain
	if (model->state == ADDDOMAIN && !model->simulating) {
		if (doubleclick) dh->addP2(false);
	}
}

// TODO
void EventHandler::mouseDrag(QPoint screen, Qt::KeyboardModifiers modifiers) {
	// Update the mouse position
	//mouse = Grid::mouse2object(QPoint(x, y), model);
	*mouse = Point(screen, SCREEN);
	
	// sMove = Move scene
	Model* model = ModelManager::getInstance()->getCurrent();
	if (model->state == MOVE && !model->simulating) {
		// Update the scene offset
		QPoint dscreen = screen - s0;
		model->offsetX = q0->getObject().x() + dscreen.x() / model->zoom;
		model->offsetY = q0->getObject().y() + dscreen.y() / model->zoom;
	}
	
	// sMeasure = Measure
	if (model->state == MEASURE && !model->simulating) {
		if (!doubleclick) {
			mh->setP2(false);
		}
	}
	
	// sAddDomain = Add domain
	if (model->state == ADDDOMAIN && !model->simulating) {
		if (!doubleclick) {
			dh->addP2(false);
		}
	}
	
	// sSelectDomain = Move domain
	if (model->state == SELECTDOMAIN && !model->simulating) {
		// Compute the move delta in object coordinates
		QPoint dscreen = screen - s0;
		QPoint dobject = dscreen / model->zoom; // TODO
		if (dobject.x() != 0 || dobject.y() != 0) {
			// Save the current ModelManager state if this is the first movement
			if (!moved) {
				ModelManager::getInstance()->saveState();
				moved = true;
			}
			
			// Move all selected domains
			dh->moveDomains(QPoint(dobject.x(), -dobject.y()));
			
			// Update the start position
			s0 += dobject * model->zoom;
		}
	}
	
	// sSelect = Move sources / receivers / walls
	if (model->state == SELECT && !model->simulating) {
		// Compute the move delta in object coordinates
		QPoint dscreen = screen - s0;
		QPoint dobject = QPoint(dscreen.x() / model->zoom, -dscreen.y() / model->zoom);
		if (dobject.x() != 0 || dobject.y() != 0) {
			// Save the current ModelManager state if this is the first movement
			if (!moved) {
				ModelManager::getInstance()->saveState();
				moved = true;
			}
			
			// Move all selected sources
			sh->moveSources(dobject);
			
			// Move all selected receivers
			rh->moveReceivers(dobject);
			
			// Move all selected walls
			dh->moveWalls(dobject);
			
			// Update the start position
			s0 += QPoint(dobject.x(), -dobject.y()) * model->zoom;
		}
	}
}

void EventHandler::changeAbsorption() {
	// Get the selected domain
	Domain* selectedDomain = nullptr;
	Model* model = ModelManager::getInstance()->getCurrent();
	for (unsigned int i = 0; i < model->domains.size(); i++) {
		// Check if this domain is selected
		if (model->domains[i]->getSelected()) {
			// Verify that there is not more than one selected domain
			if (selectedDomain != nullptr) return;
			
			// Set this as the selected domain
			selectedDomain = model->domains[i];
		}
	}
	
	// Verify that there is at least one selected domain
	if (selectedDomain == nullptr) return;
	
	// Save the current ModelManager state
	ModelManager::getInstance()->saveState();
	
	// Create an absorption dialog for this domain
	AbsorptionDialog ad(window, selectedDomain);
	ad.exec();
}

/**
 * Deletes all selected domains, sources, and receivers.
 */
void EventHandler::deleteSelected() {
	// Save the current ModelManager state
	ModelManager::getInstance()->saveState();
	
	// Delete all selected domains
	bool deleted = false;
	deleted |= dh->deleteSelected();
	
	// Delete all selected sources
	deleted |= sh->deleteSelected();
	
	// Delete all selected receivers
	deleted |= rh->deleteSelected();
	
	// Undo the last save if no domains, sources, or receivers were deleted
	if (!deleted) {
		ModelManager::getInstance()->undo(false);
	}
}

/**
 * Deselects all selected domains, sources, and receivers.
 */
void EventHandler::clearSelection() {
	// Deselect all domains and walls
	dh->deselectAll();
	
	// Deselect all sources
	sh->deselectAll();
	
	// Deselect all receivers
	rh->deselectAll();
}