#ifndef SOLVER_H
#define SOLVER_H

#include "configuration.h"

/**
 * Implementation of the PSTD solver.
 */
class Solver {
public:
	// Constructor.
	Solver(Configuration* conf);
	
	// Computes a single frame.
	void computeFrame();
	
	// Returns the number of frames to be computed
	unsigned int getNumFrames();
private:
	// Reference variables
	Configuration* conf;
	
	// Private computation methods.
	void initializePMLDomains();
	void computeDeltaP();
	void computeLEE();
	void applyWindowing();
};

#endif