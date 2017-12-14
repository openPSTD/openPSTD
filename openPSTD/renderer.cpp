#include "renderer.h"

/**
 * Constructor.
 * 
 * @param eventhandler  A reference to the EventHandler
 * @param scene  A reference to the QGraphicsScene
 */
Renderer::Renderer(EventHandler* eventhandler, QGraphicsScene* scene) {}

/**
 * Destructor.
 */
Renderer::~Renderer() {}

/**
 * Draws everything to the scene.
 */
void Renderer::draw() {
    // Clear the scene
    scene->clear();
    
    // Draw all domains
    std::vector<Domain*> domains = Model::getInstance()->domains;
    for (unsigned int i = 0; i < domains.size(); i++) {
        domains[i]->draw(pixels);
    }
}

/**
 * Updates the dimensions of the pixels array
 * 
 * @param width  The new width for the pixels array
 * @param height  The new height for the pixels array
 */
void Renderer::setDimensions(int width, int height) {}

/**
 * Private drawing method: Background grid.
 */
void Renderer::drawGrid() {}

/**
 * Private drawing method: Cursor.
 */
void Renderer::drawCursor() {}

/**
 * Private drawing method: Zoom level reference.
 */
void Renderer::drawZoomRef() {}

/**
 * Private drawing method: Domains.
 */
void Renderer::drawDomains() {}

/**
 * Private drawing method: Sources.
 */
void Renderer::drawSources() {}

/**
 * Private drawing method: Receivers.
 */
void Renderer::drawReceivers() {}

/**
 * Private drawing method: Selection rectangle.
 */
void Renderer::drawSelectionRect() {}

/**
 * Private drawing method: Measure tool.
 */
void Renderer::drawMeasure() {}