#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <QPoint>
#include "modelmanager.h"

class Domain;
class Source;
class Receiver;

// State enum
enum State {
    SELECT,
    SELECTDOMAIN,
    MOVESCENE,
    ADDDOMAIN,
    ADDSOURCE,
    ADDRECEIVER,
    MEASURE
};

/**
 * Representation of the current scene.
 */
class Model {
public:
    // Constructor, destructor
    Model();
    ~Model();
    
    // Get method for current Model instance
    static Model* getInstance();
    
    // Current state of the toolbar buttons
    State state;
    
    // Vector of all domains
    std::vector<Domain*> domains;
    
    // Vector of all sources
    std::vector<Source*> sources;
    
    // Vector of all receivers
    std::vector<Receiver*> receivers;
    
    // Size of the background grid (as a multiple of the PSTD grid size in mm)
    unsigned int gridsize;
    
    // Zoom level (in mm / pixel)
    unsigned int zoomlevel;
    
    // Offset of the scene (in mm)
    QPoint offset;
    
    // Whether or not to draw the current FPS
    bool showFPS;
    
    // Whether or not to draw the background grid
    bool showGrid;
};

#endif