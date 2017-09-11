#include "renderer.h"

/**
 * Constructor.
 * 
 * @param scene  A reference to the scene to draw to
 * @param model  A reference to the Model instance
 * @param settings  A reference to the Settings instance
 * @param modelmanager  A reference to the ModelManager instance
 * @param parent  A reference to the main window
 */
Renderer::Renderer(QGraphicsScene* scene, Model* model, Settings* settings, ModelManager* modelmanager, QWidget* parent) {
    // Save reference variables locally
    this->scene = scene;
    this->model = model;
    this->settings = settings;
    this->modelmanager = modelmanager;
    
    // Create a new EventHandler instance
    eh = new EventHandler(model, settings, modelmanager, parent);
    
    // Update the width and height according to the scene
    width = scene->sceneRect().width();
    height = scene->sceneRect().height();
    
    // Create an initial pixels array
    pixels = new QImage(1024, 768, QImage::Format_RGB32);
    
    // Load the cursor image
    image = QImage("../icons/cursor.png");
    if (image.isNull()) std::cerr << "Cursor image not found" << std::endl;
    
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
        domains[i].draw(
            pixels,
            model->zoom,
            model->offsetX,
            model->offsetY,
            eh->getSelectedWalls(i)
        );
    }
    
    // Draw all sources
    std::vector<Source> sources = model->sources;
    for (unsigned int i = 0; i < sources.size(); i++) {
        sources[i].draw(pixels,
            model->zoom,
            model->offsetX,
            model->offsetY,
            eh->isSourceSelected(i)
        );
    }
    
    // Draw all receivers
    std::vector<Receiver> receivers = model->receivers;
    for (unsigned int i = 0; i < receivers.size(); i++) {
        receivers[i].draw(
            pixels,
            model->zoom,
            model->offsetX,
            model->offsetY,
            eh->isReceiverSelected(i)
        );
    }
    
    // Draw the selection rectangle
    eh->drawSelection(pixels);
    
    // Draw cursor if adding domain
    if (model->state == ADDDOMAIN) {
        QPoint pos = eh->getMousePos();
        QPoint clamped = Grid::clampGrid(pos.x(), pos.y(), model, settings);
        if (clamped != QPoint(-1, -1)) pos = clamped;
        drawCursor(clamped.x() + model->offsetX, clamped.y() + model->offsetY);
    }
    
    // Draw zoom level reference
    int zoomaim = model->gridsize - model->gridsize % 10;
    if (zoomaim == 0) zoomaim = 10;
    drawZoom(zoomaim);
    
    // Update fps
    if (numframes++ >= 20) {
        fps = 20 * 1000.0 / time.elapsed();
        time.restart();
        numframes = 0;
    }
    
    // Draw fps
    if (model->showFPS) {
        drawText(std::to_string(fps), 5, height - 19, 14, settings->fpsColor);
    }
    
    // Draw the pixels array
    QPixmap qpm = QPixmap::fromImage(*pixels);
    scene->addPixmap(qpm);
    
    // Reset the pixels array
    delete pixels;
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
 * @param drag  Whether or not the left mouse button is pressed
 * @param modifiers  The KeyboardModifiers of the mouse drag event
 */
void Renderer::mouseDrag(int x, int y, bool drag, Qt::KeyboardModifiers modifiers) {
    // Delegate event to EventHandler
    eh->mouseDrag(x, y, drag, modifiers);
}

/**
 * Event handler for mouse double click.
 * 
 * @param x  The x position of the mouse
 * @param y  The y position of the mouse
 * @param button  The mouse button that was pressed
 */
void Renderer::doubleClick(int x, int y, Qt::MouseButton button) {
    // Delegate event to EventHandler
    eh->doubleClick(x, y, button);
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
            if (Grid::isOnGrid(x, y, model)) {
                pixels->setPixel(QPoint(x, y), settings->gridColor);
                continue;
            }
            
            // Background
            pixels->setPixel(QPoint(x, y), settings->bgColor);
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
    // Draw the cursor image at the given coordinates
    QPainter p;
    p.begin(pixels);
    p.drawImage(x-5, y-5, image);
    p.end();
}

/**
 * Draws a zoom level reference with given size.
 * 
 * @param zoomaim  The size for the zoom level reference
 */
void Renderer::drawZoom(int zoomaim) {
    // Draw the zoom level reference line
    int width = model->zoom * zoomaim;
    for (int i = 0; i < width; i++) {
        pixels->setPixel(i, 5, settings->zoomColor);
    }
    
    // Draw the current zoom level text
    drawText(std::to_string(zoomaim) + " m", 5, 10, 14, settings->zoomColor);
}

/**
 * Draws a string to the current pixels array.
 * 
 * @param text  The text to draw
 * @param x  The x position to draw at
 * @param y  The y position to draw at
 * @param size  The font size to draw with
 * @param color  The color to draw in
 */
void Renderer::drawText(std::string text, int x, int y, int size, QRgb color) {
    QPainter p;
    p.begin(pixels);
    p.setPen(QPen(color));
    p.setFont(QFont("Monospace", size));
    p.drawText(x, y + size, QString(text.c_str()));
    p.end();
}