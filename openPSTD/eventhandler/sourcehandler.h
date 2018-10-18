#ifndef SOURCEHANDLER_H
#define SOURCEHANDLER_H

#include <QPoint>
#include "../modelmanager.h"
#include "../coordinatedialog.h"
#include "../point.h"

class SourceHandler {
public:
	// Constructor
	SourceHandler(Point* mouse, QWidget* window);
	
	// Returns whether or not a source is selected, given its ID
	bool isSelected(unsigned int id);
	
	// Selects a source, given its ID
	void selectSource(bool add);
	
	// Deselects all sources
	void deselectAll();
	
	// Deletes the selected sources
	bool deleteSelected();
	
	// Adds a new source
	void addSource();
	
	void moveSources(QPoint deltaObject);
private:
	// Reference variables
	Point* mouse;
	QWidget* window;
};

#endif