#ifndef WALL_H
#define WALL_H

#include <QImage>
#include <QPainter>
#include <QPen>
#include <QFont>
#include <iostream>
#include <algorithm>
#include "settings.h"

class Point;

enum Side {
	LEFT,
	RIGHT,
	TOP,
	BOTTOM
};

/**
 * Representation of a single wall in a domain.
 */
class Wall {
public:
	// Constructor
	inline Wall() {}
	Wall(Point p1, Point p2, Side side, double* absorption, bool* e, bool* selected);
	
	// Drawing method
	void draw(QImage* pixels, bool domainSelected);
	
	// Get methods
	int getMinX();
	int getMaxX();
	int getMinY();
	int getMaxY();
	inline Point* getP1() { return p1; }
	inline Point* getP2() { return p2; }
	inline Side getSide() { return side; }
	inline double getAbsorption() { return *absorption; }
	inline bool getEgdeLocalReacting() { return *edgelocalreacting; }
	inline bool getSelected() { return *selected; }
	
	// Set methods
	void setMinX(int x);
	void setMaxX(int x);
	void setMinY(int y);
	void setMaxY(int y);
	inline void setSide(Side side) { this->side = side; }
	inline void setAbsorption(double absorption) { *(this->absorption) = absorption; }
	inline void setEdgeLocalReacting(bool e) { *(this->edgelocalreacting) = e; }
	inline void setDrawWallLength(bool draw) { this->drawWallLength = draw; }
	
	inline void toggleSelected() { *(this->selected) = !*(this->selected); }
	inline void setSelected(bool s) { *(this->selected) = s; }
	
	// Checks if two given walls need to be merged
	static bool mergeWalls(Wall one, Wall two, std::pair<int, int>* toMerge);
private:
	// Corner coordinates
	Point* p1;
	Point* p2;
	
	// Side of the wall
	Side side;
	
	// Whether or not the wall is selected
	bool* selected;
	
	// Absorption coefficient
	double* absorption;
	bool* edgelocalreacting;
	
	// Whether or not to draw the wall length text
	bool drawWallLength;
	
	// Private drawing methods
	void drawText(std::string text, int x, int y, int size, QRgb color, QImage* pixels);
	
	// Sorts a vector of integers
	static std::vector<int> sort(std::vector<int> original);
	
	// Checks if a given point is on a wall, given its end point coordinates
	inline static bool pointOnWall(int point, int wallstart, int wallend) {
		int min = std::min(wallstart, wallend);
		int max = std::max(wallstart, wallend);
		return min <= point && point <= max;
	}
	
	// Returns a gradient between the two given colors
	QRgb gradient(QRgb color1, QRgb color2, double t);
};

#endif
