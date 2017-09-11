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

/**
 * Handles all drawing and eventhandling of the QGraphicsScene.
 */
class Renderer {
public:
    // Constructor, destructor
    Renderer(QGraphicsScene* scene, Model* model, Settings* settings, ModelManager* modelmanager);
    ~Renderer();
    
    // Drawing method
    void draw();
    
    // Event handling methods
    void mousePress(int x, int y, Qt::MouseButton button);
    void mouseRelease(int x, int y, Qt::MouseButton button);
    void mouseDrag(int x, int y, bool drag, Qt::KeyboardModifiers modifiers);
    
    // Set methods for private variables
    inline void setState(State state) {
        model->state = state;
    }
    void setDimensions(int width, int height);
    
    // Delete all selected objects
    inline void deleteSelected() { eh->deleteSelected(); }
private:
    // Class instance variables
    QGraphicsScene* scene;
    Model* model;
    Settings* settings;
    ModelManager* modelmanager;
    QImage* pixels;
    EventHandler* eh;
    
    // State variables
    int width;
    int height;
    QImage image;
    QFont fpsFont;
    QTime time;
    int numframes;
    int fps;
    
    // Private drawing methods
    void drawGrid();
    void drawCursor(int x, int y);
    void drawZoom(int zoomaim);
    void drawText(std::string text, int x, int y, int size, QRgb color);
public slots:
    void setGridSize(int gridsize) { model->gridsize = gridsize; }
};

#endif