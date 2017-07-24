#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <math.h>
#include "state.h"
#include "domain.h"
#include "settings.h"

/**
 * Representation of the current scene.
 */
class Model {
public:
    // Constructor
    Model() {}
    
    // State of the main tool bar buttons
    State state;
    
    // Vector of all domains
    std::vector<Domain> domains;
    Domain* lastDomain() { return &domains[domains.size() - 1]; }
    
    // Size of the background grid
    int gridsize; // In mm
    QPoint clampGrid(int x, int y);
    bool isOnGrid(int x, int y);
    
    // Zoom level
    int zoom; // In mm / pixel
    
    // FPS counter
    bool showFPS;
};

#endif