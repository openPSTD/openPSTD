#ifndef POINT_H
#define POINT_H

#include <QPoint>
#include "modelmanager.h"

enum PointType {
	OBJECT,
	SCREEN
};

class Point {
public:
	Point(QPoint p, PointType type);
	Point(int x, int y, PointType type);
	QPoint getScreen();
	QPoint getObject();
	//QPoint getObject(Model* model);
	Point clampToGrid();
	void setScreen(QPoint p);
	void setObject(QPoint p);
	void convertToObject();
private:
	Model* model;
	
	QPoint object;
	
	QPoint object2screen(QPoint p);
	QPoint screen2object(QPoint p);
};

#endif