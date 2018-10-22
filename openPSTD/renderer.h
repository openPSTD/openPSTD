#ifndef RENDERER_H
#define RENDERER_H

#include <QGraphicsScene>
#include <QSpinBox>
#include <QPainter>
#include <QGraphicsTextItem>
#include <QTime>
#include <iostream>
#include <string>
#include "model.h"
#include "eventhandler/eventhandler.h"
#include "settings.h"
#include "modelmanager.h"
#include "grid.h"
#include "simulator2.h"

/**
 * Handles all drawing and eventhandling of the QGraphicsScene.
 */
class Renderer {
public:
	// Constructor, destructor
	Renderer(QGraphicsScene* scene, QWidget* parent, Simulator2* simulator);
	~Renderer();
	
	// Drawing method
	void draw();
	
	// Event handling methods
	void mousePress(int x, int y, Qt::MouseButton button, Qt::KeyboardModifiers modifiers);
	void mouseRelease(int x, int y, Qt::MouseButton button);
	void mouseDrag(int x, int y, bool drag);

	// Set methods for private variables
	inline void setState(State state) {
		ModelManager::getInstance()->getCurrent()->state = state;
		if (state != MEASURE) eh->removeMeasure();
	}
	void setDimensions(int width, int height);
	
	EventHandler* eh;
	QImage* pixels;
	Simulator2* simulator;
private:
	// Class instance variables
	QGraphicsScene* scene;
	
	// State variables
	int width;
	int height;
	QImage image;
	QFont fpsFont;
	QTime time;
	int numframes;
	int fps;
	bool centered;
	
	// Private drawing methods
	void drawGrid();
	void drawCursor(int x, int y);
	void drawZoom(int zoomaim);
	void drawAxes();
	void drawText(std::string text, int x, int y, int size, QRgb color);
public slots:
	void setGridSize(int gridsize) {
		ModelManager::getInstance()->getCurrent()->gridsize = gridsize;
	}
};

#endif
