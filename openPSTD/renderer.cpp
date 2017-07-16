#include "renderer.h"

/**
 * Constructor.
 * 
 * @param scene  A reference to the scene to draw to
 */
Renderer::Renderer(QGraphicsScene* scene) {
    // Save reference variables locally
    this->scene = scene;
    
    // Create a new Model instance
    model = new Model();
    
    // Create a new EventHandler instance
    eh = new EventHandler(model);
    
    // Update the width and height according to the scene
    width = scene->sceneRect().width();
    height = scene->sceneRect().height();
    
    // Create an initial pixels array
    pixels = new QImage(1024, 768, QImage::Format_RGB32);
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
    
    // Draw all domains
    std::vector<Domain> domains = model->domains;
    for (int i = 0; i < domains.size(); i++) {
        domains[i].draw(pixels);
    }
    
    // Draw the pixels array
    QPixmap qpm = QPixmap::fromImage(*pixels);
    scene->addPixmap(qpm);
    
    // Reset the pixels array
    pixels = new QImage(width, height, QImage::Format_RGB32);
}

/**
 * Event handler for mouse press.
 * 
 * @param x  The x position of the mouse
 * @param y  The y position of the mouse
 * @param button  The mouse button that was pressed
 */
void Renderer::mousePress(int x, int y, Qt::MouseButton button) {
    // Delegate event to EventHandler
    eh->mousePress(x, y, button);
}

/**
 * Event handler for mouse release.
 * 
 * @param x  The x position of the mouse
 * @param y  The y position of the mouse
 * @param button  The mouse button that was pressed
 */
void Renderer::mouseRelease(int x, int y, Qt::MouseButton button) {
    // Delegate event to EventHandler
    eh->mouseRelease(x, y, button);
}

/**
 * Event handler for mouse drag.
 * 
 * @param x  The x position of the mouse
 * @param y  The y position of the mouse
 */
void Renderer::mouseDrag(int x, int y) {
    // Delegate event to EventHandler
    eh->mouseDrag(x, y);
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
    
    // Create a new pixels array with the new dimensions
    delete pixels;
    pixels = new QImage(width, height, QImage::Format_RGB32);
    
    // Redraw the scene
    draw();
}

/**
 * Draws a background grid on the scene.
 */
void Renderer::drawGrid() {
    // Loop through all pixels
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // Vertical line
            if (x % gridsize == 0) {
                pixels->setPixelColor(QPoint(x, y), gridColor);
                continue;
            }
            
            // Horizontal line
            if (y % gridsize == 0) {
                pixels->setPixelColor(QPoint(x, y), gridColor);
                continue;
            }
            
            // Background
            pixels->setPixelColor(QPoint(x, y), bgColor);
        }
    }
}