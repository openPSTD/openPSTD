#ifndef RECEIVERHANDLER_H
#define RECEIVERHANDLER_H

#include <QPoint>
#include "../modelmanager.h"
#include "../coordinatedialog.h"
#include "../point.h"

class ReceiverHandler {
public:
	// Constructor
	ReceiverHandler(Point* mouse, QWidget* window);
	
	// Returns whether or not a receiver is selected, given its ID
	bool isSelected(unsigned int id);
	
	// Selects a receiver, given its ID
	void selectReceiver(bool add);
	
	// Deselects all receivers
	void deselectAll();
	
	// Deletes the selected receivers
	bool deleteSelected();
	
	// Adds a new receiver
	void addReceiver();
	
	void moveReceivers(QPoint deltaObject);
private:
	// Reference variables
	Point* mouse;
	QWidget* window;
};

#endif