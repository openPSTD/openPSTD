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
#include "eventhandler.h"
#include "settings.h"
#include "modelmanager.h"
#include "grid.h"
#include "simulator.h"

/**
 * Handles all drawing and eventhandling of the QGraphicsScene.
 */
class Renderer {
public:
	// Constructor, destructor
	Renderer(
		QGraphicsScene* scene,
		Model* model,
		ModelManager* modelmanager,
		QWidget* parent,
		QAction* changeabsorption,
		Simulator* simulator
	);
	~Renderer();
	
	// Drawing method
	void draw();
	
	// Event handling methods
	void mousePress(int x, int y, Qt::MouseButton button, Qt::KeyboardModifiers modifiers);
	void mouseRelease(int x, int y, Qt::MouseButton button);
	void mouseDrag(int x, int y, bool drag, Qt::KeyboardModifiers modifiers);

	// Set methods for private variables
	inline void setState(State state) {
		model->state = state;
		if (state != MEASURE) eh->removeMeasure();
	}
	void setDimensions(int width, int height);
	
	EventHandler* eh;
	QImage* pixels;
	Simulator* simulator;
private:
	// Class instance variables
	QGraphicsScene* scene;
	Model* model;
	ModelManager* modelmanager;
	
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
	void setGridSize(int gridsize) { model->gridsize = gridsize; }
};

#endif
