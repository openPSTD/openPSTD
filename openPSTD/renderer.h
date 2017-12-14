#ifndef RENDERER_H
#define RENDERER_H

#include <QGraphicsScene>
#include <QImage>
#include "eventhandler.h"

/**
 * Handles all rendering to the scene.
 */
class Renderer {
public:
    // Constructor, destructor
    Renderer(EventHandler* eventhandler, QGraphicsScene* scene);
    ~Renderer();
    
    // Draws everything to the scene
    void draw();
    
    // Updates the dimensions of the pixels array
    void setDimensions(int width, int height);
    
    EventHandler* eventhandler;
private:
    // Reference variables
    QGraphicsScene* scene;
    
    // Class instance variables
    QImage* pixels;
    
    // Private drawing methods
    void drawGrid();
    void drawCursor();
    void drawZoomRef();
    void drawDomains();
    void drawSources();
    void drawReceivers();
    void drawSelectionRect();
    void drawMeasure();
};

#endif