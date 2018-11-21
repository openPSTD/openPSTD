#include "measurehandler.h"

/**
 * Constructor.
 * Initializes the private reference variables.
 * 
 * @param mouse  A reference to the mouse position
 */
MeasureHandler::MeasureHandler(Point* mouse) {
	// Save reference variables locally
	this->mouse = mouse;
	
	// Initialize state variables
	this->hasP1 = false;
	this->hasP2 = false;
	this->shown = false;
	p1 = new Point(0, 0, OBJECT);
	p2 = new Point(0, 0, OBJECT);
}

/**
 * Destructor.
 * Deallocates allocated state variables.
 */
MeasureHandler::~MeasureHandler() {
	// Delete allocated state variables
	delete p1;
	delete p2;
}

/**
 * Registers the first point of the measure.
 */
void MeasureHandler::setP1() {
	// Save the first point of the measure
	Model* model = ModelManager::getInstance()->getCurrent();
	if (model->ctrlPressed) *p1 = *mouse;
	else *p1 = mouse->clampToGrid();
	
	// Hide the measure tool until p2 is defined
	this->hasP1 = true;
	this->hasP2 = false;
	this->shown = false;
}

/**
 * Registers the second point of the measure.
 */
void MeasureHandler::setP2(bool final) {
	if (!hasP1) return;
	
	// Save the second point of the measure
	Model* model = ModelManager::getInstance()->getCurrent();
	if (model->ctrlPressed) *p2 = *mouse;
	else *p2 = mouse->clampToGrid();
	
	// Show the measure tool
	this->shown = true;
	if (final) {
		this->hasP1 = false;
		this->hasP2 = false;
	}
}

/**
 * Removes the measure.
 */
void MeasureHandler::remove() {
	// Hide the measure tool
	this->shown = false;
	this->hasP1 = false;
	this->hasP2 = false;
}

// TODO
void MeasureHandler::draw(QImage* pixels) {
	// Do nothing if not measuring
	if (!shown) return;
	
	// Get the measure line coordinates
	Model* model = ModelManager::getInstance()->getCurrent();
	int x0 = p1->getScreen().x();
	int x1 = p2->getScreen().x();
	int y0 = p1->getScreen().y();
	int y1 = p2->getScreen().y();
	
	// Loop through all points on the line
	for (int i = 0; i < 1000; i++) {
		double t = static_cast<double>(i) / 1000;
		
		int x = static_cast<int>(t * x0 + (1 - t) * x1);
		int y = static_cast<int>(t * y0 + (1 - t) * y1);
		
		if (x < 0 || x >= pixels->width()) continue;
		if (y < 0 || y >= pixels->height()) continue;
		pixels->setPixel(x, y, qRgb(0, 0, 255));
	}
	
	// Compute the length of the measure
	int minx = x0 / model->zoom;
	int maxx = x1 / model->zoom;
	int miny = y0 / model->zoom;
	int maxy = y1 / model->zoom;
	double length = std::sqrt((maxx-minx)*(maxx-minx) + (maxy-miny)*(maxy-miny));
	std::ostringstream oss;
	oss << std::fixed << std::setprecision(1) << length;
	std::string lengthtext = oss.str();
	
	// Compute the midpoint of the measure
	int midx = (x0 + x1) / 2;
	int midy = (y0 + y1) / 2;
	
	// Draw the measure length text
	drawText(
		lengthtext,
		midx,
		midy,
		14,
		qRgb(0, 0, 255),
		pixels
	);
}

// TODO
void MeasureHandler::drawText(std::string text, int x, int y, int size, QRgb color, QImage* pixels) {
	QPainter p;
	p.begin(pixels);
	p.setPen(QPen(color));
	p.setFont(QFont("Monospace", size));
	p.drawText(x, y + size, QString(text.c_str()));
	p.end();
}