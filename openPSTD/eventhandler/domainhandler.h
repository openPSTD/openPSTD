#ifndef DOMAINHANDLER_H
#define DOMAINHANDLER_H

#include <QPoint>
#include <vector>
#include <set>
#include "../modelmanager.h"
#include "../grid.h"
#include "../point.h"

class DomainHandler {
public:
	// Constructor
	DomainHandler(Point* mouse);
	
	// Returns a vector of all IDs of the walls of a domain that are selected,
	// given the domain's ID
	std::vector<unsigned int> getSelectedWalls(unsigned int domainID);
	
	// Registers the first corner of a new domain
	void addP1();
	
	// Registers the second corner of a new domain
	void addP2(bool final);
	
	// Cancels adding a new domain
	void cancelNewDomain();
	
	// Deletes all selected domains
	bool deleteSelected();
	
	// Deselects all selected domains and walls
	void deselectAll();
	
	// Returns the first corner of the new domain
	QPoint getP1() { return p1; }
	
	// Selects a domain
	void selectDomain(bool add);
	
	// Selects a wall
	void selectWall(bool add);
	
	void moveDomains(QPoint deltaObject);
	void moveWalls(QPoint deltaObject);
private:
	// Reference to the mouse position
	Point* mouse;
	
	// State variables
	QPoint p1;
	bool hasP1;
	bool hasP2;
	
	void updateWindowAction();
};

#endif