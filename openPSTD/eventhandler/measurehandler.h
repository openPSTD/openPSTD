#ifndef MEASUREHANDLER_H
#define MEASUREHANDLER_H

#include <QPoint>
#include <sstream>
#include <iomanip>
#include "../modelmanager.h"
#include "../grid.h"
#include "../point.h"

class MeasureHandler {
public:
	// Constructor, destructor
	MeasureHandler(Point* mouse);
	~MeasureHandler();
	
	// Registers the first point of the measure
	void setP1();
	
	// Registers the second point of the measure
	void setP2(bool final);
	
	// Removes the measure
	void remove();
	
	// Draws the measure
	void draw(QImage* pixels);
	
	// Returns the first point of the measure
	QPoint getP1() { return p1->getObject(); }
private:
	// Reference variables
	Point* mouse;
	
	// State variables
	Point* p1;
	Point* p2;
	bool hasP1;
	bool hasP2;
	bool shown;
	
	// Private drawing methods
	void drawText(std::string text, int x, int y, int size, QRgb color, QImage* pixels);
};

#endif