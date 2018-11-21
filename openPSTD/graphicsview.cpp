#include "graphicsview.h"

/**
 * Constructor.
 * Creates a new QGraphicsScene for the QGraphicsView.
 * 
 * @param parent  A reference to the parent widget
 * @param settings  A reference to the Settings instance
 * @param slZoom  A reference to the zoom level slider
 */
GraphicsView::GraphicsView(
	QWidget* parent,
	QSlider* slZoom,
	Simulator2* simulator
) : QGraphicsView(parent) {
	// Create a new QGraphicsScene
	scene = new QGraphicsScene();
	setScene(scene);
	
	// Create a new Renderer
	renderer = new Renderer(scene, parent, simulator);
	
	// Enable mouse tracking
	setMouseTracking(true);
	
	// Save the reference to the zoom level slider
	this->slZoom = slZoom;
	
	this->simulator = simulator;
	offsetInitialized = false;
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
	
	// Reset the scene offset
	if (!offsetInitialized) {
		Model* model = ModelManager::getInstance()->getCurrent();
		model->offsetX = 7;
		model->offsetY = (geometry().height() - 4) / model->zoom - 6;
		offsetInitialized = true;
	}
}

/**
 * Generic event method inheritted from QObject.
 * Needed to capture keypresses of only a modifier key,
 * specifically needed for the CTRL key for disabling
 * clamping of the mouse temporarily.
 * 
 * @param event  A reference to the QEvent
 * 
 * @return  Whether or not the event has been processed
 */
bool GraphicsView::event(QEvent* event) {
	// Pass the event to the parent
	QGraphicsView::event(event);
	
	// Check if the event is a keypress
	if (event->type() == QEvent::KeyPress) {
		// Check if the CTRL key is currently pressed, and
		// update its state in the Model
		bool ctrl = ((QKeyEvent*) event)->modifiers() == Qt::ControlModifier;
		Model* model = ModelManager::getInstance()->getCurrent();
		model->ctrlPressed = ctrl;
		
		// Fire a mouse move event to update the eventhandler
		renderer->mouseDrag(
			mousePosition.x(),
			mousePosition.y(),
			QApplication::mouseButtons() == Qt::LeftButton
		);
	}
	
	// Check if the event is a keyrelease
	if (event->type() == QEvent::KeyRelease) {
		// Check if the CTRL key is currently pressed, and
		// update its state in the Model
		bool ctrl = ((QKeyEvent*) event)->modifiers() == Qt::ControlModifier;
		Model* model = ModelManager::getInstance()->getCurrent();
		model->ctrlPressed = ctrl;
		
		// Fire a mouse move event to update the eventhandler
		renderer->mouseDrag(
			mousePosition.x(),
			mousePosition.y(),
			QApplication::mouseButtons() == Qt::LeftButton
		);
	}
	
	// Don't register any processing of the event, such
	// that propagation of the event will continue normally
	return false;
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
	renderer->mousePress(
		static_cast<int>(point.x()),
		static_cast<int>(point.y()),
		event->button(),
		event->modifiers()
	);
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
	renderer->mouseRelease(
		static_cast<int>(point.x()),
		static_cast<int>(point.y()),
		event->button()
	);
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
	renderer->mouseDrag(
		static_cast<int>(point.x()),
		static_cast<int>(point.y()),
		event->buttons() == Qt::LeftButton
	);
	
	// Save the new mouse position
	this->mousePosition = event->pos();
}

/**
 * Event listener for mouse scroll.
 * 
 * @param event  A reference to the QWheelEvent
 */
void GraphicsView::wheelEvent(QWheelEvent* event) {
	// Compute how much the mouse wheel was rotated
	int delta = event->delta() / 120;
	
	// Save the current mouse position
	QPoint mouse_screen = mousePosition;
	QPoint mouse_object = Point(mouse_screen, SCREEN).getObject();
	
	// Update the zoom level
	Model* model = ModelManager::getInstance()->getCurrent();
	model->zoom += delta;
	if (model->zoom < slZoom->minimum()) model->zoom = slZoom->minimum();
	if (model->zoom > slZoom->maximum()) model->zoom = slZoom->maximum();
	
	// Update the offset such that zooming is centered on the mouse
	model->offsetX = mouse_screen.x() / model->zoom - mouse_object.x();
	model->offsetY = mouse_screen.y() / model->zoom + mouse_object.y();
	
	// Update the value of the zoom level spinbox
	slZoom->setValue(model->zoom);
}

/**
 * Event listener for key press
 * 
 * @param event  A reference to the QKeyEvent
 */
void GraphicsView::keyPressEvent(QKeyEvent* event) {
	// Get key and modifiers
	int key = event->key();
	Qt::KeyboardModifiers modifiers = event->modifiers();

	// Check for CTRL+Z
	if (key == Qt::Key_Z && modifiers == Qt::ControlModifier) {
		// Undo the last operation
		ModelManager::getInstance()->undo();
		renderer->draw();
	}
	
	// Check for CTRL+SHIFT+Z
	if (key == Qt::Key_Z && modifiers == Qt::ControlModifier + Qt::ShiftModifier) {
		// Redo the last operation
		ModelManager::getInstance()->redo();
		renderer->draw();
	}
	
	// Check for CTRL+Y
	if (key == Qt::Key_Y && modifiers == Qt::ControlModifier) {
		// Redo the last operation
		ModelManager::getInstance()->redo();
		renderer->draw();
	}
	
	// Check for DELETE
	if (key == Qt::Key_Delete && modifiers == Qt::NoModifier) {
		// Delete the selected objects
		renderer->eh->deleteSelected();
		renderer->draw();
	}
	
	// Check for ESC
	if (key == Qt::Key_Escape && modifiers == Qt::NoModifier) {
		// Cancel adding a domain
		renderer->eh->cancelNewDomain();
		renderer->draw();
	}
	
	// Check for F12
	if (key == Qt::Key_F12 && modifiers == Qt::NoModifier) {
		// Print all domains, sources, and receivers to stdout
		printModel();
	}
	
	// Check for CTRL+Q
	if (key == Qt::Key_Q && modifiers == Qt::ControlModifier) {
		// Close the application
		QApplication::quit();
	}
	
	// Check for CTRL+R
	if (key == Qt::Key_R && modifiers == Qt::ControlModifier) {
		// Run the simulation
		simulator->start();
	}
}

void GraphicsView::printModel() {
	// Print all domains
	Model* model = ModelManager::getInstance()->getCurrent();
	std::cout << std::endl;
	std::cout << "---Printing all domains, sources, receivers---" << std::endl;
	std::cout << "Domains:" << std::endl;
	for (unsigned int i = 0; i < model->domains.size(); i++) {
		int x0 = model->domains[i]->getMinX();
		int x1 = model->domains[i]->getMaxX();
		int y0 = model->domains[i]->getMinY();
		int y1 = model->domains[i]->getMaxY();
		double al = *(model->domains[i]->getAbsorption(LEFT));
		double ar = *(model->domains[i]->getAbsorption(RIGHT));
		double at = *(model->domains[i]->getAbsorption(TOP));
		double ab = *(model->domains[i]->getAbsorption(BOTTOM));
		bool el = *(model->domains[i]->getEdgeLocalReacting(LEFT));
		bool er = *(model->domains[i]->getEdgeLocalReacting(RIGHT));
		bool et = *(model->domains[i]->getEdgeLocalReacting(TOP));
		bool eb = *(model->domains[i]->getEdgeLocalReacting(BOTTOM));
		std::cout << "  " << i << ": (" << x0 << ", " << y0 << ") to (";
		std::cout << x1 << ", " << y1 << ")" << std::endl;
		std::cout << "    ELR left:   " << el << "    Absorption left:   " << al << std::endl;
		std::cout << "    ELR right:  " << er << "    Absorption right:  " << ar << std::endl;
		std::cout << "    ELR top:    " << et << "    Absorption top:    " << at << std::endl;
		std::cout << "    ELR bottom: " << eb << "    Absorption bottom: " << ab << std::endl;
		std::cout << "    Walls:" << std::endl;
		std::vector<Wall*>* walls = model->domains[i]->getWalls();
		for (unsigned int j = 0; j < walls->size(); j++) {
			Side side = walls->at(j)->getSide();
			std::string sd;
			if (side == LEFT)   sd = "LEFT:   ";
			if (side == RIGHT)  sd = "RIGHT:  ";
			if (side == TOP)    sd = "TOP:    ";
			if (side == BOTTOM) sd = "BOTTOM: ";
			int x0 = walls->at(j)->getMinX();
			int x1 = walls->at(j)->getMaxX();
			int y0 = walls->at(j)->getMinY();
			int y1 = walls->at(j)->getMaxY();
			bool selected = walls->at(j)->getSelected();
			std::cout << "      " << sd << "(" << x0 << ", " << y0 << ") to (";
			std::cout << x1 << ", " << y1 << ") (selected: " << selected << ")" << std::endl;
		}
	}
	
	// Print all sources
	std::cout << "Sources:" << std::endl;
	for (unsigned int i = 0; i < model->sources.size(); i++) {
		int x = model->sources[i]->getX();
		int y = model->sources[i]->getY();
		std::cout << "  " << i << ": (" << x << ", " << y << ")" << std::endl;
	}
	
	// Print all receivers
	std::cout << "Receivers:" << std::endl;
	for (unsigned int i = 0; i < model->receivers.size(); i++) {
		int x = model->receivers[i]->getX();
		int y = model->receivers[i]->getY();
		std::cout << "  " << i << ": (" << x << ", " << y << ")" << std::endl;
	}
	std::cout << "---Done printing all domains, sources, receivers---" << std::endl;
	std::cout << std::endl;
}