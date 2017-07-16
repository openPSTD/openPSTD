#include "graphicsview.h"

/**
 * Constructor.
 * Creates a new QGraphicsScene for the QGraphicsView.
 * 
 * @param parent  A reference to the parent widget
 */
GraphicsView::GraphicsView(QWidget* parent) : QGraphicsView(parent) {
    // Create a new QGraphicsScene
    scene = new QGraphicsScene();
    setScene(scene);
    
    // Create a new Renderer
    renderer = new Renderer(scene);
    
    // Enable mouse tracking
    setMouseTracking(true);
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
        geometry().width() - 4,
        geometry().height() - 4
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
    // Get the position of the mouse
    QPointF point = mapToScene(event->pos());
    
    // Delegate the event to Renderer
    renderer->mousePress(point.x(), point.y(), event->button());
}

/**
 * Event listener for mouse release.
 * 
 * @param event  A reference to the QMouseEvent
 */
void GraphicsView::mouseReleaseEvent(QMouseEvent* event) {
    // Get the position of the mouse
    QPointF point = mapToScene(event->pos());
    
    // Delegate the event to Renderer
    renderer->mouseRelease(point.x(), point.y(), event->button());
}

/**
 * Event listener for mouse drag.
 * 
 * @param event  A reference to the QMouseEvent
 */
void GraphicsView::mouseMoveEvent(QMouseEvent* event) {
    // Get the position of the mouse
    QPointF point = mapToScene(event->pos());
    
    // Delegate the event to Renderer
    renderer->mouseDrag(point.x(), point.y());
}