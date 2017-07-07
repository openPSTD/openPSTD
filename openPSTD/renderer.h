#ifndef RENDERER_H
#define RENDERER_H

#include <QGraphicsScene>
#include <QSpinBox>
#include <iostream>
#include "state.h"

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
        this->state = state;
    }
    void setDimensions(int width, int height);
private:
    // Class instance variables
    QGraphicsScene* scene;
    
    // State variables
    State state;
    int width;
    int height;
    int gridsize;
    
    // Private drawing methods
    void drawGrid();
public slots:
    void setGridSize(int gridsize) { this->gridsize = gridsize; }
};

#endif