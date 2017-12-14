#include "graphicsview.h"

/**
 * Constructor.
 * Creates a new QGraphicsScene for the QGraphicsView.
 * 
 * @param parent  A reference to the parent widget
 * @param eventlistener  A reference to the EventListener
 */
GraphicsView::GraphicsView(QWidget* parent, EventListener* eventlistener) : QGraphicsView(parent) {
    // Create a new QGraphicsScene
    scene = new QGraphicsScene();
    setScene(scene);
    
    // Create a new Renderer
    renderer = new Renderer(eventlistener->eventhandler, scene);
    
    // Enable mouse tracking
    setMouseTracking(true);
    
    // Save reference to the EventListener instance
    this->eventlistener = eventlistener;
}

/**
 * Destructor.
 */
GraphicsView::~GraphicsView() {
    // Delete class instance variables
    delete renderer;
    delete scene;
}

/**
 * PaintEvent for the QGraphicsView widget.
 * 
 * @param event  A reference to the QPaintEvent
 */
void GraphicsView::paintEvent(QPaintEvent* event) {
    // Pass the new dimensions to the renderer
    QGraphicsView::paintEvent(event);
    renderer->setDimensions(
        geometry().width() - 2,
        geometry().height() - 2
    );
}

/**
 * ResizeEvent for the QGraphicsView widget
 * 
 * @param event  A reference to the QResizeEvent
 */
void GraphicsView::resizeEvent(QResizeEvent* event) {
    // Pass the new dimensions to the renderer
    QGraphicsView::resizeEvent(event);
    renderer->setDimensions(
        geometry().width() - 4,
        geometry().height() - 4
    );
}

/**
 * Event listener for mouse click.
 * 
 * @param event  A reference to the QMouseEvent
 */
void GraphicsView::mousePressEvent(QMouseEvent* event) {
    // Delegate the event to EventListener
    eventlistener->mousePress(event);
}

/**
 * Event listener for mouse release.
 * 
 * @param event  A reference to the QMouseEvent
 */
void GraphicsView::mouseReleaseEvent(QMouseEvent* event) {
    // Delegate the event to EventListener
    eventlistener->mouseRelease(event);
}

/**
 * Event listener for mouse drag.
 * 
 * @param event  A reference to the QMouseEvent
 */
void GraphicsView::mouseMoveEvent(QMouseEvent* event) {
    // Delegate the event to EventListener
    eventlistener->mouseMove(event);
}

/**
 * Event listener for mouse double click.
 * 
 * @param event  A reference to the QMouseEvent
 */
void GraphicsView::mouseDoubleClickEvent(QMouseEvent* event) {
    // Delegate the event to EventListener
    eventlistener->doubleClick(event);
}

/**
 * Event listener for mouse scroll.
 * 
 * @param event  A reference to the QWheelEvent
 */
void GraphicsView::wheelEvent(QWheelEvent* event) {
    // Delegate the event to EventListener
    eventlistener->mouseWheel(event);
}

/**
 * Event listener for key press
 * 
 * @param event  A reference to the QKeyEvent
 */
void GraphicsView::keyPressEvent(QKeyEvent* event) {
    // Delegate the event to EventListener
    eventlistener->keyPressed(event);
}