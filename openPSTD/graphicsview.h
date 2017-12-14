#ifndef GRAPHICSVIEW_H
#define GRAPHICSVIEW_H

#include <QWidget>
#include <QGraphicsView>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QSpinBox>
#include <iostream>
#include "renderer.h"
#include "eventlistener.h"
#include "settings.h"

/**
 * Custom QGraphicsView widget for drawing the main frame.
 */
class GraphicsView : public QGraphicsView {
    Q_OBJECT
public:
    explicit GraphicsView(QWidget* parent = 0, EventListener* eventlistener = 0);
    ~GraphicsView();
    
    Renderer* renderer;
    EventListener* eventlistener;
protected:
    void paintEvent(QPaintEvent* event);
    void resizeEvent(QResizeEvent* event);
public slots:
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseDoubleClickEvent(QMouseEvent* event);
    void wheelEvent(QWheelEvent* event);
    void keyPressEvent(QKeyEvent* event);
private:
    QGraphicsScene* scene;
};

#endif