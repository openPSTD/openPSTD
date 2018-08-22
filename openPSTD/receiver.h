#ifndef RECEIVER_H
#define RECEIVER_H

#include <QImage>
#include <QPainter>
#include "settings.h"

/**
 * Representation of a single receiver.
 */
class Receiver {
public:
	// Constructor
	Receiver(int x, int y);
	
	// Drawing method
	void draw(QImage* pixels, int zoom, int offsetX, int offsetY, bool selected);
	
	// Get methods for the position of the receiver
	int getX() { return x; }
	int getY() { return y; }
	
	// Set methods for the position of the receiver
	void setX(int x) { this->x = x; }
	void setY(int y) { this->y = y; }
private:
	// Receiver image
	QImage image;
	
	// Position of the receiver
	int x;
	int y;
	
	// Sets a single pixel's color
	void setPixel(int x, int y, QRgb color, QImage* pixels);
};

#endif