#include "source.h"

#include "modelmanager.h"
#include "point.h"

/**
 * Constructor.
 * 
 * @param x  The x coordinate of the source
 * @param y  The y coordinate of the source
 * @param settings  A reference to the Settings instance
 */
Source::Source(Point* p) {
	// Save position locally
	this->p = p;
	
	// Load the source image
	image = QImage(":/new/prefix1/icons/source.png");
	
	// Initialize state variables
	this->selected = false;
}

Source* Source::copy() {
	Point* pp = new Point(p->getObject(), OBJECT);
	Source* c = new Source(pp);
	return c;
}

/**
 * Draws the source.
 * 
 * @param pixels  The pixels array to draw to
 * @param zoom  The current zoom level (as in model)
 * @param offsetX  The current x offset of the scene (as in model)
 * @param offsetY  The current y offset of the scene (as in model)
 * @param selected  Whether or not the source is currently selected
 */
void Source::draw(QImage* pixels) {
	int x = this->p->getScreen().x();
	int y = this->p->getScreen().y();
	
	// Draw the source image
	QPainter p;
	p.begin(pixels);
	p.drawImage(
		x - 5,
		y - 5,
		image
	);
	p.end();
	
	// Check if this source is selected
	if (selected) {
		// Draw a square around the source
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
void Source::setPixel(int x, int y, QRgb color, QImage* pixels) {
	if (x < 0 || y < 0) return;
	if (x >= pixels->width() || y >= pixels->height()) return;
	pixels->setPixel(x, y, color);
}

int Source::getX() { return p->getObject().x(); }
int Source::getY() { return p->getObject().y(); }

void Source::moveSource(QPoint deltaObject) {
	// Move p by deltaObject
	p->setObject(p->getObject() + deltaObject);
}