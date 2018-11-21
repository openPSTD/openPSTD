#include "point.h"

Point::Point(QPoint p, PointType type) {
	this->model = ModelManager::getInstance()->getCurrent();
	if (type == OBJECT) {
		this->object = p;
	} else {
		this->object = screen2object(p);
	}
}

Point::Point(int x, int y, PointType type) {
	this->model = ModelManager::getInstance()->getCurrent();
	if (type == OBJECT) {
		this->object = QPoint(x, y);
	} else {
		this->object = screen2object(QPoint(x, y));
	}
}

QPoint Point::getScreen() {
	return object2screen(object);
}

QPoint Point::getObject() {
	return object;
}

/*QPoint Point::getObject(Model* model) {
	this->model = model;
	QPoint obj = getObject();
	this->model = ModelManager::getInstance()->getCurrent();
	return obj;
}*/

Point Point::clampToGrid() {
	// Clamping distance in pixels
	int max_delta = Settings::getInstance()->clampDist;
	
	// Define some helper variables
	int gz = model->gridsize * model->zoom;
	int ozx = model->offsetX * model->zoom;
	int ozy = model->offsetY * model->zoom;
	
	// Revert the offset on the mouse position
	QPoint orig(getScreen().x() - ozx, getScreen().y() - ozy);
	
	// Compute the clamped position (without any maximum clamping distance)
	int clamped_x = static_cast<int>(std::round(static_cast<double>(orig.x()) / gz)) * gz;
	int clamped_y = static_cast<int>(std::round(static_cast<double>(orig.y()) / gz)) * gz;
	
	// Compute the clamped distance
	int dx = std::abs(orig.x() - clamped_x);
	int dy = std::abs(orig.y() - clamped_y);
	
	// Use the clamped position or the original position, depending on the clamped distance
	int px = (dx <= max_delta ? clamped_x : orig.x());
	int py = (dy <= max_delta ? clamped_y : orig.y());
	
	/*// Return the resulting point
	return QPoint(
		px + ozx,
		py + ozy
	);*/
	
	// Save the new screen coordinates
	return Point(
		px + ozx,
		py + ozy,
		SCREEN
	);
}

void Point::setScreen(QPoint p) {
	this->object = screen2object(p);
}

void Point::setObject(QPoint p) {
	this->object = p;
}

void Point::convertToObject() {
	object = QPoint(
		static_cast<int>(std::round(object.x())),
		static_cast<int>(std::round(object.y()))
	);
}

QPoint Point::object2screen(QPoint p) {
	model = ModelManager::getInstance()->getCurrent();
	if (model == nullptr) std::cout << "ERROR" << std::endl;
	return QPoint(
		//p.x() * model->zoom + model->offsetX,
		//p.y() * model->zoom + model->offsetY
		(p.x() + model->offsetX) * model->zoom,
		(-p.y() + model->offsetY) * model->zoom
	);
}

QPoint Point::screen2object(QPoint p) {
	return QPoint(
		//(p.x() - model->offsetX) / model->zoom,
		//(p.y() - model->offsetY) / model->zoom
		p.x() / model->zoom - model->offsetX,
		-(p.y() / model->zoom - model->offsetY)
	);
}