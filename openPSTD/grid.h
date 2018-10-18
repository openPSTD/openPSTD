#ifndef GRID_H
#define GRID_H

#include <QPoint>
#include "settings.h"
#include "modelmanager.h"

/**
 * Class providing methods for comparing a point against the grid.
 */
class Grid {
public:
	/**
	 * Clamps a point to the grid, given its x and y
	 * coordinates. Returns the input point if there
	 * is no grid edge within clampDist.
	 * 
	 * @param x  The x coordinate of the point to clamp
	 * @param y  The y coordinate of the point to clamp
	 * @param model  A reference to the Model
	 * @param settings  A reference to the Settings instance
	 * @param clamped  Whether or not the given (x, y) position could be clamped
	 * @return  The clamped point
	 */
	inline static QPoint clampGrid(int x, int y, bool* clamped) {
		/*// Update x according to the scene offset
		int xx = x; // - model->offsetX;
		int yy = y; // - model->offsetY;
		
		// Do not clamp if there is no grid
		if (!model->showGrid) {
			if (clamped != nullptr) *clamped = false;
			return QPoint(xx, yy);
		}
		
		// Compute the coordinates of the nearest grid corner
		int gz = model->gridsize; // * model->zoom;
		int gridx = static_cast<int>(gz * round(static_cast<double>(xx) / gz));
		int gridy = static_cast<int>(gz * round(static_cast<double>(yy) / gz));
		
		// Compute the distance to this point from the given point
		int dx = std::abs(gridx - xx);
		int dy = std::abs(gridy - yy);
		
		// Clamp if the point is within clampDist, otherwise take itself
		Settings* settings = Settings::getInstance();
		int px = (dx < settings->clampDist ? gridx : xx);
		int py = (dy < settings->clampDist ? gridy : yy);
		
		// Return the clamped point
		if (clamped != nullptr) {
			*clamped = dx < settings->clampDist || dy < settings->clampDist;
		}
		return QPoint(px, py);*/
		
		// TODO: Input mouse coordinates instead of object coordinates
		// Then compute mouse coordinates of the lower grid edge.
		// Clamp within the clamp distance (in mouse coordinates).
		// Return result in mouse coordinates.
		
		int gridsize = 16; // Object coordinates
		
		// Convert the input point into mouse coordinates
		QPoint p = Grid::object2mouse(QPoint(x, y));
		
		// Compute the mouse coordinates of the lower grid edge
		QPoint g0 = QPoint(
			(int) (x / gridsize) * gridsize,
			(int) (y / gridsize) * gridsize
		);
		QPoint gg0 = Grid::object2mouse(g0);
		
		QPoint delta = gg0 - p;
		if (delta.x() < gridsize) p.setX(gg0.x());
		if (delta.y() < gridsize) p.setY(gg0.y());
		
		return Grid::mouse2object(p);
	}
	
	/**
	 * Clamps a point to a domain wall, given its x and y
	 * coordinates. Returns the input point if there is
	 * no wall within clampDist.
	 * 
	 * @param x  The x coordinate of the point to clamp
	 * @param y  The y coordinate of the point to clamp
	 * @param domains  A vector of domains to clamp to
	 * @param model  A reference to the Model
	 * @param settings  A reference to the Settings instance
	 * @param clamped  Whether or not the given (x, y) position could be clamped
	 * @param clampLastDomain  Whether or not to clamp to the last domain as well
	 * @return  The clamped point
	 */
	inline static QPoint clampDomains(
		int x,
		int y,
		std::vector<Domain*> domains,
		bool* clamped,
		bool clampLastDomain
	) {
		// Update x according to the scene offset and zoom level
		Model* model = ModelManager::getInstance()->getCurrent();
		int xx = (x - model->offsetX) / model->zoom;
		int yy = (y - model->offsetY) / model->zoom;
		
		// Return the input point if there are no domains to clamp to
		if (domains.size() == 0) {
			*clamped = false;
			return QPoint(x, y);
		}
		
		// Return the input point if the only domain is not allowed to be clamped to
		if (domains.size() == 1 && !clampLastDomain) {
			*clamped = false;
			return QPoint(x, y);
		}
		
		// Loop through all domains
		QPoint domainClamp(-1, -1);
		int mind2 = -1;
		for (unsigned int i = 0; i < domains.size(); i++) {
			// Do not clamp to the last domain if requested so
			if (!clampLastDomain && i == domains.size() - 1) continue;
			
			// Loop through all walls in the domain
			std::vector<Wall*>* walls = domains[i]->getWalls();
			for (unsigned int j = 0; j < walls->size(); j++) {
				// Get a pointer to this wall
				Wall* wall = walls->at(j);
				
				// Clamp the input point to this wall
				QPoint clamped = clampWall(xx, yy, wall);
				
				// Compute the distance of the input point to the clamped point
				int dx = xx - clamped.x();
				int dy = yy - clamped.y();
				int d2 = dx*dx + dy*dy;
				
				// Check if this distance is less than the current smallest distance
				if (mind2 == -1 || d2 < mind2) {
					// Save the new closest point
					domainClamp = QPoint(
						clamped.x() * model->zoom,
						clamped.y() * model->zoom
					);
					mind2 = d2;
				}
			}
		}
		
		// Return the input point if the closest clamped point is not within clampDist
		Settings* settings = Settings::getInstance();
		if (mind2 > settings->clampDist * settings->clampDist) {
			*clamped = false;
			return QPoint(x, y);
		}
		
		// Return the clamped point otherwise
		*clamped = true;
		return domainClamp;
	}
	
	/**
	 * Clamps a point to the grid and to domain walls. Returns the input point
	 * if there is no wall or grid within clampDist.
	 * 
	 * @param x  The x coordinate of the point to clamp
	 * @param y  The y coordinate of the point to clamp
	 * @param model  A reference to the Model
	 * @param settings  A reference to the Settings instance
	 * @param clampLastDomain  Whether or not to clamp to the last domain as well
	 * @return  The clamped point
	 */
	inline static QPoint clampFull(int x, int y, bool clampLastDomain) {
		// Clamp the input point to the grid
		bool clampedGrid;
		QPoint clampgrid = Grid::clampGrid(x, y, &clampedGrid);
		
		// Clamp the input point to domain walls
		Model* model = ModelManager::getInstance()->getCurrent();
		bool clampedWall;
		QPoint clampwall = Grid::clampDomains(
			x,
			y,
			model->domains,
			&clampedWall,
			clampLastDomain
		);
		
		// If only a wall is within clampDist, clamp to that wall
		if (!clampedGrid && clampedWall) return clampwall;
		
		// If only a grid is within clampDist, clamp to the grid
		if (clampedGrid && !clampedWall) return clampgrid;
		
		// If both a grid and wall are within clampDist, compute the closest
		if (clampedGrid && clampedWall) {
			// Compute the dx and dy to the grid and wall
			int dgx = std::abs(clampgrid.x() - x);
			int dgy = std::abs(clampgrid.y() - y);
			int dwx = std::abs(clampwall.x() - x);
			int dwy = std::abs(clampwall.y() - y);
			
			// Compute the total distance to the grid and wall
			int dg = dgx*dgx + dgy*dgy;
			int dw = dwx*dwx + dwy*dwy;
			
			// Return the closest
			if (dg < dw) return clampgrid;
			return clampwall;
		}
		
		// If neither is within clampDist, return the input point
		return QPoint(x - model->offsetX, y - model->offsetY);
	}
	
	/**
	 * Checks whether or not a given point is on a grid edge.
	 * 
	 * @param x  The x coordinate of the point to test
	 * @param y  The y coordinate of the point to test
	 * @param model  A reference to the Model
	 * @return  Whether or not the given point is on a grid edge
	 */
	inline static bool isOnGrid(int x, int y) {
		// Pixel is not on grid if there is no grid
		Model* model = ModelManager::getInstance()->getCurrent();
		if (!model->showGrid) return false;
		
		QPoint p = mouse2object(QPoint(x, y));
		bool on_x = (p.x() % model->gridsize == 0 && x % model->zoom == 0);
		bool on_y = (p.y() % model->gridsize == 0 && y % model->zoom == 0);
		
		// Return whether or not the given point is on a grid edge
		//bool on_x = (x - model->offsetX) % (model->zoom * model->gridsize) == 0;
		//bool on_y = (y - model->offsetY) % (model->zoom * model->gridsize) == 0;
		return on_x || on_y;
	}
	
	// TODO
	inline static QPoint mouse2object(QPoint mouse) {
		Model* model = ModelManager::getInstance()->getCurrent();
		return QPoint(
			mouse.x() / model->zoom - model->offsetX,
			mouse.y() / model->zoom - model->offsetY
		);
	}
	
	// TODO
	inline static QPoint object2mouse(QPoint object) {
		Model* model = ModelManager::getInstance()->getCurrent();
		return QPoint(
			(object.x() + model->offsetX) * model->zoom,
			(object.y() + model->offsetY) * model->zoom
		);
	}
private:
	/**
	 * Clamps a point to a given wall. Always returns a point on the wall.
	 * 
	 * @param x  The x coordinate of the point to clamp
	 * @param y  The y coordinate of the point to clamp
	 * @param wall  A reference to the wall to clamp to
	 * @return  The clamped point
	 */
	inline static QPoint clampWall(int x, int y, Wall* wall) {
		// Check if the wall is vertical or horizontal
		if (wall->getSide() == LEFT || wall->getSide() == RIGHT) {
			// Get the lowest and highest points of the wall
			int miny = std::min(wall->getMinY(), wall->getMaxY());
			int maxy = std::max(wall->getMinY(), wall->getMaxY());
			
			// Return the lowest point if y < miny
			if (y < miny) return QPoint(wall->getMinX(), miny);
			
			// Return the highest point if y > maxy
			if (y > maxy) return QPoint(wall->getMinX(), maxy);
			
			// Return the same y coordinate otherwise
			return QPoint(wall->getMinX(), y);
		} else {
			// Get the leftmost and rightmost points of the wall
			int minx = std::min(wall->getMinX(), wall->getMaxX());
			int maxx = std::max(wall->getMinX(), wall->getMaxX());
			
			// Return the leftmost point if x < minx
			if (x < minx) return QPoint(minx, wall->getMinY());
			
			// Return the rightmost point if x > maxx
			if (x > maxx) return QPoint(maxx, wall->getMinY());
			
			// Return the same x coordinate otherwise
			return QPoint(x, wall->getMinY());
		}
	}
};

#endif
