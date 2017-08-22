#include "graphicsview.h"

/**
 * Constructor.
 * Creates a new QGraphicsScene for the QGraphicsView.
 * 
 * @param parent  A reference to the parent widget
 * @param settings  A reference to the Settings instance
 */
GraphicsView::GraphicsView(QWidget* parent, Settings* settings) : QGraphicsView(parent) {
    // Create a new QGraphicsScene
    scene = new QGraphicsScene();
    setScene(scene);
    
    // Create a new Model instance
    model = new Model();
    
    // Create a ModelManager instance
    modelmanager = new ModelManager(model);
    
    // Create a new Renderer
    renderer = new Renderer(scene, model, settings, modelmanager);
    
    // Enable mouse tracking
    setMouseTracking(true);
}

/**
 * Destructor.
 */
GraphicsView::~GraphicsView() {
    // Delete class instance variables
    delete renderer;
    delete model;
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

/**
 * Event listener for key press
 * 
 * @param event  A reference to the QKeyEvent
 */
void GraphicsView::keyPressEvent(QKeyEvent* event) {
    // Check for CTRL+Z
    if (event->key() == Qt::Key_Z && event->modifiers() == Qt::ControlModifier) {
        // Undo the last operation
        undo();
    }
    
    // Check for CTRL+SHIFT+Z
    if (event->key() == Qt::Key_Z && event->modifiers() == Qt::ControlModifier + Qt::ShiftModifier) {
        // Redo the last operation
        redo();
    }
    
    // Check for CTRL+Y
    if (event->key() == Qt::Key_Y && event->modifiers() == Qt::ControlModifier) {
        // Redo the last operation
        redo();
    }
}