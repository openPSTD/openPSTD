#include "wall.h"

#include "modelmanager.h"
#include "point.h"

/**
 * Constructor.
 * 
 * @param x0  The x coordinate of the first end of the wall
 * @param y0  The y coordinate of the first end of the wall
 * @param x1  The x coordinate of the second end of the wall
 * @param y1  The y coordinate of the second end of the wall
 * @param side  The side of the domain that this wall is on
 * @param settings  A reference to a Settings instance
 * @param absorption  The initial absorption coefficient of the wall
 * @param e  The edge local reacting value of the wall
 */
Wall::Wall(Point p1, Point p2, Side side, double* absorption, bool* e, bool* selected) {
	// Save end point coordinates locally
	this->p1 = new Point(p1.getObject(), OBJECT);
	this->p2 = new Point(p2.getObject(), OBJECT);
	this->side = side;
	this->absorption = absorption;
	this->edgelocalreacting = e;
	this->selected = selected;
	
	// Initialize the state variables
	drawWallLength = true;
}

/**
 * Draws the length of a wall given its end points.
 * 
 * @param pixels  The pixels array to draw to
 * @param zoom  The current zoom level (as in model)
 * @param offsetX  The current x offset of the scene (as in model)
 * @param offsetY  The current y offset of the scene (as in model)
 * @param selected  Whether or not the wall is currently selected
 */
void Wall::draw(QImage* pixels, bool domainSelected) {
	int x0 = p1->getScreen().x();
	int x1 = p2->getScreen().x();
	int y0 = p1->getScreen().y();
	int y1 = p2->getScreen().y();
	
	// Verify that the wall is orthogonal
	if (x0 != x1 && y0 != y1) {
		std::cerr << "Cannot draw non-orthogonal wall length text" << std::endl;
		return;
	}
	
	// Get the minimum and maximum x and y coordinates of the corners
	int minx = std::min(x0, x1);
	int maxx = std::max(x0, x1);
	int miny = std::min(y0, y1);
	int maxy = std::max(y0, y1);
	
	// Draw all points on the wall
	Settings* settings = Settings::getInstance();
	if (minx == maxx) {
		for (int j = miny; j <= maxy; j++) {
			for (int d = -1; d <= 1; d++) {
				int x = minx + d;
				int y = j;
				QRgb color = gradient(settings->wallColor0, settings->wallColor1, *absorption);
				if (*selected || domainSelected) color = qRgb(0, 255, 255);
				if (x < 0 || y < 0) continue;
				if (x >= pixels->width() || y >= pixels->height()) continue;
				pixels->setPixel(x, y, color);
			}
		}
	}
	if (miny == maxy) {
		for (int i = minx; i <= maxx; i++) {
			for (int d = -1; d <= 1; d++) {
				int x = i;
				int y = miny + d;
				QRgb color = gradient(settings->wallColor0, settings->wallColor1, *absorption);
				if (*selected || domainSelected) color = qRgb(0, 255, 255);
				if (x < 0 || y < 0) continue;
				if (x >= pixels->width() || y >= pixels->height()) continue;
				pixels->setPixel(x, y, color);
			}
		}
	}
	
	// Compute the length of the wall
	int lx = p2->getObject().x() - p1->getObject().x();
	int ly = p2->getObject().y() - p1->getObject().y();
	int length = std::abs(lx + ly);
	std::string lengthtext = std::to_string(length);
	
	// Compute the midpoint of the wall
	int midx = (minx + maxx) / 2;
	int midy = (miny + maxy) / 2;
	
	// Update midpoint based on the wall's side
	if (side == LEFT) {
		midx -= 12 * lengthtext.size();
	}
	if (side == RIGHT) {
		midx += 2;
	}
	if (side == TOP) {
		midx -= 5 * lengthtext.size();
		midy -= 10;
	}
	if (side == BOTTOM) {
		midx -= 5 * lengthtext.size();
		midy += 8;
	}
	
	// Draw the wall length text
	if (drawWallLength) {
		drawText(
			lengthtext,
			midx,
			midy - 7,
			14,
			qRgb(0, 0, 0),
			pixels
		);
	}
}

/**
 * Checks whether or not two given walls need to be merged.
 * The algorithm implemented in this method is documented
 * in docs/Wall intersection.docx.
 * 
 * @param one  The first wall to check with
 * @param two  The second wall to check with
 * @param toMerge  A pointer to a location to write the segment that needs to be merged
 * @return  Whether or not the two walls need to be merged
 */
bool Wall::mergeWalls(Wall one, Wall two, std::pair<int, int>* toMerge) {
	// Compute the orientation of the walls
	bool o1 = one.getSide() == LEFT || one.getSide() == RIGHT;
	bool o2 = two.getSide() == LEFT || two.getSide() == RIGHT;
	
	// Check if both walls are vertical
	if (o1 && o2 && one.getMinX() == two.getMinX()) {
		// Get the start and end points of both walls
		int p1 = one.getMinY();
		int p2 = one.getMaxY();
		int q1 = two.getMinY();
		int q2 = two.getMaxY();
		std::cout << std::endl;
		std::cout << p1 << ", " << p2 << std::endl;
		std::cout << q1 << ", " << q2 << std::endl;
		
		// Check if the two walls overlap
		// Do nothing if the walls do not overlap
		if (p2 <= q1 || p1 >= q2) return false;
		
		// Compute the overlapping segment
		//   We know that there is overlap, so
		//   p1 and q1 are both smaller than p2 and q2.
		//   Hence the overlapping segment is from
		//   max(p1, q1) to min(p2, q2).
		int start = std::max(p1, q1);
		int end = std::min(p2, q2);
		*toMerge = std::make_pair(start, end);
		std::cout << "overlap" << std::endl;
		return true;
	}
	
	// Check if both walls are horizontal
	if (!o1 && !o2 && one.getMinY() == two.getMinY()) {
		// Get the start and end points of both walls
		int p1 = one.getMinX();
		int p2 = one.getMaxX();
		int q1 = two.getMinX();
		int q2 = two.getMaxX();
		
		// Check if the two walls overlap
		// Do nothing if the walls do not overlap
		if (p2 <= q1 || p1 >= q2) return false;
		
		// Compute the overlapping segment
		//   We know that there is overlap, so
		//   p1 and q1 are both smaller than p2 and q2.
		//   Hence the overlapping segment is from
		//   max(p1, q1) to min(p2, q2).
		int start = std::max(p1, q1);
		int end = std::min(p2, q2);
		*toMerge = std::make_pair(start, end);
		return true;
	}
	
	return false;
	
	/*// Get the end point coordinates of both walls
	int p1x = one.getMinX();
	int p1y = one.getMinY();
	int p2x = one.getMaxX();
	int p2y = one.getMaxY();
	int p3x = two.getMinX();
	int p3y = two.getMinY();
	int p4x = two.getMaxX();
	int p4y = two.getMaxY();
	std::cout << p1x << ", " << p1y << " - " << p2x << ", " << p2y << std::endl;
	std::cout << p3x << ", " << p3y << " - " << p4x << ", " << p4y << std::endl;
	std::cout << std::endl;
	
	// Check if the walls are horizontal or vertical
	if (one.getSide() == LEFT || one.getSide() == RIGHT) {
		// Do nothing if the walls are not parallel
		if (two.getSide() != LEFT && two.getSide() != RIGHT) return false;
		
		// Do nothing if the walls do not have the same x coordinate
		if (p1x != p3x) return false;
		
		// Sort points on coordinate
		std::vector<int> original = {p1y, p2y, p3y, p4y};
		std::vector<int> order = sort(original);
		
		// Check if the second segment is on both walls
		bool o1w1 = pointOnWall(original[order[1]], p1y, p2y);
		bool o2w1 = pointOnWall(original[order[2]], p1y, p2y);
		bool o1w2 = pointOnWall(original[order[1]], p3y, p4y);
		bool o2w2 = pointOnWall(original[order[2]], p3y, p4y);
		bool s1w1 = o1w1 && o2w1;
		bool s1w2 = o1w2 && o2w2;
		if (s1w1 && s1w2 && original[order[1]] != original[order[2]]) {
			// The walls intersect between o1 and o2
			toMerge->first = std::min(original[order[1]], original[order[2]]);
			toMerge->second = std::max(original[order[1]], original[order[2]]);
			return true;
		}
		
		// There is no intersection
		return false;
	} else {
		// Do nothing if the walls are not parallel
		if (two.getSide() != TOP && two.getSide() != BOTTOM) return false;
		
		// Do nothing if the walls do not have the same y coordinate
		if (p1y != p3y) return false;
		
		// Sort points on coordinate
		std::vector<int> original = {p1x, p2x, p3x, p4x};
		std::vector<int> order = sort(original);
		
		// Check if the second segment is on both walls
		bool o1w1 = pointOnWall(original[order[1]], p1x, p2x);
		bool o2w1 = pointOnWall(original[order[2]], p1x, p2x);
		bool o1w2 = pointOnWall(original[order[1]], p3x, p4x);
		bool o2w2 = pointOnWall(original[order[2]], p3x, p4x);
		bool s1w1 = o1w1 && o2w1;
		bool s1w2 = o1w2 && o2w2;
		if (s1w1 && s1w2 && original[order[1]] != original[order[2]]) {
			// The walls intersect between o1 and o2
			toMerge->first = std::min(original[order[1]], original[order[2]]);
			toMerge->second = std::max(original[order[1]], original[order[2]]);
			return true;
		}
		
		// There is no intersection
		return false;
	}*/
}

/**
 * Draws a string to the given pixels array.
 * 
 * @param text  The text to draw
 * @param x  The x position to draw at
 * @param y  The y position to draw at
 * @param size  The font size to draw with
 * @param color  The color to draw in
 * @param pixels  The pixels array to draw to
 */
void Wall::drawText(std::string text, int x, int y, int size, QRgb color, QImage* pixels) {
	QPainter p;
	p.begin(pixels);
	p.setPen(QPen(color));
	p.setFont(QFont("Monospace", size));
	p.drawText(x, y + size, QString(text.c_str()));
	p.end();
}

/**
 * Sorts a vector of integers, and returns a vector containing the indices
 * of the sorted elements in the original vector.
 * 
 * @param original  The vector of integers to sort
 * @return  A vector of indices in the original vector
 */
std::vector<int> Wall::sort(std::vector<int> original) {
	// Create the result vector
	std::vector<int> result;
	
	// Loop while there are unsorted elements
	while (result.size() < original.size()) {
		// Get the minimum element in original that is not yet in result
		int minIndex = -1;
		int minValue = 0;
		for (unsigned int i = 0; i < original.size(); i++) {
			if (std::find(result.begin(), result.end(), i) != result.end()) continue;
			if (minIndex == -1 || original[i] < minValue) {
				minIndex = static_cast<int>(i);
				minValue = original[i];
			}
		}
		
		// Add the index of this element to result
		result.push_back(minIndex);
	}
	
	// Return the result vector
	return result;
}

/**
 * Returns a gradient between the two given colors, according to
 * a double t in [0, 1].
 * 
 * @param color1  The first color
 * @param color2  The second color
 * @param t  The percentage that the first color should be included
 * @return  A gradient between the two given colors
 */
QRgb Wall::gradient(QRgb color1, QRgb color2, double t) {
	int r1 = QColor(color1).red();
	int r2 = QColor(color2).red();
	int g1 = QColor(color1).green();
	int g2 = QColor(color2).green();
	int b1 = QColor(color1).blue();
	int b2 = QColor(color2).blue();
	
	return qRgb(
		static_cast<int>((1-t) * r1 + t * r2),
		static_cast<int>((1-t) * g1 + t * g2),
		static_cast<int>((1-t) * b1 + t * b2)
	);
}

int Wall::getMinX() { return std::min(p1->getObject().x(), p2->getObject().x()); }
int Wall::getMaxX() { return std::max(p1->getObject().x(), p2->getObject().x()); }
int Wall::getMinY() { return std::min(p1->getObject().y(), p2->getObject().y()); }
int Wall::getMaxY() { return std::max(p1->getObject().y(), p2->getObject().y()); }

// TODO
void Wall::setMinX(int x) {
	if (p1->getObject().x() < p2->getObject().x()) {
		QPoint object = p1->getObject();
		object.setX(x);
		p1->setObject(object);
	} else {
		QPoint object = p2->getObject();
		object.setX(x);
		p2->setObject(object);
	}
}

void Wall::setMaxX(int x) {
	if (p1->getObject().x() > p2->getObject().x()) {
		QPoint object = p1->getObject();
		object.setX(x);
		p1->setObject(object);
	} else {
		QPoint object = p2->getObject();
		object.setX(x);
		p2->setObject(object);
	}
}

void Wall::setMinY(int y) {
	if (p1->getObject().y() < p2->getObject().y()) {
		QPoint object = p1->getObject();
		object.setY(y);
		p1->setObject(object);
	} else {
		QPoint object = p2->getObject();
		object.setY(y);
		p2->setObject(object);
	}
}

void Wall::setMaxY(int y) {
	if (p1->getObject().y() > p2->getObject().y()) {
		QPoint object = p1->getObject();
		object.setY(y);
		p1->setObject(object);
	} else {
		QPoint object = p2->getObject();
		object.setY(y);
		p2->setObject(object);
	}
}