#include "renderer.h"

/**
 * Constructor.
 * 
 * @param scene  A reference to the scene to draw to
 */
Renderer::Renderer(QGraphicsScene* scene) {
    // Save reference variables locally
    this->scene = scene;
}

/**
 * Drawing method.
 * Redraws the scene.
 */
void Renderer::draw() {
    // Clear the scene
    scene->clear();
    
    // Draw the background grid
    drawGrid();
}

/**
 * Event handler for mouse press.
 * 
 * @param x  The x position of the mouse
 * @param y  The y position of the mouse
 * @param button  The mouse button that was pressed
 */
void Renderer::mousePress(int x, int y, Qt::MouseButton button) {
    if (button == Qt::LeftButton) {
        std::cout << "press" << std::endl;
    }
}

/**
 * Event handler for mouse release.
 * 
 * @param x  The x position of the mouse
 * @param y  The y position of the mouse
 * @param button  The mouse button that was pressed
 */
void Renderer::mouseRelease(int x, int y, Qt::MouseButton button) {
    if (button == Qt::LeftButton) {
        std::cout << "release" << std::endl;
    }
}

/**
 * Event handler for mouse drag.
 * 
 * @param x  The x position of the mouse
 * @param y  The y position of the mouse
 */
void Renderer::mouseDrag(int x, int y) {
    if (state == MOVE) {
        std::cout << "drag" << std::endl;
    }
}

/**
 * Sets the dimensions of the scene to match the given dimensions.
 * 
 * @param width  The width to assign to the scene
 * @param height  The height to assign to the scene
 */
void Renderer::setDimensions(int width, int height) {
    // Save the new dimensions locally
    this->width = width;
    this->height = height;
    
    // Update the dimension of the scene to match the QGraphicsView
    scene->setSceneRect(0, 0, width, height);
    
    // Redraw the scene
    draw();
}

/**
 * Draws a background grid on the scene.
 */
void Renderer::drawGrid() {
    // Draw all horizontal lines
    for (int i = 0; i < height; i += gridsize) {
        scene->addLine(0, i, width, i);
    }
    
    // Draw all vertical lines
    for (int i = 0; i < width; i += gridsize) {
        scene->addLine(i, 0, i, height);
    }
}