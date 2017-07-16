#ifndef RENDERER_H
#define RENDERER_H

#include <QGraphicsScene>
#include <QSpinBox>
#include <iostream>
#include "model.h"
#include "eventhandler.h"

/**
 * Handles all drawing and eventhandling of the QGraphicsScene.
 */
class Renderer {
public:
    // Constructor
    Renderer(QGraphicsScene* scene);
    
    // Drawing method
    void draw();
    
    // Event handling methods
    void mousePress(int x, int y, Qt::MouseButton button);
    void mouseRelease(int x, int y, Qt::MouseButton button);
    void mouseDrag(int x, int y);
    
    // Set methods for private variables
    inline void setState(State state) {
        std::cout << "State: " << state << std::endl;
        model->state = state;
    }
    void setDimensions(int width, int height);
private:
    // Class instance variables
    QGraphicsScene* scene;
    QImage* pixels;
    Model* model;
    EventHandler* eh;
    
    // Settings variables
    QColor gridColor = QColor(Qt::GlobalColor::red);
    QColor bgColor = QColor(Qt::GlobalColor::white);
    
    // State variables
    int width;
    int height;
    int gridsize;
    
    // Private drawing methods
    void drawGrid();
public slots:
    void setGridSize(int gridsize) { this->gridsize = gridsize; }
};

#endif