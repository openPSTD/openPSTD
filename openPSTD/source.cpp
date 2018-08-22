#include "source.h"

/**
 * Constructor.
 * 
 * @param x  The x coordinate of the source
 * @param y  The y coordinate of the source
 * @param settings  A reference to the Settings instance
 */
Source::Source(int x, int y) {
	// Save position locally
	this->x = x;
	this->y = y;
	
	// Load the source image
	image = QImage(":/new/prefix1/icons/source.png");
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
void Source::draw(QImage* pixels, int zoom, int offsetX, int offsetY, bool selected) {
	// Draw the source image
	QPainter p;
	p.begin(pixels);
	p.drawImage(
		zoom * x + offsetX - 5,
		zoom * y + offsetY - 5,
		image
	);
	p.end();
	
	// Check if this source is selected
	if (selected) {
		// Draw a square around the source
		for (int i = -5; i <= 5; i++) {
			setPixel(
				zoom * x + i + offsetX,
				zoom * y + 5 + offsetY,
				qRgb(0, 255, 255),
				pixels
			);
			setPixel(
				zoom * x + i + offsetX,
				zoom * y - 5 + offsetY,
				qRgb(0, 255, 255),
				pixels
			);
			setPixel(
				zoom * x + 5 + offsetX,
				zoom * y + i + offsetY,
				qRgb(0, 255, 255),
				pixels
			);
			setPixel(
				zoom * x - 5 + offsetX,
				zoom * y + i + offsetY,
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