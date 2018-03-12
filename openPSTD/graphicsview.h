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
    explicit GraphicsView(QWidget* parent = 0, QSlider* slZoom = 0, QAction* changeabsorption = 0, QAction* showoutput = 0);
    ~GraphicsView();
    
    inline void undo() { modelmanager->undo(); }
    inline void redo() { modelmanager->redo(); }
    
    Renderer* renderer;
    Model* model;
    Simulator* simulator;
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
    ModelManager* modelmanager;
    QSlider* slZoom;
};

#endif