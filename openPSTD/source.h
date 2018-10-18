#ifndef SOURCE_H
#define SOURCE_H

#include <QImage>
#include <QPainter>
#include "settings.h"

class Point;

/**
 * Representation of a single source.
 */
class Source {
public:
	// Constructor
	Source(Point* p);
	Source* copy();
	
	// Drawing method
	void draw(QImage* pixels);
	
	// Get methods
	int getX();
	int getY();
	bool getSelected() { return selected; }
	
	// Set methods
	void toggleSelected() { this->selected = !this->selected; }
	void setSelected(bool selected) { this->selected = selected; }
	
	void moveSource(QPoint deltaObject);
private:
	// Source image
	QImage image;
	
	// Position of the source
	Point* p;
	
	// Whether or not this source is selected
	bool selected;
	
	// Sets a single pixel's color
	void setPixel(int x, int y, QRgb color, QImage* pixels);
};

#endif