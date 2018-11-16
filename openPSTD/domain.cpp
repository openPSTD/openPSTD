#include "domain.h"

#include "point.h"

/**
 * Constructor.
 * 
 * @param x0  The x coordinate of the first corner of the domain
 * @param y0  The y coordinate of the first corner of the domain
 * @param x1  The x coordinate of the second corner of the domain
 * @param y1  The y coordinate of the second corner of the domain
 * @param settings  A reference to a Settings instance
 */
Domain::Domain(Point* p1, Point* p2) {
	// Save the domain's corner coordinates
	this->p1 = p1;
	this->p2 = p2;

	// Initialize the domain's absorption coefficients
	absorptionTop = 0;
	absorptionBottom = 0;
	absorptionLeft = 0;
	absorptionRight = 0;
	elrTop = false;
	elrBottom = false;
	elrLeft = false;
	elrRight = false;
	selected = false;
	selectedLeft = false;
	selectedRight = false;
	selectedTop = false;
	selectedBottom = false;
	
	// Initialize the walls vector
	resetWalls();
}

// TODO: Method contract.
Domain* Domain::copy() {
	Point* pp1 = new Point(p1->getObject(), OBJECT);
	Point* pp2 = new Point(p2->getObject(), OBJECT);
	Domain* c = new Domain(pp1, pp2);
	c->setAbsorption(LEFT, absorptionLeft);
	c->setAbsorption(RIGHT, absorptionRight);
	c->setAbsorption(TOP, absorptionTop);
	c->setAbsorption(BOTTOM, absorptionBottom);
	c->setEdgeLocalReacting(LEFT, elrLeft);
	c->setEdgeLocalReacting(RIGHT, elrRight);
	c->setEdgeLocalReacting(TOP, elrTop);
	c->setEdgeLocalReacting(BOTTOM, elrBottom);
	return c;
}

/**
 * Drawing method.
 * Draws the domain to a given QImage
 * 
 * @param pixels  A reference te the QImage to draw to
 * @param zoom  The current zoom level (as in model)
 * @param offsetX  The current x offset of the scene
 * @param offsetY  The current y offset of the scene
 * @param selectedWalls  A vector of indices in the walls vector of all selected walls
 */
void Domain::draw(QImage* pixels) {
	// Update the sides of all walls
	updateWallSides();

	// Draw all walls
	for (unsigned int i = 0; i < walls.size(); i++) {
		walls[i]->draw(pixels, selected);
	}
	
	// Overlay dashed walls if domain is selected
	if (selected) {
		// Get the corner coordinates of the domain
		int x1 = std::min(p1->getScreen().x(), p2->getScreen().x());
		int x2 = std::max(p1->getScreen().x(), p2->getScreen().x());
		int y1 = std::min(p1->getScreen().y(), p2->getScreen().y());
		int y2 = std::max(p1->getScreen().y(), p2->getScreen().y());
		
		// Draw a dashed line for the left wall
		for (int y = y1; y < y2; y++) {
			if ((y/10) % 2 == 0) {
				for (int d = -1; d <= 1; d++) {
					pixels->setPixel(x1 + d, y, qRgb(0, 128, 255));
				}
			}
		}
		
		// Draw a dashed line for the right wall
		for (int y = y1; y < y2; y++) {
			if ((y/10) % 2 == 0) {
				for (int d = -1; d <= 1; d++) {
					pixels->setPixel(x2 + d, y, qRgb(0, 128, 255));
				}
			}
		}
		
		// Draw a dashed line for the top wall
		for (int x = x1; x < x2; x++) {
			if ((x/10) % 2 == 0) {
				for (int d = -1; d <= 1; d++) {
					pixels->setPixel(x, y1 + d, qRgb(0, 128, 255));
				}
			}
		}
		
		// Draw a dashed line for the bottom wall
		for (int x = x1; x < x2; x++) {
			if ((x/10) % 2 == 0) {
				for (int d = -1; d <= 1; d++) {
					pixels->setPixel(x, y2 + d, qRgb(0, 128, 255));
				}
			}
		}
	}
}

/**
 * Checks if any of the walls in this domain need to be merged with any of
 * the walls in any other domain, and performs the merge if needed.
 * 
 * @param domains  A pointer to the domains vector
 * @param ownID  The index in the domains vector of this domain
 */
void Domain::mergeDomains(std::vector<Domain*>* domains, unsigned int ownID) {
	// Loop through all walls in this domain
	for (unsigned int i = 0; i < walls.size(); i++) {
		// Loop through all domains
		for (unsigned int j = 0; j < domains->size(); j++) {
			// Do not merge with ourselves
			if (j == ownID) continue;

			// Loop through all walls in the domain
			for (unsigned int k = 0; k < domains->at(j)->getWalls()->size(); k++) {
				Wall* wallk = domains->at(j)->getWalls()->at(k);

				// Merge with our wall
				std::pair<int, int> toMerge;
				bool merge = Wall::mergeWalls(*(walls[i]), *wallk, &toMerge);
				if (merge) {
					// The walls intersect, update both walls according to the intersection
					handleIntersection(this, i, toMerge);
					handleIntersection(domains->at(j), k, toMerge);
				}
			}
		}
	}
}

/**
 * Resets the domain's walls to be the original four non-merged walls.
 */
void Domain::resetWalls() {
	// Remove all old walls from the walls vector
	for (unsigned int i = 0; i < walls.size(); i++) {
		delete walls[i];
	}
	walls.clear();
	
	Point tl(getMinX(), getMaxY(), OBJECT);
	Point bl(getMinX(), getMinY(), OBJECT);
	Point tr(getMaxX(), getMaxY(), OBJECT);
	Point br(getMaxX(), getMinY(), OBJECT);
	
	// Recreate the original four non-merged walls
	walls.push_back(new Wall(tl, bl, LEFT, &absorptionLeft, &elrLeft, &selectedLeft));
	walls.push_back(new Wall(tr, br, RIGHT, &absorptionRight, &elrRight, &selectedRight));
	walls.push_back(new Wall(tl, tr, TOP, &absorptionTop, &elrTop, &selectedTop));
	walls.push_back(new Wall(bl, br, BOTTOM, &absorptionBottom, &elrBottom, &selectedBottom));
}

/**
 * Checks whether or not this domain overlaps with a given other domain.
 * 
 * @param other  The domain to check overlap against
 * @return  Whether or not there is overlap between the two domains
 */
bool Domain::overlaps(Domain* other) {
	// Get corner coordinates of this domain
	int x0a = getMinX();
	int y0a = getMinY();
	int x1a = getMaxX();
	int y1a = getMaxY();

	// Get corner coordinates of other domain
	int x0b = other->getMinX();
	int y0b = other->getMinY();
	int x1b = other->getMaxX();
	int y1b = other->getMaxY();

	// Check if the domains overlap in the x direction
	bool overlapxa = ((x0a < x0b && x0b < x1a) || (x0a < x1b && x1b < x1a));
	bool overlapxb = ((x0b < x0a && x0a < x1b) || (x0b < x1a && x1a < x1b));
	bool overlapx = overlapxa || overlapxb;

	// Check if the domains overlap in the y direction
	bool overlapya = ((y0a < y0b && y0b < y1a) || (y0a < y1b && y1b < y1a));
	bool overlapyb = ((y0b < y0a && y0a < y1b) || (y0b < y1a && y1a < y1b));
	bool overlapy = overlapya || overlapyb;

	// Return whether or not the two domains overlap
	return (overlapx && overlapy);
}

/**
 * Deselects the domain, and all walls.
 */
void Domain::deselectAll() {
	// Deselect this domain
	selected = false;
	
	// Loop through all walls
	for (unsigned int i = 0; i < walls.size(); i++) {
		// Deselect this wall
		walls[i]->setSelected(false);
	}
}

/**
 * Updates the side of all walls.
 */
void Domain::updateWallSides() {
	// Compute the minimum and maximum x and y coordinates among all walls
	int minx = -1;
	int maxx = -1;
	int miny = -1;
	int maxy = -1;
	for (unsigned int i = 0; i < walls.size(); i++) {
		int x0 = walls[i]->getMinX();
		int x1 = walls[i]->getMaxX();
		int y0 = walls[i]->getMinY();
		int y1 = walls[i]->getMaxY();

		if (minx == -1 || std::min(x0, x1) < minx) minx = std::min(x0, x1);
		if (maxx == -1 || std::max(x0, x1) > maxx) maxx = std::max(x0, x1);
		if (miny == -1 || std::min(y0, y1) < miny) miny = std::min(y0, y1);
		if (maxy == -1 || std::max(y0, y1) > maxy) maxy = std::max(y0, y1);
	}

	// Compute the midpoint of this domain
	double midx = static_cast<double>(minx + maxx) / 2;
	double midy = static_cast<double>(miny + maxy) / 2;

	// Loop through all walls
	for (unsigned int i = 0; i < walls.size(); i++) {
		// Get the corner coordinates of this wall
		int x0 = walls[i]->getMinX();
		int x1 = walls[i]->getMaxX();
		int y0 = walls[i]->getMinY();
		int y1 = walls[i]->getMaxY();

		// Check if this is a vertical wall
		if (x0 == x1 && y0 != y1) {
			// Set side according to position relative to the midpoint
			if (x0 < midx) walls[i]->setSide(LEFT);
			if (x0 > midx) walls[i]->setSide(RIGHT);
		}

		// Check if this is a horizontal wall
		if (x0 != x1 && y0 == y1) {
			// Set side according to position relative to the midpoint
			if (y0 > midy) walls[i]->setSide(TOP);
			if (y0 < midy) walls[i]->setSide(BOTTOM);
		}
	}
}

/**
 * Updates a given wall according to a given intersecting segment.
 * The algorithm implemented in this method is documented
 * in docs/Wall intersection.docx.
 * 
 * @param parent  The parent domain of the wall to update
 * @param wallID  The index of the wall in the walls vector of the parent
 * @param toMerge  The segment to merge
 */
void Domain::handleIntersection(
	Domain* parent,
	unsigned int wallID,
	std::pair<int, int> toMerge
) {
	// Check if the wall is horizontal or vertical
	Wall* wall = parent->getWalls()->at(wallID);
	if (wall->getSide() == LEFT || wall->getSide() == RIGHT) {
		// Get the end points of the wall and merge segment
		int p1 = wall->getMinY();
		int p2 = wall->getMaxY();
		int q1 = toMerge.first;
		int q2 = toMerge.second;
		
		// Sort the points on increasing coordinate
		//   Note that p1 and q1 are both at most p2
		//   and q2.
		int a = std::min(p1, q1);
		int b = std::max(p1, q1);
		int c = std::min(p2, q2);
		int d = std::max(p2, q2);
		
		// Save some variables from the current wall
		std::vector<Wall*>* walls = parent->getWalls();
		int x = walls->at(wallID)->getP1()->getObject().x();
		Side side = walls->at(wallID)->getSide();
		
		// Remove the current wall
		walls->erase(walls->begin() + wallID);
		
		// Add two new walls from a to b and from c to d
		//   Note that the overlapping segment is always
		//   from b to c.
		if (a != b) {
			bool* selected = nullptr;
			if (side == LEFT) selected = &selectedLeft;
			if (side == RIGHT) selected = &selectedRight;
			if (side == TOP) selected = &selectedTop;
			if (side == BOTTOM) selected = &selectedBottom;
			Wall* ab = new Wall(
				Point(x, a, OBJECT),
				Point(x, b, OBJECT),
				side,
				parent->getAbsorption(side),
				parent->getEdgeLocalReacting(side),
				selected
			);
			walls->push_back(ab);
		}
		if (c != d) {
			bool* selected = nullptr;
			if (side == LEFT) selected = &selectedLeft;
			if (side == RIGHT) selected = &selectedRight;
			if (side == TOP) selected = &selectedTop;
			if (side == BOTTOM) selected = &selectedBottom;
			Wall* cd = new Wall(
				Point(x, c, OBJECT),
				Point(x, d, OBJECT),
				side,
				parent->getAbsorption(side),
				parent->getEdgeLocalReacting(side),
				selected
			);
			walls->push_back(cd);
		}
	} else {
		// Get the end points of the wall and merge segment
		int p1 = wall->getMinX();
		int p2 = wall->getMaxX();
		int q1 = toMerge.first;
		int q2 = toMerge.second;
		
		// Sort the points on increasing coordinate
		//   Note that p1 and q1 are both at most p2
		//   and q2.
		int a = std::min(p1, q1);
		int b = std::max(p1, q1);
		int c = std::min(p2, q2);
		int d = std::max(p2, q2);
		
		// Save some variables from the current wall
		std::vector<Wall*>* walls = parent->getWalls();
		int y = walls->at(wallID)->getP1()->getObject().y();
		Side side = walls->at(wallID)->getSide();
		
		// Remove the current wall
		walls->erase(walls->begin() + wallID);
		
		// Add two new walls from a to b and from c to d
		//   Note that the overlapping segment is always
		//   from b to c.
		if (a != b) {
			bool* selected = nullptr;
			if (side == LEFT) selected = &selectedLeft;
			if (side == RIGHT) selected = &selectedRight;
			if (side == TOP) selected = &selectedTop;
			if (side == BOTTOM) selected = &selectedBottom;
			Wall* ab = new Wall(
				Point(a, y, OBJECT),
				Point(b, y, OBJECT),
				side,
				parent->getAbsorption(side),
				parent->getEdgeLocalReacting(side),
				selected
			);
			walls->push_back(ab);
		}
		if (c != d) {
			bool* selected = nullptr;
			if (side == LEFT) selected = &selectedLeft;
			if (side == RIGHT) selected = &selectedRight;
			if (side == TOP) selected = &selectedTop;
			if (side == BOTTOM) selected = &selectedBottom;
			Wall* cd = new Wall(
				Point(c, y, OBJECT),
				Point(d, y, OBJECT),
				side,
				parent->getAbsorption(side),
				parent->getEdgeLocalReacting(side),
				selected
			);
			walls->push_back(cd);
		}
	}
}

/**
 * Returns the absorption of a wall given its side.
 * 
 * @param side  The side of the wall
 * @return  The absorption coefficient of this wall
 */
double* Domain::getAbsorption(Side side) {
	if (side == TOP) return &absorptionTop;
	if (side == BOTTOM) return &absorptionBottom;
	if (side == LEFT) return &absorptionLeft;
	if (side == RIGHT) return &absorptionRight;
	return nullptr;
}

/**
 * Returns the edge local reacting value of a wall
 * given its side.
 * 
 * @param side  The side of the wall
 * @return  The edge local reacting value of this wall
 */
bool* Domain::getEdgeLocalReacting(Side side) {
	if (side == TOP) return &elrTop;
	if (side == BOTTOM) return &elrBottom;
	if (side == LEFT) return &elrLeft;
	if (side == RIGHT) return &elrRight;
	return nullptr;
}

/**
 * Updates each wall's absorption coefficient according to
 * the domain's absorption coefficient.
 */
void Domain::updateAbsorption() {
	/*for (unsigned int i = 0; i < walls.size(); i++) {
		if (walls[i]->getSide() == TOP) walls[i]->setAbsorption(absorptionTop);
		if (walls[i]->getSide() == BOTTOM) walls[i]->setAbsorption(absorptionBottom);
		if (walls[i]->getSide() == LEFT) walls[i]->setAbsorption(absorptionLeft);
		if (walls[i]->getSide() == RIGHT) walls[i]->setAbsorption(absorptionRight);
	}*/
}

/**
 * Updates the domain's absorption coefficient given a side to update.
 * 
 * @param side  The side of which to update the absorption coefficient
 * @param value  The new value of the absorption coefficient
 */
void Domain::setAbsorption(Side side, double value) {
	if (side == TOP) absorptionTop = value;
	if (side == BOTTOM) absorptionBottom = value;
	if (side == LEFT) absorptionLeft = value;
	if (side == RIGHT) absorptionRight = value;
}

/**
 * Updates the domain's edge local reacting values given a side to update.
 * 
 * @param side  The side of which to update the edge local reacting
 * @param e  The new value of edge local reacting
 */
void Domain::setEdgeLocalReacting(Side side, bool e) {
	if (side == TOP) elrTop = e;
	if (side == BOTTOM) elrBottom = e;
	if (side == LEFT) elrLeft = e;
	if (side == RIGHT) elrRight = e;
}

int Domain::getMinX() { return std::min(p1->getObject().x(), p2->getObject().x()); }
int Domain::getMaxX() { return std::max(p1->getObject().x(), p2->getObject().x()); }
int Domain::getMinY() { return std::min(p1->getObject().y(), p2->getObject().y()); }
int Domain::getMaxY() { return std::max(p1->getObject().y(), p2->getObject().y()); }
Point* Domain::getMinXP() { return (p1->getObject().x() <  p2->getObject().x() ? p1 : p2); }
Point* Domain::getMaxXP() { return (p1->getObject().x() >= p2->getObject().x() ? p1 : p2); }
Point* Domain::getMinYP() { return (p1->getObject().y() <  p2->getObject().y() ? p1 : p2); }
Point* Domain::getMaxYP() { return (p1->getObject().y() >= p2->getObject().y() ? p1 : p2); }
void Domain::setP1(Point p1) { this->p1 = new Point(p1.getObject(), OBJECT); }
void Domain::setP2(Point p2) { this->p2 = new Point(p2.getObject(), OBJECT); }

void Domain::moveDomain(QPoint deltaObject) {
	// Move p1 and p2 by deltaObject
	p1->setObject(p1->getObject() + deltaObject);
	p2->setObject(p2->getObject() + deltaObject);
	resetWalls();
}

void Domain::moveWalls(std::set<Side> toMove, QPoint deltaObject) {
	// Loop through all sides to move
	for (std::set<Side>::iterator i = toMove.begin(); i != toMove.end(); i++) {
		// Check the side to move
		if (*i == LEFT) {
			// Get the points with min and max x-coordinate
			Point* min = getMinXP();
			Point* max = getMaxXP();
			
			// Update the min (left) according to deltaObject (horizontal only)
			min->setObject(min->getObject() + QPoint(deltaObject.x(), 0));
			
			// Check if min and max have switched
			if (min->getObject().x() > max->getObject().x()) {
				switchSelectedWalls(LEFT, RIGHT);
			}
		}
		if (*i == RIGHT) {
			// Get the points with min and max x-coordinate
			Point* min = getMinXP();
			Point* max = getMaxXP();
			
			// Update the max (right) according to deltaObject (horizontal only)
			max->setObject(max->getObject() + QPoint(deltaObject.x(), 0));
			
			// Check if min and max have switched
			if (min->getObject().x() > max->getObject().x()) {
				switchSelectedWalls(RIGHT, LEFT);
			}
		}
		if (*i == TOP) {
			// Get the points with min and max y-coordinate
			Point* min = getMinYP();
			Point* max = getMaxYP();
			
			// Update the max (top) according to deltaObject (vertical only)
			max->setObject(max->getObject() + QPoint(0, deltaObject.y()));
			
			// Check if min and max have switched
			if (min->getObject().y() > max->getObject().y()) {
				switchSelectedWalls(TOP, BOTTOM);
			}
		}
		if (*i == BOTTOM) {
			// Get the points with min and max y-coordinate
			Point* min = getMinYP();
			Point* max = getMaxYP();
			
			// Update the min (bottom) according to deltaObject (vertical only)
			min->setObject(min->getObject() + QPoint(0, deltaObject.y()));
			
			// Check if min and max have switched
			if (min->getObject().y() > max->getObject().y()) {
				switchSelectedWalls(BOTTOM, TOP);
			}
		}
	}
}

void Domain::switchSelectedWalls(Side oldside, Side newside) {
	std::cout << "switching" << std::endl;
	// Deselect the old walls
	if (oldside == LEFT) selectedLeft = false;
	if (oldside == RIGHT) selectedRight = false;
	if (oldside == TOP) selectedTop = false;
	if (oldside == BOTTOM) selectedBottom = false;
	
	// Select the new walls
	if (newside == LEFT) selectedLeft = true;
	if (newside == RIGHT) selectedRight = true;
	if (newside == TOP) selectedTop = true;
	if (newside == BOTTOM) selectedBottom = true;
}