#include "solver.h"

/**
 * Constructor.
 * Initializes the reference variables.
 * 
 * @param conf  A reference to the Configuration instance to solve
 */
Solver::Solver(Configuration* conf) {
	// Initialize the reference variables
	this->conf = conf;
	
	// Initialize the PML domains
	initializePMLDomains();
}

/**
 * Computes a single frome.
 */
void Solver::computeFrame() {
	// Compute the delta p values
	computeDeltaP();
	
	// Compute the Linearized Euler Equations
	computeLEE();
	
	// Apply the windowing function
	applyWindowing();
}

/**
 * Returns the number of frames to be computed.
 * 
 * @return  The number of frames to be computed
 */
unsigned int Solver::getNumFrames() {
	// Get the settings variables from the Configuration instance
	double renderTime = conf->renderTime;
	int soundSpeed = conf->soundSpeed;
	double gridSpacing = conf->gridSpacing;
	
	// Number of frames = render time * sound speed / (0.5 * grid spacing)
	// TODO: Is the 0.5 is the computation below always 0.5, or related to some variable ?
	return static_cast<unsigned int>(renderTime * soundSpeed / (0.5 * gridSpacing));
}

/**
 * Private computation method: Initialize the PML domains.
 */
void Solver::initializePMLDomains() {}

/**
 * Private computation method: Compute the delta p values.
 */
void Solver::computeDeltaP() {}

/**
 * Private computation method: Compute the Linearized Euler Equations.
 */
void Solver::computeLEE() {}

/**
 * Private computation method: Apply the windowing function.
 */
void Solver::applyWindowing() {}