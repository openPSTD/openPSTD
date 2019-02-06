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
 * TODO: Currently two overlapping PML domains are created in concave
 * corners, and no PML domains are created in convex corners. Is this
 * correct?
 */
void Solver::initializePMLDomains() {
	// Compute a list of all non-shared wall segments
	std::vector<Line> segments = SolverLib::computeBorders(&conf->domains);
	
	// Loop through all segments
	for (unsigned int i = 0; i < segments.size(); i++) {
		// Create a new PML domain for this segment
		Domain d;
		d.isPML = true;
		if (segments[i].side == LEFT) {
			d.left = segments[i].p1.x - conf->pmlCells;
			d.bottom = segments[i].p1.y;
			d.width = conf->pmlCells;
			d.height = segments[i].p2.y - segments[i].p1.y;
		}
		if (segments[i].side == RIGHT) {
			d.left = segments[i].p1.x;
			d.bottom = segments[i].p1.y;
			d.width = conf->pmlCells;
			d.height = segments[i].p2.y - segments[i].p1.y;
		}
		if (segments[i].side == TOP) {
			d.left = segments[i].p1.x;
			d.bottom = segments[i].p1.y;
			d.width = segments[i].p2.x - segments[i].p1.x;
			d.height = conf->pmlCells;
		}
		if (segments[i].side == BOTTOM) {
			d.left = segments[i].p1.x;
			d.bottom = segments[i].p1.y - conf->pmlCells;
			d.width = segments[i].p2.x - segments[i].p1.x;
			d.height = conf->pmlCells;
		}
		conf->domains.push_back(d);
		
		// Output the coordinates of the added PML domain
		if (conf->debug) {
			std::cout << "Adding PML domain: ";
			std::cout << "  (" << d.left << ", " << d.bottom << ") -> (";
			std::cout << (d.left+d.width) << ", " << (d.bottom+d.height) << ")" << std::endl;
		}
	}
}

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