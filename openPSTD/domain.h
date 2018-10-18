#ifndef DOMAIN_H
#define DOMAIN_H

#include <QImage>
#include <QPen>
#include <QPainter>
#include <vector>
#include <set>
#include "wall.h"
#include "settings.h"
#include <iostream>

class Point;

/**
 * Representation of a single domain.
 */
class Domain {
public:
	// Constructor
	Domain(Point* p1, Point* p2);
	Domain* copy();

	// Drawing method
	void draw(QImage* pixels);

	// Get methods for the domain's corner coordinates
	inline Point* getP1() { return p1; }
	inline Point* getP2() { return p2; }
	int getMinX();
	int getMaxX();
	int getMinY();
	int getMaxY();
	Point* getMinXP();
	Point* getMaxXP();
	Point* getMinYP();
	Point* getMaxYP();

	// Set methods for the domain's corner coordinates
	void setP1(Point p1);
	void setP2(Point p2);
	void moveDomain(QPoint deltaObject);
	void moveWalls(std::set<Side> toMove, QPoint deltaObject);
	
	// Get method for the walls vector
	std::vector<Wall*>* getWalls() { return &walls; }

	// Merges this domain with all other domains
	void mergeDomains(std::vector<Domain*>* domains, unsigned int ownID);

	// Resets the domain's walls to be the original four non-merged walls
	void resetWalls();

	// Checks whether or not this domain overlaps with another domain
	bool overlaps(Domain* other);

	// Get method for the absorption of a wall given its side
	double* getAbsorption(Side side);
	bool* getEdgeLocalReacting(Side side);
	void updateAbsorption();
	void setAbsorption(Side side, double value);
	void setEdgeLocalReacting(Side side, bool e);
	
	// Deselects the domain, and all walls
	bool getSelected() { return selected; }
	inline void toggleSelected() { this->selected = !this->selected; }
	void deselectAll();
private:
	// Domain walls
	Point* p1;
	Point* p2;
	std::vector<Wall*> walls;
	
	// Absorption coefficients
	double absorptionTop;
	double absorptionBottom;
	double absorptionLeft;
	double absorptionRight;
	bool elrTop;
	bool elrBottom;
	bool elrLeft;
	bool elrRight;
	bool selectedTop;
	bool selectedBottom;
	bool selectedLeft;
	bool selectedRight;
	
	// Wether or not this domain is selected
	bool selected;
	
	// Method for updating the side of all walls
	void updateWallSides();
	
	// Updates a given wall according to a given intersecting segment
	void handleIntersection(Domain* parent, unsigned int wallID, std::pair<int, int> toMerge);
	
	void switchSelectedWalls(Side oldside, Side newside);
};

#endif