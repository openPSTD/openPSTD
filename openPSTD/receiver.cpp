#include "receiver.h"

#include "modelmanager.h"
#include "point.h"

/**
 * Constructor.
 * 
 * @param x  The x coordinate of the receiver
 * @param y  The y coordinate of the receiver
 * @param settings  A reference to the Settings instance
 */
Receiver::Receiver(Point* p) {
	// Save position locally
	this->p = p;
	
	// Load the receiver image
	image = QImage(":/new/prefix1/icons/receiver.png");
	
	// Initialize state variables
	this->selected = false;
}

Receiver* Receiver::copy() {
	Point* pp = new Point(p->getObject(), OBJECT);
	Receiver* c = new Receiver(pp);
	return c;
}

/**
 * Draws the receiver.
 * 
 * @param pixels  The pixels array to draw to
 * @param zoom  The current zoom level (as in model)
 * @param offsetX  The current x offset of the scene (as in model)
 * @param offsetY  The current y offset of the scene (as in model)
 * @param selected  Whether or not the receiver is currently selected
 */
void Receiver::draw(QImage* pixels) {
	int x = this->p->getScreen().x();
	int y = this->p->getScreen().y();
	
	// Draw the receiver image
	QPainter p;
	p.begin(pixels);
	p.drawImage(
		x - 5,
		y - 5,
		image
	);
	p.end();
	
	// Check if this receiver is selected
	if (selected) {
		// Draw a square around the receiver
		int d = 8;
		for (int i = -d; i <= d; i++) {
			setPixel(
				x + i,
				y + d,
				qRgb(0, 255, 255),
				pixels
			);
			setPixel(
				x + i,
				y - d,
				qRgb(0, 255, 255),
				pixels
			);
			setPixel(
				x + d,
				y + i,
				qRgb(0, 255, 255),
				pixels
			);
			setPixel(
				x - d,
				y + i,
				qRgb(0, 255, 255),
				pixels
			);
		}
	}
}

/**
 * Sets a single pixel's color.
 * 
 * @param x  The x coordinate of the pixel
 * @param y  The y coordinate of the pixel
 * @param color  The color to give the pixel
 * @param pixels  A pixels array to draw to
 */
void Receiver::setPixel(int x, int y, QRgb color, QImage* pixels) {
	if (x < 0 || y < 0) return;
	if (x >= pixels->width() || y >= pixels->height()) return;
	pixels->setPixel(x, y, color);
}

int Receiver::getX() { return p->getObject().x(); }
int Receiver::getY() { return p->getObject().y(); }

void Receiver::moveReceiver(QPoint deltaObject) {
	// Move p by deltaObject
	p->setObject(p->getObject() + deltaObject);
}