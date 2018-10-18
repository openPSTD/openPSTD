#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H

#include <QPoint>
#include <vector>
#include "../modelmanager.h"
#include "../point.h"
#include "../absorptiondialog.h"
#include "domainhandler.h"
#include "sourcehandler.h"
#include "receiverhandler.h"
#include "measurehandler.h"

/**
 * Event:
 *   MouseButton
 *   Modifiers
 *   State
 * 
 * EventHandler:
 *   mousePress
 *   mouseRelease
 *   mouseDrag
 * 
 * DomainHandler:
 *   selectDomain
 *   deselectAll
 *   deleteSelected
 *   addDomain
 * 
 * SourceHandler:
 *   selectSource
 *   deselectAll
 *   deleteSelected
 *   addSource
 * 
 * ReceiverHandler:
 *   selectReceiver
 *   deselectAll
 *   deleteSelected
 *   addReceiver
 */
class EventHandler {
public:
	// Constructor, destructor
	EventHandler(QWidget* window);
	~EventHandler();
	
	// Event handling methods
	void mousePress(QPoint screen, Qt::MouseButton button, Qt::KeyboardModifiers modifiers);
	void mouseRelease(QPoint screen, Qt::MouseButton button);
	void mouseMove(QPoint screen, Qt::KeyboardModifiers modifiers);
	void mouseDrag(QPoint screen, Qt::KeyboardModifiers modifiers);
	void changeAbsorption();
	
	// Returns the current mouse position
	inline Point getMousePos() { return *mouse; }
	
	// Delegating methods
	inline std::vector<unsigned int> getSelectedWalls(unsigned int domainID) {
		return dh->getSelectedWalls(domainID);
	}
	inline void cancelNewDomain() {
		dh->cancelNewDomain();
		doubleclick = false;
	}
	inline bool isSourceSelected(unsigned int id) { return sh->isSelected(id); }
	inline bool isReceiverSelected(unsigned int id) { return rh->isSelected(id); }
	inline void selectReceiver(unsigned int id) { rh->selectReceiver(id); }
	inline void setWidth(unsigned int width) { this->width = width; }
	inline void setHeight(unsigned int height) { this->height = height; }
	inline void addSource() { sh->addSource(); }
	inline void addReceiver() { rh->addReceiver(); }
	inline void removeMeasure() { mh->remove(); }
	inline void drawMeasure(QImage* pixels) { mh->draw(pixels); }
	
	// Multi-delegating methods
	void deleteSelected();
	void clearSelection();
	
	void moveToCenter();
private:
	// Reference variables
	QWidget* window;
	
	// Class instance variables
	DomainHandler* dh;
	SourceHandler* sh;
	ReceiverHandler* rh;
	MeasureHandler* mh;
	
	// General-use variables (for adding domains, measuring, ...)
	Point* p0;
	Point* q0;
	QPoint s0;
	bool doubleclick;
	bool moved;
	QPoint moveDelta;
	
	// Current position of the mouse
	Point* mouse;
	
	// Screen dimensions
	unsigned int width;
	unsigned int height;
};

#endif