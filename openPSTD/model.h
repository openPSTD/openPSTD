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
	bool ctrlPressed = false;
	
	// PSTD settings
	double gridspacing = 0.2;
	int windowsize = 32;
	double rendertime = 1;
	double airdensity = 1.2;
	int soundspeed = 340;
	int pmlcells = 50;
	int attenuationpmlcells = 20000;
	
	// Simulator state
	bool simulating = false;
    bool hasSimulationOutput = false;
	unsigned int simulatorHeight = 0; // 0 if hidden, height otherwise
	
	// Window actions
	QAction* actionSelect_Domain;
	QAction* actionSelect;
	QAction* actionMoveScene;
	QAction* actionAddDomain;
	QAction* actionAddSource;
	QAction* actionAddReceiver;
	QAction* actionMeasure;
	QAction* actionUndo;
	QAction* actionRedo;
	QAction* actionMoveToCenter;
	QAction* actionChangeAbsorption;
	QAction* actionShow_Output;
	QAction* actionShowSidebar;
	QAction* actionStart;
	QAction* actionStop;
	QAction* actionRemoveSimulation;
};

#endif