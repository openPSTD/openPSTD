#ifndef GRAPHICSVIEW_H
#define GRAPHICSVIEW_H

#include <QWidget>
#include <QGraphicsView>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QSpinBox>
#include <QSlider>
#include <QApplication>
#include <iostream>
#include "renderer.h"
#include "settings.h"
#include "modelmanager.h"
#include "simulator2.h"

/**
 * Custom QGraphicsView widget for drawing the main frame.
 */
class GraphicsView : public QGraphicsView {
	Q_OBJECT
public:
	explicit GraphicsView(
		QWidget* parent = nullptr,
		QSlider* slZoom = nullptr,
		Simulator2* simulator = nullptr
	);
	~GraphicsView();
	
	Renderer* renderer;
protected:
	void paintEvent(QPaintEvent* event);
	void resizeEvent(QResizeEvent* event);
	bool event(QEvent* event);
public slots:
	void mousePressEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);
	void mouseMoveEvent(QMouseEvent* event);
	void wheelEvent(QWheelEvent* event);
	void keyPressEvent(QKeyEvent* event);
private:
	QGraphicsScene* scene;
	QSlider* slZoom;
	Simulator2* simulator;
	QPoint mousePosition;
	
	bool offsetInitialized;
	
	void printModel();
};

#endif
