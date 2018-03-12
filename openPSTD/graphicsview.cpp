#include "graphicsview.h"

/**
 * Constructor.
 * Creates a new QGraphicsScene for the QGraphicsView.
 * 
 * @param parent  A reference to the parent widget
 * @param settings  A reference to the Settings instance
 * @param slZoom  A reference to the zoom level slider
 */
GraphicsView::GraphicsView(QWidget* parent, QSlider* slZoom, QAction* changeabsorption, QAction* showoutput) : QGraphicsView(parent) {
    // Create a new QGraphicsScene
    scene = new QGraphicsScene();
    setScene(scene);
    
    // Create a new Model instance
    model = new Model();
    
    // Set initial state
    model->state = SELECT;
    
    // Create a ModelManager instance
    modelmanager = new ModelManager(model);
    
    // Create a Simulator instance
    simulator = new Simulator(model, showoutput);
    
    // Create a new Renderer
    renderer = new Renderer(scene, model, modelmanager, parent, changeabsorption, simulator);
    
    // Enable mouse tracking
    setMouseTracking(true);
    
    // Save the reference to the zoom level slider
    this->slZoom = slZoom;
}

/**
 * Destructor.
 */
GraphicsView::~GraphicsView() {
    // Delete class instance variables
    delete renderer;
    delete simulator;
    delete modelmanager;
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
    renderer->mousePress(point.x(), point.y(), event->button(), event->modifiers());
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
    renderer->mouseDrag(point.x(), point.y(), event->buttons() == Qt::LeftButton, event->modifiers());
}

/**
 * Event listener for mouse double click.
 * 
 * @param event  A reference to the QMouseEvent
 */
void GraphicsView::mouseDoubleClickEvent(QMouseEvent* event) {
    // Get the position of the mouse
    QPointF point = mapToScene(event->pos());
    
    // Delegate the event to Renderer
    renderer->doubleClick(point.x(), point.y(), event->button());
}

/**
 * Event listener for mouse scroll.
 * 
 * @param event  A reference to the QWheelEvent
 */
void GraphicsView::wheelEvent(QWheelEvent* event) {
    // Compute how much the mouse wheel was rotated
    int delta = event->delta() / 120;
    
    // Update the zoom level
    model->zoom += delta;
    if (model->zoom < slZoom->minimum()) model->zoom = slZoom->minimum();
    if (model->zoom > slZoom->maximum()) model->zoom = slZoom->maximum();
    
    // Update the value of the zoom level spinbox
    slZoom->setValue(model->zoom);
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
    
    // Check for DELETE
    if (event->key() == Qt::Key_Delete && event->modifiers() == Qt::NoModifier) {
        // Delete the selected objects
        renderer->eh->deleteSelected();
    }
    
    // Check for ESC
    if (event->key() == Qt::Key_Escape && event->modifiers() == Qt::NoModifier) {
        // Cancel adding a domain
        renderer->eh->cancelNewDomain();
    }
}