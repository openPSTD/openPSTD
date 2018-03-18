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
    model->zoom = 16;
    model->offsetX = 0;
    model->offsetY = 0;
    
    // Create a ModelManager instance
    modelmanager = new ModelManager(model);
    
    // Create a Simulator instance
    simulator = new Simulator(model, showoutput);
    
    // Create a new Renderer
    renderer = new Renderer(scene, model, modelmanager, parent, changeabsorption, simulator);
    
    // Create an EventListener
    eventlistener = new EventListener(model, renderer->pixels, parent, renderer);
    
    // Enable mouse tracking
    setMouseTracking(true);
    
    // Save the reference to the zoom level slider
    this->slZoom = slZoom;
    
    // Center the scene initially
    eventlistener->getEventHandler()->moveToCenter();
}

/**
 * Destructor.
 */
GraphicsView::~GraphicsView() {
    // Delete class instance variables
    delete eventlistener;
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
    QPoint point = mapToScene(event->pos()).toPoint();
    
    // Delegate the event to EventListener
    eventlistener->mousePress(point, event->button(), event->modifiers());
}

/**
 * Event listener for mouse release.
 * 
 * @param event  A reference to the QMouseEvent
 */
void GraphicsView::mouseReleaseEvent(QMouseEvent* event) {
    // Get the position of the mouse
    QPoint point = mapToScene(event->pos()).toPoint();
    
    // Delegate the event to EventListener
    eventlistener->mouseRelease(point, event->button());
}

/**
 * Event listener for mouse drag.
 * 
 * @param event  A reference to the QMouseEvent
 */
void GraphicsView::mouseMoveEvent(QMouseEvent* event) {
    // Get the position of the mouse
    QPoint point = mapToScene(event->pos()).toPoint();
    
    // Delegate the event to EventListener
    eventlistener->mouseMove(point, event->modifiers());
}

/**
 * Event listener for mouse double click.
 * 
 * @param event  A reference to the QMouseEvent
 */
void GraphicsView::mouseDoubleClickEvent(QMouseEvent* event) {
    // Get the position of the mouse
    QPoint point = mapToScene(event->pos()).toPoint();
    
    // Delegate the event to EventListener
    eventlistener->doubleClick(point, event->button(), event->modifiers());
}

/**
 * Event listener for mouse scroll.
 * 
 * @param event  A reference to the QWheelEvent
 */
void GraphicsView::wheelEvent(QWheelEvent* event) {
    // Get the position of the mouse
    QPoint point = mapToScene(event->pos()).toPoint();
    
    // Compute the rotation of the mouse wheel
    int dx = event->angleDelta().x() / 120;
    int dy = event->angleDelta().y() / 120;
    
    // Delegate the event to EventListener
    eventlistener->mouseWheel(point, dx, dy);
}

/**
 * Event listener for key press
 * 
 * @param event  A reference to the QKeyEvent
 */
void GraphicsView::keyPressEvent(QKeyEvent* event) {
    // Delegate the event to EventListener
    eventlistener->keyPress(event->key(), event->modifiers());
}