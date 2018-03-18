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
Renderer::Renderer(QGraphicsScene* scene, Model* model, ModelManager* modelmanager, QWidget* parent, QAction* changeabsorption, Simulator* simulator) {
    // Save reference variables locally
    this->scene = scene;
    this->model = model;
    this->modelmanager = modelmanager;
    this->simulator = simulator;
    this->centered = false;
    
    // Create an initial pixels array
    pixels = new QImage(1024, 768, QImage::Format_RGB32);
    
    // Update the width and height according to the scene
    width = scene->sceneRect().width();
    height = scene->sceneRect().height();
    
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
    
    // Initialize the state variables
    selectionEnabled = false;
    measureShown = false;
}

/**
 * Destructor.
 */
Renderer::~Renderer() {
    // Delete class instance variables
    delete pixels;
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
            model->offsetY
        );
    }
    
    // Draw all sources
    std::vector<Source> sources = model->sources;
    for (unsigned int i = 0; i < sources.size(); i++) {
        sources[i].draw(pixels,
            model->zoom,
            model->offsetX,
            model->offsetY
        );
    }
    
    // Draw all receivers
    std::vector<Receiver> receivers = model->receivers;
    for (unsigned int i = 0; i < receivers.size(); i++) {
        receivers[i].draw(
            pixels,
            model->zoom,
            model->offsetX,
            model->offsetY
        );
    }
    
    // Draw the selection rectangle
    drawSelection();
    
    // Draw the measure tool
    drawMeasure();
    
    /*// Draw the overlap message text
    eh->drawOverlap(pixels);
    
    // Draw the zero wall message text
    eh->drawZerowall(pixels);*/
    
    // Draw cursor if adding domain or measuring
    if (model->state == ADDDOMAIN || model->state == MEASURE) {
        QPoint clamped = Grid::clampFull(mouse.x(), mouse.y(), model, true);
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
        drawText(std::to_string(fps), 5, height - 19, 14, Settings::getInstance()->fpsColor);
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
    /*if (!centered) {
        eh->moveToCenter();
        centered = true;
    }*/
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
    //eh->mousePress(x, y, button, modifiers);
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
    //eh->mouseRelease(x, y, button);
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
    //eh->mouseDrag(x, y, drag, modifiers);
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
    //eh->doubleClick(x, y, button);
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
    //eh->setWidth(width);
    //eh->setHeight(height);
    
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
                pixels->setPixel(QPoint(x, y), Settings::getInstance()->gridColor);
                continue;
            }
            
            // Background
            pixels->setPixel(QPoint(x, y), Settings::getInstance()->bgColor);
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
 * Draws the selection rectangle.
 */
void Renderer::drawSelection() {
    // Do nothing if the selection is not enabled
    if (!selectionEnabled) return;
    
    // Convert the coordinates to screen coordinates
    int y0 = -selectionRectangle.top() * model->zoom + model->offsetY;
    int y1 = -selectionRectangle.bottom() * model->zoom + model->offsetY;
    int x0 = selectionRectangle.left() * model->zoom + model->offsetX;
    int x1 = selectionRectangle.right() * model->zoom + model->offsetX;
    int top = std::min(y0, y1);
    int bottom = std::max(y0, y1);
    int left = std::min(x0, x1);
    int right = std::max(x0, x1);
    
    // Draw the four edges of the selection rectangle
    QRgb color = qRgb(100, 100, 100);
    for (int x = left; x <= right; x++) {
        if (x < 0 || x >= pixels->width()) continue;
        if (0 <= top && top < pixels->height()) pixels->setPixel(x, top, color);
        if (0 <= top && top < pixels->height()) pixels->setPixel(x, bottom, color);
    }
    for (int y = top; y <= bottom; y++) {
        if (y < 0 || y >= pixels->height()) continue;
        if (0 <= left && left < pixels->width()) pixels->setPixel(left, y, color);
        if (0 <= right && right < pixels->width()) pixels->setPixel(right, y, color);
    }
}

/**
 * Draws the measuring tool.
 */
void Renderer::drawMeasure() {
    // Do nothing if the measure tool is not shown
    if (!measureShown) return;
    
    // Convert to screen coordinates
    int x0 = measure.x1() * model->zoom + model->offsetX;
    int x1 = measure.x2() * model->zoom + model->offsetX;
    int y0 = measure.y1() * model->zoom + model->offsetY;
    int y1 = measure.y2() * model->zoom + model->offsetY;
    
    // Loop through all points on the line
    for (int t = 0; t < 1000; t++) {
        double tt = (double) t / 1000;
        
        int x = tt * x0 + (1 - tt) * x1;
        int y = tt * y0 + (1 - tt) * y1;
        
        pixels->setPixel(x, y, qRgb(0, 0, 255));
    }
    
    // Compute the length of the measure
    int dx2 = (measure.x2()-measure.x1()) * (measure.x2()-measure.x1());
    int dy2 = (measure.y2()-measure.y1()) * (measure.y2()-measure.y1());
    double length = std::sqrt(dx2 + dy2);
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(1) << length;
    std::string lengthtext = oss.str();
    
    // Compute the midpoint of the measure
    int midx = (x0 + x1) / 2;
    int midy = (y0 + y1) / 2;
    
    // Draw the measure length text
    drawText(
        lengthtext,
        midx,
        midy,
        14,
        qRgb(0, 0, 255)
    );
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
        if (i < pixels->width()) pixels->setPixel(i, 5, Settings::getInstance()->zoomColor);
    }
    for (int i = 3; i < 8; i++) {
        pixels->setPixel(5, i, Settings::getInstance()->zoomColor);
        if (5 + width < pixels->width()) pixels->setPixel(5 + width, i, Settings::getInstance()->zoomColor);
    }
    
    // Draw the current zoom level text
    drawText(std::to_string(zoomaim) + " m", 5, 10, 14, Settings::getInstance()->zoomColor);
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
    
    /*// Draw horizontal offset text
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
    );*/
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