#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <math.h>
#include <QAction>
#include "state.h"
#include "source.h"
#include "receiver.h"
#include "settings.h"

class Domain;

/**
 * Representation of the current scene.
 */
class Model {
public:
	// Constructor
	Model() {
		offsetX = 0;
		offsetY = 0;
	}
	
	// State of the main tool bar buttons
	State state;
	
	// Vector of all domains
	std::vector<Domain*> domains;
	Domain* lastDomain() { return domains[domains.size() - 1]; }
	
	// Vector of all sources
	std::vector<Source*> sources;
	
	// Vector of all receivers
	std::vector<Receiver*> receivers;
	
	// Size of the background grid
	int gridsize; // Multiple of PSTD grid size
	
	// Zoom level
	int zoom; // In pixels / m
	
	// Scene offset
	int offsetX; // In pixels
	int offsetY; // In pixels
	
	// FPS counter
	bool showFPS = false;
	
	// Grid
	bool showGrid = true;
	
	QAction* actionUndo;
	QAction* actionRedo;
	QAction* actionChangeAbsorption;
	bool simulating = false;
};

#endif