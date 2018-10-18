#ifndef FRAME_H
#define FRAME_H

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include "modelmanager.h"

/**
 * Represents a single domain inside a single frame.
 */
class Frame_Domain {
public:
	// Constructor
	Frame_Domain(std::string filename);
	
	// Get method for a sample
	double getSample(unsigned int x, unsigned int y);
	
	// Data vector
	std::vector<double> samples;
	
	// Width and height of the domain according to the kernel grid
	unsigned int width;
	unsigned int height;
};

/**
 * Represents a single simulated frame across all domains.
 */
class Frame {
public:
	// Constructor
	Frame(int frameID);
	
	// Get method for a sample
	double getSample(unsigned int x, unsigned int y, unsigned int domainID);
	
	// Get method for the width and height of the domains
	unsigned int getWidth(unsigned int domainID) { return domains[domainID].width; }
	unsigned int getHeight(unsigned int domainID) { return domains[domainID].height; }
private:
	// Data vector
	std::vector<Frame_Domain> domains;
	
	// Fills the domains vector with Frame_Domain instances
	void readDomainFile(std::string filename, int i);
};

#endif