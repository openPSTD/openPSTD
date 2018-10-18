#ifndef GRAPHICSVIEW_H
#define GRAPHICSVIEW_H

#include <QWidget>
#include <QGraphicsView>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QSpinBox>
#include <QSlider>
#include <iostream>
#include "renderer.h"
#include "settings.h"
#include "modelmanager.h"
#include "simulator.h"

/**
 * Custom QGraphicsView widget for drawing the main frame.
 */
class GraphicsView : public QGraphicsView {
	Q_OBJECT
public:
	explicit GraphicsView(
		QWidget* parent = nullptr,
		QSlider* slZoom = nullptr,
		QAction* changeabsorption = nullptr,
		QAction* showoutput = nullptr,
		QStatusBar* statusbar = nullptr
	);
	~GraphicsView();
	
	Renderer* renderer;
	Simulator* simulator;
protected:
	void paintEvent(QPaintEvent* event);
	void resizeEvent(QResizeEvent* event);
public slots:
	void mousePressEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);
	void mouseMoveEvent(QMouseEvent* event);
	void wheelEvent(QWheelEvent* event);
	void keyPressEvent(QKeyEvent* event);
private:
	QGraphicsScene* scene;
	QSlider* slZoom;
	
	void printModel();
};

#endif
