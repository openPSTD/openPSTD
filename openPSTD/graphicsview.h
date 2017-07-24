#ifndef GRAPHICSVIEW_H
#define GRAPHICSVIEW_H

#include <QWidget>
#include <QGraphicsView>
#include <QMouseEvent>
#include <QSpinBox>
#include <iostream>
#include "renderer.h"
#include "settings.h"

/**
 * Custom QGraphicsView widget for drawing the main frame.
 */
class GraphicsView : public QGraphicsView {
    Q_OBJECT
public:
    explicit GraphicsView(QWidget* parent = 0, Settings* settings = 0);
    ~GraphicsView();
    
    Renderer* renderer;
    Model* model;
protected:
    void paintEvent(QPaintEvent* event);
    void resizeEvent(QResizeEvent* event);
public slots:
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
private:
    QGraphicsScene* scene;
};

#endif