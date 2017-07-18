#include "renderer.h"

/**
 * Constructor.
 * 
 * @param scene  A reference to the scene to draw to
 * @param model  A reference to the Model instance
 */
Renderer::Renderer(QGraphicsScene* scene, Model* model) {
    // Save reference variables locally
    this->scene = scene;
    this->model = model;
    
    // Create a new EventHandler instance
    eh = new EventHandler(model);
    
    // Update the width and height according to the scene
    width = scene->sceneRect().width();
    height = scene->sceneRect().height();
    
    // Create an initial pixels array
    pixels = new QImage(1024, 768, QImage::Format_RGB32);
    
    // Initialize fps counter
    fpsFont.setPixelSize(18);
    fpsFont.setBold(false);
    fpsFont.setFamily("Monospace");
    numframes = 0;
    fps = 0;
    time.start();
}

/**
 * Destructor.
 */
Renderer::~Renderer() {
    // Delete class instance variables
    delete pixels;
    delete eh;
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
    for (unsigned int i = 0; i < domains.size(); i++) {
        domains[i].draw(pixels);
    }
    
    // Draw cursor if adding domain
    if (model->state == ADDDOMAIN) {
        QPoint pos = eh->getMousePos();
        QPoint clamped = model->clampGrid(pos.x(), pos.y());
        if (clamped != QPoint(-1, -1)) pos = clamped;
        drawCursor(clamped.x(), clamped.y());
    }
    
    // Draw the pixels array
    QPixmap qpm = QPixmap::fromImage(*pixels);
    scene->addPixmap(qpm);
    
    // Reset the pixels array
    delete pixels;
    pixels = new QImage(width, height, QImage::Format_RGB32);
    
    // Update fps
    if (numframes++ >= 20) {
        fps = 20 * 1000.0 / time.elapsed();
        time.restart();
        numframes = 0;
    }
    
    // Draw fps
    if (model->showFPS) {
        QGraphicsTextItem* fpsText = scene->addText(QString(std::to_string(fps).c_str()), fpsFont);
        fpsText->setPos(QPoint(0, height - 22));
    }
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
            // Check if this point is on the grid
            if (model->isOnGrid(x, y)) {
                pixels->setPixel(QPoint(x, y), gridColor);
                continue;
            }
            
            // Background
            pixels->setPixel(QPoint(x, y), bgColor);
        }
    }
}

/**
 * Draws a cursor at the given x, y coordinates.
 * 
 * @param x  The x coordinate at which to draw the cursor
 * @param y  The y coordinate at which to draw the cursor
 */
void Renderer::drawCursor(int x, int y) {
    // Length of each of the cursor's legs
    int d = 5;
    
    // Draw a horizontal line
    for (int i = x - d; i < x + d; i++) {
        if (i < 0 || i >= width) continue;
        if (y < 0 || y >= height) break;
        pixels->setPixel(i, y, cursorColor);
    }
    
    // Draw a vertical line
    for (int j = y - d; j < y + d; j++) {
        if (x < 0 || x >= width) break;
        if (j < 0 || j >= height) continue;
        pixels->setPixel(x, j, cursorColor);
    }
}
