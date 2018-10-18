#ifndef RECEIVER_H
#define RECEIVER_H

#include <QImage>
#include <QPainter>
#include "settings.h"

class Point;

/**
 * Representation of a single receiver.
 */
class Receiver {
public:
	// Constructor
	Receiver(Point* p);
	Receiver* copy();
	
	// Drawing method
	void draw(QImage* pixels);
	
	// Get methods for the position of the receiver
	int getX();
	int getY();
	bool getSelected() { return selected; }
	
	// Set methods for the position of the receiver
	void toggleSelected() { this->selected = !this->selected; }
	void setSelected(bool s) { this->selected = s; }
	
	void moveReceiver(QPoint deltaObject);
private:
	// Receiver image
	QImage image;
	
	// Position of the receiver
	Point* p;
	
	// Whether or not this receiver is selected
	bool selected;
	
	// Sets a single pixel's color
	void setPixel(int x, int y, QRgb color, QImage* pixels);
};

#endif