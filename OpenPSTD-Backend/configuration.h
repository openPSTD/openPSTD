#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <vector>

struct Domain {
	// Position and size of the domain
	int left;
	int top;
	int width;
	int height;
	
	// Absorption values of the walls
	double absorptionTop;
	double absorptionBottom;
	double absorptionLeft;
	double absorptionRight;
	
	// ELR values of the walls
	bool elrTop;
	bool elrBottom;
	bool elrLeft;
	bool elrRight;
	
	// Whether or not this domain is a PML domain
	bool isPML;
};

struct Source {
	// Position of the source
	int position_x;
	int position_y;
};

struct Receiver {
	// Position of the receiver
	int position_x;
	int position_y;
};

struct Configuration {
	// Vector of all domains
	std::vector<Domain> domains;
	
	// Vector of all sources
	std::vector<Source> sources;
	
	// Vector of all receivers
	std::vector<Receiver> receivers;
	
	// PSTD Settings
	double gridSpacing;
	int windowSize;
	double renderTime;
	double airDensity;
	int soundSpeed;
	int pmlCells;
	int attenuation;
};

#endif