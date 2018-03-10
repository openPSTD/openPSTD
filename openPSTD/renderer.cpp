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
Renderer::Renderer(QGraphicsScene* scene, Model* model, Settings* settings, ModelManager* modelmanager, QWidget* parent, QAction* changeabsorption, Simulator* simulator) {
    // Save reference variables locally
    this->scene = scene;
    this->model = model;
    this->settings = settings;
    this->modelmanager = modelmanager;
    this->simulator = simulator;
    bool centered = false;
    
    // Create a new EventHandler instance
    eh = new EventHandler(model, settings, modelmanager, simulator, parent, changeabsorption);
    
    // Update the width and height according to the scene
    width = scene->sceneRect().width();
    height = scene->sceneRect().height();
    eh->setWidth(width);
    eh->setHeight(height);
    
    // Create an initial pixels array
    pixels = new QImage(1024, 768, QImage::Format_RGB32);
    
    // Load the cursor image
    image = QImage(":/new/prefix1/icons/cursor.png");
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
    
    // Draw axes
    drawAxes();
    
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
    
    // Draw the measure tool
    eh->drawMeasure(pixels);
    
    // Draw the overlap message text
    eh->drawOverlap(pixels);
    
    // Draw the zero wall message text
    eh->drawZerowall(pixels);
    
    // Draw cursor if adding domain or measuring
    if (model->state == ADDDOMAIN || model->state == MEASURE) {
        QPoint pos = eh->getMousePos();
        QPoint clamped = Grid::clampFull(pos.x(), pos.y(), model, settings, true);
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
    
    // Draw the simulator output
    simulator->draw(pixels);
    
    // Draw the pixels array
    QPixmap qpm = QPixmap::fromImage(*pixels);
    scene->addPixmap(qpm);
    
    // Reset the pixels array
    delete pixels;
    pixels = new QImage(width, height, QImage::Format_RGB32);
    
    // Center the scene initially
    if (!centered) {
        moveToCenter();
        centered = true;
    }
}

/**
 * Event handler for mouse press.
 * 
 * @param x  The x position of the mouse
 * @param y  The y position of the mouse
 * @param button  The mouse button that was pressed
 * @param modifiers  The KeyboardModifiers of the mouse press event
 */
void Renderer::mousePress(int x, int y, Qt::MouseButton button, Qt::KeyboardModifiers modifiers) {
    // Delegate event to EventHandler
    eh->mousePress(x, y, button, modifiers);
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
    eh->setWidth(width);
    eh->setHeight(height);
    
    // Create a new pixels array with the new dimensions
    delete pixels;
    pixels = new QImage(width, height, QImage::Format_RGB32);
    
    // Redraw the scene
    draw();
}

/**
 * Updates the zoom level and offset values in model to center the scene.
 */
void Renderer::moveToCenter() {
    // Go to (0, 0) if there are no domains
    if (model->domains.size() == 0) {
        model->offsetX = pixels->width() / 2;
        model->offsetY = pixels->height() / 2;
        return;
    }
    
    // Keep track of the minimum and maximum x and y coordinates
    int minx;
    int maxx;
    int miny;
    int maxy;
    
    // Loop through all domains
    for (unsigned int i = 0; i < model->domains.size(); i++) {
        // Get the min and max coordinates of this domain
        Domain d = model->domains[i];
        int x0 = d.getX0();
        int x1 = d.getX1();
        int y0 = d.getY0();
        int y1 = d.getY1();
        
        // Update the min and max coordinates according to this domain
        if (i == 0 || x0 < minx) minx = x0;
        if (i == 0 || x1 > maxx) maxx = x1;
        if (i == 0 || y0 < miny) miny = y0;
        if (i == 0 || y1 > maxy) maxy = y1;
    }
    
    // Loop through all sources
    for (unsigned int i = 0; i < model->sources.size(); i++) {
        // Get the min and max coordinates of this source
        Source s = model->sources[i];
        int x = s.getX();
        int y = s.getY();
        
        // Update the min and max coordinates according to this source
        if (x < minx) minx = x;
        if (x > maxx) maxx = x;
        if (y < miny) miny = y;
        if (y > maxy) maxy = y;
    }
    
    // Loop through all receivers
    for (unsigned int i = 0; i < model->receivers.size(); i++) {
        // Get the min and max coordinates of this receiver
        Receiver s = model->receivers[i];
        int x = s.getX();
        int y = s.getY();
        
        // Update the min and max coordinates according to this receiver
        if (x < minx) minx = x;
        if (x > maxx) maxx = x;
        if (y < miny) miny = y;
        if (y > maxy) maxy = y;
    }
    
    // Compute the maximum zoom level
    int dx = maxx - minx;
    int dy = maxy - miny;
    int maxzoomx = pixels->width() / (1.1 * dx);
    int maxzoomy = pixels->height() / (1.1 * dy);
    int maxzoom = (maxzoomx < maxzoomy ? maxzoomx : maxzoomy);
    
    // Compute the new offset
    int offsetx = -maxzoom * minx;
    int offsety = -maxzoom * miny;
    int doffsetx = (pixels->width() - dx * maxzoom) / 2;
    int doffsety = (pixels->height() - dy * maxzoom) / 2;
    
    // Save the new zoom level
    model->zoom = maxzoom;
    
    // Save the new offset
    model->offsetX = offsetx + doffsetx;
    model->offsetY = offsety + doffsety;
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
    for (int i = 5; i < 5 + width; i++) {
        if (i < pixels->width()) pixels->setPixel(i, 5, settings->zoomColor);
    }
    for (int i = 3; i < 8; i++) {
        pixels->setPixel(5, i, settings->zoomColor);
        if (5 + width < pixels->width()) pixels->setPixel(5 + width, i, settings->zoomColor);
    }
    
    // Draw the current zoom level text
    drawText(std::to_string(zoomaim) + " m", 5, 10, 14, settings->zoomColor);
}

void Renderer::drawAxes() {
    // Draw horizontal axis at (0, 0)
    for (int i = 0; i < pixels->width(); i++) {
        for (int d = -1; d < 1; d++) {
            if (model->offsetY + d < 0 || model->offsetY >= pixels->height()) continue;
            pixels->setPixel(
                i,
                model->offsetY + d,
                qRgb(0, 0, 255)
            );
        }
    }
    
    // Draw vertical axis at (0, 0)
    for (int j = 0; j < pixels->height(); j++) {
        for (int d = -1; d < 1; d++) {
            if (model->offsetX + d < 0 || model->offsetX >= pixels->width()) continue;
            pixels->setPixel(
                model->offsetX + d,
                j,
                qRgb(0, 0, 255)
            );
        }
    }
    
    // Draw horizontal offset text
    int mousex = (eh->getMousePos().x() - model->offsetX) / model->zoom;
    int mousey = -(eh->getMousePos().y() - model->offsetY) / model->zoom;
    drawText(
        "X: " + std::to_string(mousex),
        0,
        pixels->height() - 36,
        16,
        qRgb(0, 0, 0)
    );
    
    // Draw vertical offset text
    drawText(
        "Y: " + std::to_string(mousey),
        0,
        pixels->height() - 18,
        16,
        qRgb(0, 0, 0)
    );
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